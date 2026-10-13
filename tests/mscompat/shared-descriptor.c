/**
 * @file shared-descriptor.c
 * @brief Tests shared descriptors and owned-file creation permissions.
 */

#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/** @brief Checks an additional open against the selected sharing policy. */
static int check_open(const char *path, int access, int allowed)
{
    int descriptor;
    errno = EDOM;
    descriptor = _sopen(path, access | _O_BINARY, _SH_DENYNO);
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
    char bytes[8];
    struct _stat64 status;
    int descriptor;
    int duplicate;
    int index;

    if (argument_count != 2) return 1;
    errno = EDOM;
    descriptor = _sopen(arguments[1], _O_CREAT | _O_EXCL | _O_RDWR |
        _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
    if (descriptor < 0 || errno != EDOM ||
        _write(descriptor, "abc", 3) != 3 || _close(descriptor) != 0)
        return 2;
    for (index = 0; index < 4; ++index) {
        descriptor = _sopen(arguments[1], _O_RDWR | _O_BINARY, modes[index]);
        if (descriptor < 0) return 3;
        if (check_open(arguments[1], _O_RDONLY,
            modes[index] == _SH_DENYWR || modes[index] == _SH_DENYNO)) return 4;
        if (check_open(arguments[1], _O_WRONLY,
            modes[index] == _SH_DENYRD || modes[index] == _SH_DENYNO)) return 5;
        if (check_open(arguments[1], _O_RDWR, modes[index] == _SH_DENYNO))
            return 6;
        if (_close(descriptor) != 0) return 7;
    }
    descriptor = _sopen(arguments[1], _O_RDWR, _SH_DENYRW);
    duplicate = _dup(descriptor);
    if (descriptor < 0 || duplicate < 0 || _close(descriptor) != 0 ||
        check_open(arguments[1], _O_RDONLY, 0) ||
        _close(duplicate) != 0 || check_open(arguments[1], _O_RDWR, 1))
        return 8;

    for (index = 0; index < 100; ++index) {
        errno = 0;
        if (_sopen(arguments[1], _O_TRUNC | _O_WRONLY, -1) != -1 ||
            errno != EINVAL) return 9;
    }
    errno = 0;
    if (_sopen(arguments[1], _O_CREAT | _O_TRUNC | _O_WRONLY,
        _SH_DENYNO, _S_IEXEC) != -1 || errno != EINVAL) return 10;
    errno = 0;
    if (_sopen(arguments[1], _O_CREAT | _O_EXCL | _O_WRONLY,
        _SH_DENYNO, _S_IWRITE) != -1 || errno != EEXIST) return 11;
    errno = 0;
    if (_sopen(NULL, _O_RDONLY, _SH_DENYNO) != -1 || errno != EINVAL)
        return 12;
    errno = 0;
    if (_sopen("", _O_RDONLY, _SH_DENYNO) != -1 || errno != EINVAL)
        return 13;
    errno = 0;
    if (_sopen(arguments[1], 3, _SH_DENYNO) != -1 || errno != EINVAL)
        return 14;
    errno = 0;
    if (_sopen(arguments[1], _O_TEXT | _O_BINARY, _SH_DENYNO) != -1 ||
        errno != EINVAL) return 15;
    errno = 0;
    if (_sopen(arguments[1], 0x100000, _SH_DENYNO) != -1 || errno != EINVAL)
        return 16;
    descriptor = _sopen(arguments[1], _O_RDONLY | _O_BINARY, _SH_DENYNO);
    if (descriptor < 0 || _read(descriptor, bytes, 3) != 3 ||
        memcmp(bytes, "abc", 3) || _close(descriptor) != 0) return 17;

    descriptor = _sopen(arguments[1], _O_CREAT | _O_RDWR, _SH_DENYNO,
        _S_IREAD);
    if (descriptor < 0 || _close(descriptor) != 0 ||
        _stat64(arguments[1], &status) != 0 ||
        !(status.st_mode & _S_IWRITE)) return 18;
    descriptor = _sopen(arguments[1], _O_CREAT | _O_TRUNC | _O_RDWR,
        _SH_DENYNO, _S_IREAD);
    if (descriptor < 0 || _write(descriptor, "T\n", 2) != 2 ||
        _close(descriptor) != 0 || _stat64(arguments[1], &status) != 0 ||
        !(status.st_mode & _S_IWRITE)) return 19;
    descriptor = _sopen(arguments[1], _O_WRONLY | _O_APPEND | _O_BINARY,
        _SH_DENYNO);
    if (descriptor < 0 || _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _write(descriptor, "B\n", 2) != 2 || _close(descriptor) != 0)
        return 20;
    descriptor = _sopen(arguments[1], _O_RDONLY | _O_BINARY, _SH_DENYNO);
    if (descriptor < 0 || _read(descriptor, bytes, 5) != 5 ||
        memcmp(bytes, "T\r\nB\n", 5) || _close(descriptor) != 0 ||
        remove(arguments[1]) != 0) return 21;

    for (index = 0; index < 2; ++index) {
        descriptor = _sopen(arguments[1], _O_CREAT | _O_RDWR |
            (index ? _O_EXCL : 0), _SH_DENYNO, _S_IREAD);
        if (descriptor < 0 || _write(descriptor, "R", 1) != 1 ||
            _close(descriptor) != 0 || _stat64(arguments[1], &status) != 0 ||
            (status.st_mode & _S_IWRITE)) return 22;
        if (check_open(arguments[1], _O_WRONLY, 0) ||
            check_open(arguments[1], _O_RDONLY, 1)) return 23;
        if (_chmod(arguments[1], _S_IREAD | _S_IWRITE) != 0 ||
            remove(arguments[1]) != 0) return 24;
    }
    errno = 0;
    if (_sopen(arguments[1], _O_RDONLY, _SH_DENYNO) != -1 ||
        errno != ENOENT) return 25;
    return 0;
}
