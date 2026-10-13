/**
 * @file descriptor-mode.c
 * @brief Tests descriptor translation using owned files and pipes.
 */

#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>

int main(int argument_count, char **arguments)
{
    char bytes[16];
    FILE *stream;
    int descriptor;
    int duplicate;
    int pipes[2];
    int previous;
    int index;
    int invalid[] = {0, _O_TEXT | _O_BINARY, _O_RDONLY | _O_APPEND, -1};

    if (argument_count != 2) return 1;
    descriptor = _open(arguments[1], _O_CREAT | _O_TRUNC | _O_RDWR |
        _O_BINARY, 0600);
    if (descriptor < 0) return 2;
    errno = EDOM;
    if (_setmode(descriptor, _O_TEXT) != _O_BINARY || errno != EDOM)
        return 3;
    if (_setmode(descriptor, _O_TEXT) != _O_TEXT) return 4;
    if (_write(descriptor, "A\n", 2) != 2 || _tell(descriptor) != 3)
        return 5;
    if (_setmode(descriptor, _O_BINARY) != _O_TEXT ||
        _tell(descriptor) != 3 || _write(descriptor, "B\n", 2) != 2)
        return 6;
    if (_lseek(descriptor, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 5) != 5 || memcmp(bytes, "A\r\nB\n", 5))
        return 7;
    if (_setmode(descriptor, _O_TEXT) != _O_BINARY ||
        _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 4) != 4 || memcmp(bytes, "A\nB\n", 4))
        return 8;
    duplicate = _dup(descriptor);
    if (duplicate < 0 || _setmode(duplicate, _O_BINARY) != _O_TEXT ||
        _setmode(descriptor, _O_TEXT) != _O_TEXT) return 9;
    if (_lseek(duplicate, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 2) != 2 || memcmp(bytes, "A\n", 2) ||
        _tell(duplicate) != 3 || _read(duplicate, bytes, 2) != 2 ||
        memcmp(bytes, "B\n", 2)) return 10;
    for (index = 0; index < 4; ++index) {
        errno = EDOM;
        if (_setmode(descriptor, invalid[index]) != -1 || errno != EINVAL)
            return 11;
        if (_setmode(descriptor, _O_TEXT) != _O_TEXT) return 12;
    }
    if (_close(duplicate) != 0 || _close(descriptor) != 0) return 13;
    errno = 0;
    if (_setmode(descriptor, _O_TEXT) != -1 || errno != EBADF) return 14;
    errno = 0;
    if (_setmode(-1, _O_TEXT) != -1 || errno != EBADF) return 15;
    errno = 0;
    if (_setmode(FOPEN_MAX + 3, 0) != -1 || errno != EBADF) return 16;

    stream = fopen(arguments[1], "ab");
    if (stream == NULL) return 17;
    descriptor = _fileno(stream);
    if (_setmode(descriptor, _O_TEXT) != _O_BINARY ||
        fputs("C\n", stream) == EOF || fflush(stream) != 0 ||
        _setmode(descriptor, _O_BINARY) != _O_TEXT ||
        fputs("D\n", stream) == EOF || fclose(stream) != 0) return 18;
    descriptor = _open(arguments[1], _O_RDONLY | _O_BINARY);
    if (descriptor < 0 || _read(descriptor, bytes, 10) != 10 ||
        memcmp(bytes, "A\r\nB\nC\r\nD\n", 10)) return 19;
    if (_setmode(descriptor, _O_TEXT) != _O_BINARY) return 20;
    errno = 0;
    if (_write(descriptor, "!", 1) != -1 || errno != EBADF ||
        _close(descriptor) != 0) return 21;

    if (_pipe(pipes, 128, _O_BINARY) != 0) return 22;
    if (_setmode(pipes[1], _O_TEXT) != _O_BINARY ||
        _write(pipes[1], "P\n", 2) != 2 || _close(pipes[1]) != 0 ||
        _read(pipes[0], bytes, 3) != 3 || memcmp(bytes, "P\r\n", 3) ||
        _close(pipes[0]) != 0) return 23;
    for (index = 0; index < 3; ++index) {
        if (_get_osfhandle(index) == (__wcrt_intptr_t)-1) continue;
        errno = EDOM;
        previous = _setmode(index, _O_BINARY);
        if ((previous != _O_TEXT && previous != _O_BINARY) ||
            errno != EDOM || _setmode(index, previous) != _O_BINARY)
            return 24;
    }
    if (remove(arguments[1]) != 0) return 25;
    return 0;
}
