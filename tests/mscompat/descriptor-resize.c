/** @file descriptor-resize.c @brief Tests raw resizing of small owned files. */
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include "../../src/internal/file.h"

int main(int argc, char **argv)
{
    FILE *stream;
    struct wcrt_file before;
    unsigned char bytes[1100];
    int descriptor;
    int duplicate;
    int readonly;
    int pipes[2];
    int index;
    if (argc != 2) return 1;
    stream = fopen(argv[1], "w+b");
    if (stream == NULL || fputs("abc", stream) != 0 ||
        fseek(stream, 1, SEEK_SET) != 0 || ungetc('Z', stream) != 'Z') return 2;
    descriptor = _fileno(stream);
    stream->end_of_file = stream->error = 1;
    before = *stream;
    errno = EDOM;
    if (_chsize(descriptor, 1100) || errno != EDOM ||
        _filelength(descriptor) != 1100 || _telli64(descriptor) != 1 ||
        memcmp(&before, stream, sizeof(before))) return 3;
    duplicate = _dup(descriptor);
    if (duplicate < 0 || _lseeki64(duplicate, 0, SEEK_SET) != 0 ||
        _read(duplicate, bytes, sizeof(bytes)) != sizeof(bytes) ||
        memcmp(bytes, "abc", 3)) return 4;
    for (index = 3; index < sizeof(bytes); ++index) {
        if (bytes[index] != 0) return 5;
    }
    if (_lseeki64(duplicate, 8589934591LL, SEEK_SET) != 8589934591LL)
        return 6;
    errno = EDOM;
    if (_chsize(descriptor, 2) || errno != EDOM ||
        _filelength(duplicate) != 2 || _telli64(duplicate) != 8589934591LL ||
        memcmp(&before, stream, sizeof(before))) return 7;
    if (_chsize(descriptor, 2) || _chsize(descriptor, 0) ||
        _filelength(descriptor) != 0 || _telli64(descriptor) != 8589934591LL)
        return 8;
    if (_chsize(descriptor, -1) != -1 || errno != EINVAL ||
        _filelength(descriptor) != 0) return 9;
    readonly = _open(argv[1], _O_RDONLY | _O_BINARY);
    if (readonly < 0 || _chsize(readonly, 1) != -1 || errno != EACCES ||
        _close(readonly)) return 10;
    if (_pipe(pipes, 0, _O_BINARY) || _chsize(pipes[1], 1) != -1 ||
        errno != EINVAL || _close(pipes[0]) || _close(pipes[1])) return 11;
    if (_chsize(-1, -1) != -1 || errno != EBADF || _close(duplicate) ||
        _chsize(duplicate, 1) != -1 || errno != EBADF) return 12;
    clearerr(stream);
    if (fclose(stream)) return 13;
    descriptor = _open(argv[1], _O_RDWR | _O_APPEND | _O_TEXT);
    if (descriptor < 0 || _chsize(descriptor, 1025) ||
        _telli64(descriptor) != 0 || _filelength(descriptor) != 1025 ||
        _chsize(descriptor, 1) || _chsize(descriptor, 3) ||
        _telli64(descriptor) != 0 || _read(descriptor, bytes, 3) != 3 ||
        bytes[0] || bytes[1] || bytes[2] || _close(descriptor)) return 14;
    return 0;
}
