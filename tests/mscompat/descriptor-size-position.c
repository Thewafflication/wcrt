/** @file descriptor-size-position.c @brief Tests owned descriptor queries. */
#define WCRT_POSIX 1
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include "../../src/internal/file.h"

int main(int argument_count, char **arguments)
{
    FILE *stream;
    int descriptor;
    int duplicate;
    int pipes[2];
    char byte;
    unsigned char saved_state[sizeof(FILE)];
    wcrt_ms_int64 far_position = 8589934591LL;
    if (argument_count != 2) return 1;
    descriptor = _open(arguments[1], _O_CREAT | _O_TRUNC | _O_RDWR |
        _O_BINARY, 0600);
    if (descriptor < 0) return 2;
    errno = EDOM;
    if (_filelength(descriptor) != 0 || _filelengthi64(descriptor) != 0 ||
        _telli64(descriptor) != 0 || errno != EDOM) return 3;
    if (_write(descriptor, "ABC", 3) != 3 ||
        _filelength(descriptor) != 3 || _telli64(descriptor) != 3) return 4;
    duplicate = _dup(descriptor);
    if (duplicate < 0 || _lseeki64(duplicate, far_position, SEEK_SET) !=
        far_position || _telli64(descriptor) != far_position ||
        _filelengthi64(descriptor) != 3 ||
        _telli64(duplicate) != far_position) return 5;
    if (_lseeki64(descriptor, -1, SEEK_SET) != -1 || errno != EINVAL ||
        _telli64(descriptor) != far_position) return 6;
    if (_lseeki64(descriptor, 0, 99) != -1 || errno != EINVAL ||
        _telli64(descriptor) != far_position) return 7;
    if (_lseeki64(descriptor, -1, SEEK_END) != 2 ||
        _lseeki64(duplicate, -1, SEEK_CUR) != 1 ||
        _read(descriptor, &byte, 1) != 1 || byte != 'B') return 8;
    if (_close(duplicate) != 0) return 9;
    stream = _fdopen(descriptor, "r+b");
    if (stream == NULL || fseek(stream, 0, SEEK_END) != 0 ||
        fgetc(stream) != EOF || !feof(stream)) return 10;
    errno = EDOM;
    memcpy(saved_state, stream, sizeof(FILE));
    if (_filelength(descriptor) != 3 || _filelengthi64(descriptor) != 3 ||
        _telli64(descriptor) != 3 || !feof(stream) || errno != EDOM ||
        memcmp(saved_state, stream, sizeof(FILE)))
        return 11;
    if (ungetc('X', stream) != 'X') return 12;
    stream->error = 1;
    memcpy(saved_state, stream, sizeof(FILE));
    if (_filelengthi64(descriptor) != 3 || _telli64(descriptor) != 3 ||
        memcmp(saved_state, stream, sizeof(FILE)) ||
        fgetc(stream) != 'X' || !ferror(stream)) return 13;
    clearerr(stream);
    if (ungetc('Y', stream) != 'Y' ||
        _lseeki64(descriptor, 0, SEEK_SET) != 0 || feof(stream) ||
        fgetc(stream) != 'A') return 14;
    if (fclose(stream) != 0) return 15;
    errno = EDOM;
    if (_filelength(descriptor) != -1 || errno != EBADF ||
        _filelengthi64(-1) != -1 || errno != EBADF ||
        _telli64(-1) != -1 || errno != EBADF ||
        _lseeki64(-1, 0, SEEK_SET) != -1 || errno != EBADF) return 16;
    if (_pipe(pipes, 0, _O_BINARY) != 0) return 17;
    if (_filelength(pipes[0]) != -1 || errno != ESPIPE ||
        _filelengthi64(pipes[1]) != -1 || errno != ESPIPE ||
        _telli64(pipes[0]) != -1 || errno != ESPIPE ||
        _lseeki64(pipes[1], 0, SEEK_CUR) != -1 || errno != ESPIPE)
        return 18;
    if (_close(pipes[0]) != 0 || _close(pipes[1]) != 0) return 19;
    descriptor = _open(arguments[1], _O_TRUNC | _O_RDWR | _O_TEXT);
    if (descriptor < 0 || _write(descriptor, "A\nB", 3) != 3 ||
        _filelength(descriptor) != 4 || _telli64(descriptor) != 4)
        return 20;
    if (_close(descriptor) != 0) return 21;
    descriptor = _open(arguments[1], _O_RDONLY | _O_BINARY);
    if (descriptor < 0 || _filelength(descriptor) != 4 ||
        _lseeki64(descriptor, 0, SEEK_END) != 4 ||
        _telli64(descriptor) != 4 || _close(descriptor) != 0) return 22;
    descriptor = _open(arguments[1], _O_WRONLY | _O_BINARY);
    if (descriptor < 0 || _filelengthi64(descriptor) != 4 ||
        _telli64(descriptor) != 0 || _close(descriptor) != 0) return 23;
    return 0;
}
