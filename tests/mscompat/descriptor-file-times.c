/** @file descriptor-file-times.c @brief Tests borrowed descriptor times. */
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <time.h>
#include "../../src/internal/file.h"

/** @brief Checks metadata without changing the descriptor position. */
static int check_times(int descriptor, long long access, long long modified)
{
    struct _stat64 status;
    return _fstat64(descriptor, &status) == 0 && status.st_atime == access &&
        status.st_mtime == modified;
}

int main(int argc, char **argv)
{
    struct __utimbuf32 narrow;
    struct __utimbuf64 wide;
    FILE *stream;
    struct wcrt_file before;
    struct _stat64 current;
    time_t current_before;
    time_t current_after;
    int descriptor;
    int duplicate;
    int readonly;
    int pipes[2];
    int character;
    if (argc != 2) return 1;
    stream = fopen(argv[1], "w+b");
    if (stream == NULL || fputs("abc", stream) != 0 ||
        fseek(stream, 1, SEEK_SET) != 0) return 2;
    descriptor = _fileno(stream);
    if (ungetc('Z', stream) != 'Z') return 3;
    stream->end_of_file = 1;
    stream->error = 1;
    before = *stream;
    narrow.actime = 946684800L;
    narrow.modtime = 946684900L;
    errno = EDOM;
    if (_futime32(descriptor, &narrow) != 0 || errno != EDOM ||
        memcmp(&before, stream, sizeof(before)) || _telli64(descriptor) != 1 ||
        !check_times(descriptor, narrow.actime, narrow.modtime)) return 4;
    duplicate = _dup(descriptor);
    if (duplicate < 0) return 5;
    wide.actime = 2208988800LL;
    wide.modtime = 2208988900LL;
    if (_futime64(duplicate, &wide) != 0 ||
        !check_times(descriptor, wide.actime, wide.modtime) ||
        memcmp(&before, stream, sizeof(before))) return 6;
    narrow.modtime = -1;
    if (_futime32(descriptor, &narrow) != -1 || errno != EINVAL ||
        !check_times(descriptor, wide.actime, wide.modtime)) return 7;
    wide.actime = 32535216000LL;
    if (_futime64(descriptor, &wide) != -1 || errno != EINVAL ||
        !check_times(descriptor, 2208988800LL, wide.modtime)) return 8;
    errno = EDOM;
    current_before = time(NULL);
    if (_futime32(descriptor, NULL) != 0 || errno != EDOM ||
        _futime64(descriptor, NULL) != 0 || errno != EDOM ||
        memcmp(&before, stream, sizeof(before))) return 9;
    current_after = time(NULL);
    if (_fstat64(descriptor, &current) != 0 ||
        current.st_atime != current.st_mtime ||
        current.st_mtime < current_before ||
        current.st_mtime > current_after || current.st_size != 3) return 17;
    readonly = _open(argv[1], _O_RDONLY | _O_BINARY);
    narrow.actime = narrow.modtime = 946684800L;
    if (readonly < 0 || _futime32(readonly, &narrow) != -1 ||
        errno != EACCES || _close(readonly) != 0) return 10;
    if (_pipe(pipes, 0, _O_BINARY) != 0) return 11;
    if (_futime64(pipes[0], NULL) != -1 || errno != EINVAL ||
        _futime32(pipes[1], &narrow) != -1 || errno != EINVAL ||
        _close(pipes[0]) != 0 || _close(pipes[1]) != 0) return 12;
    character = _open("NUL", _O_RDWR | _O_BINARY);
    if (character < 0 || _futime64(character, NULL) != -1 ||
        errno != EINVAL || _close(character) != 0) return 13;
    if (_futime32(-1, NULL) != -1 || errno != EBADF ||
        _futime64(100000, &wide) != -1 || errno != EBADF) return 14;
    if (_close(duplicate) != 0 || _futime64(duplicate, NULL) != -1 ||
        errno != EBADF || _telli64(descriptor) != 1 ||
        memcmp(&before, stream, sizeof(before))) return 15;
    clearerr(stream);
    if (fgetc(stream) != 'Z' || fgetc(stream) != 'b' ||
        fclose(stream) != 0) return 16;
    return 0;
}
