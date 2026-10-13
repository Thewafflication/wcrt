/**
 * @file shared-stream.c
 * @brief Tests sharing restrictions and duplicate lifetime on owned files.
 */

#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <stdio.h>
#include <string.h>

/** @brief Tests one additional descriptor open without changing contents. */
static int check_open(const char *path, int access, int allowed)
{
    int descriptor;
    errno = EDOM;
    descriptor = _open(path, access | _O_BINARY);
    if (allowed) {
        if (descriptor < 0 || errno != EDOM) return 1;
        return _close(descriptor) != 0;
    }
    if (descriptor >= 0) {
        _close(descriptor);
        return 1;
    }
    return errno != EACCES;
}

int main(int argument_count, char **arguments)
{
    static const int modes[] = {
        _SH_DENYRW, _SH_DENYWR, _SH_DENYRD, _SH_DENYNO
    };
    FILE *stream;
    FILE *other;
    char buffer[4];
    int index;
    int duplicate;
    int descriptor;

    if (argument_count != 2) return 1;
    stream = _fsopen(arguments[1], "w+b", _SH_DENYNO);
    if (stream == NULL) return 2;
    if (fwrite("abc", 1, 3, stream) != 3 || fclose(stream) != 0) return 3;
    for (index = 0; index < 4; ++index) {
        errno = EDOM;
        stream = _fsopen(arguments[1], "r+b", modes[index]);
        if (stream == NULL || errno != EDOM) return 4;
        if (check_open(arguments[1], _O_RDONLY,
            modes[index] == _SH_DENYWR || modes[index] == _SH_DENYNO)) return 5;
        if (check_open(arguments[1], _O_WRONLY,
            modes[index] == _SH_DENYRD || modes[index] == _SH_DENYNO)) return 6;
        if (check_open(arguments[1], _O_RDWR, modes[index] == _SH_DENYNO))
            return 7;
        errno = EDOM;
        other = fopen(arguments[1], "rb");
        if (modes[index] == _SH_DENYWR || modes[index] == _SH_DENYNO) {
            if (other == NULL || fclose(other) != 0) return 8;
        } else if (other != NULL || errno != EACCES) return 9;
        if (fclose(stream) != 0) return 10;
    }
    stream = _fsopen(arguments[1], "r+b", _SH_DENYRW);
    if (stream == NULL) return 11;
    duplicate = _dup(_fileno(stream));
    if (duplicate < 0 || fclose(stream) != 0) return 12;
    if (check_open(arguments[1], _O_RDONLY, 0)) return 13;
    if (_close(duplicate) != 0 || check_open(arguments[1], _O_RDWR, 1))
        return 14;
    for (index = 0; index < 100; ++index) {
        errno = 0;
        if (_fsopen(arguments[1], "rb", -1) != NULL || errno != EINVAL)
            return 15;
    }
    if (_fsopen(NULL, "rb", _SH_DENYNO) != NULL || errno != EINVAL) return 16;
    if (_fsopen(arguments[1], NULL, _SH_DENYNO) != NULL || errno != EINVAL)
        return 17;
    if (_fsopen(arguments[1], "", _SH_DENYNO) != NULL || errno != EINVAL)
        return 18;
    if (_fsopen(arguments[1], "x", _SH_DENYNO) != NULL || errno != EINVAL)
        return 19;
    if (_fsopen(arguments[1], "wjunk", _SH_DENYNO) != NULL || errno != EINVAL)
        return 25;
    if (_fsopen(arguments[1], "r++", _SH_DENYNO) != NULL || errno != EINVAL)
        return 26;
    if (_fsopen(arguments[1], "rtb", _SH_DENYNO) != NULL || errno != EINVAL)
        return 27;
    if (_fsopen("", "rb", _SH_DENYNO) != NULL || errno != EINVAL) return 28;
    stream = _fsopen(arguments[1], "ab", _SH_DENYNO);
    if (stream == NULL || fputc('d', stream) != 'd' || fclose(stream) != 0)
        return 20;
    stream = _fsopen(arguments[1], "rb", _SH_DENYNO);
    if (stream == NULL || fread(buffer, 1, 4, stream) != 4 ||
        memcmp(buffer, "abcd", 4) != 0 || fclose(stream) != 0) return 21;
    descriptor = _open(arguments[1], _O_RDONLY | _O_BINARY);
    if (descriptor < 0 || _close(descriptor) != 0) return 22;
    if (remove(arguments[1]) != 0) return 23;
    errno = 0;
    if (_fsopen(arguments[1], "rb", _SH_DENYNO) != NULL || errno != ENOENT)
        return 24;
    return 0;
}
