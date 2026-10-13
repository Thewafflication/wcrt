/** @file descriptor-eof.c @brief Tests physical EOF with an owned file. */
#define WCRT_POSIX 1
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include "../../src/internal/file.h"

int main(int argument_count, char **arguments)
{
    int descriptor;
    int duplicate;
    int pipes[2];
    FILE *stream;
    unsigned char saved[sizeof(FILE)];
    if (argument_count != 2) return 1;
    descriptor = _open(arguments[1], _O_CREAT | _O_TRUNC | _O_RDWR |
        _O_BINARY, 0600);
    if (descriptor < 0) return 2;
    errno = EDOM;
    if (_eof(descriptor) != 1 || errno != EDOM ||
        _telli64(descriptor) != 0) return 3;
    if (_write(descriptor, "AB", 2) != 2 || _eof(descriptor) != 1 ||
        _lseeki64(descriptor, 0, SEEK_SET) != 0 || _eof(descriptor) != 0)
        return 4;
    duplicate = _dup(descriptor);
    if (duplicate < 0 || _lseeki64(duplicate, 0, SEEK_END) != 2 ||
        _eof(descriptor) != 1 || _telli64(duplicate) != 2) return 5;
    if (_lseeki64(descriptor, 8589934591LL, SEEK_SET) != 8589934591LL ||
        _eof(descriptor) != 0 || _filelength(descriptor) != 2 ||
        _telli64(duplicate) != 8589934591LL) return 6;
    if (_close(duplicate) != 0) return 7;
    stream = _fdopen(descriptor, "r+b");
    if (stream == NULL || fseek(stream, 0, SEEK_END) != 0 || feof(stream) ||
        _eof(descriptor) != 1 || feof(stream)) return 8;
    if (fgetc(stream) != EOF || !feof(stream)) return 9;
    memcpy(saved, stream, sizeof(FILE));
    if (_eof(descriptor) != 1 || memcmp(saved, stream, sizeof(FILE)))
        return 10;
    if (ungetc('X', stream) != 'X') return 11;
    stream->error = 1;
    memcpy(saved, stream, sizeof(FILE));
    if (_eof(descriptor) != 1 || memcmp(saved, stream, sizeof(FILE)) ||
        fgetc(stream) != 'X' || !ferror(stream)) return 12;
    if (fclose(stream) != 0) return 13;
    if (_eof(descriptor) != -1 || errno != EBADF ||
        _eof(-1) != -1 || errno != EBADF) return 14;
    if (_pipe(pipes, 0, _O_BINARY) != 0) return 15;
    if (_eof(pipes[0]) != -1 || errno != ESPIPE ||
        _eof(pipes[1]) != -1 || errno != ESPIPE) return 16;
    if (_close(pipes[0]) != 0 || _close(pipes[1]) != 0) return 17;
    descriptor = _open(arguments[1], _O_TRUNC | _O_RDWR | _O_TEXT);
    if (descriptor < 0 || _write(descriptor, "A\nB", 3) != 3 ||
        _telli64(descriptor) != 4 || _eof(descriptor) != 1 ||
        _lseeki64(descriptor, 3, SEEK_SET) != 3 || _eof(descriptor) != 0 ||
        _telli64(descriptor) != 3 || _close(descriptor) != 0) return 18;
    return 0;
}
