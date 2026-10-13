/** @file descriptor-locking.c @brief Tests owned native range locks. */
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../../src/internal/file.h"

/** @brief Verifies independent-handle contention, exact unlock and state. */
int main(int argc, char **argv)
{
    int first, second, readonly, pipes[2], null_descriptor;
    FILE *stream;
    FILE saved;
    wcrt_ms_int64 wide = 8589934591LL;
    if (argc != 2) return 1;
    first = _sopen(argv[1], _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY,
        _SH_DENYNO, _S_IREAD | _S_IWRITE);
    if (first < 0 || _write(first, "abcd", 4) != 4) return 2;
    second = _sopen(argv[1], _O_RDWR | _O_BINARY, _SH_DENYNO);
    if (second < 0 || _lseek(first, 2, SEEK_SET) != 2 ||
        _lseek(second, 2, SEEK_SET) != 2) return 3;
    stream = _fdopen(first, "r+b");
    if (!stream) return 4;
    stream->end_of_file = stream->error = 1;
    stream->pushback = 'Z';
    saved = *stream;
    errno = 123;
    if (_locking(first, _LK_NBLCK, 5) || errno != 123 ||
        _telli64(first) != 2 || memcmp(&saved, stream, sizeof(saved))) return 5;
    if (_locking(second, _LK_NBRLCK, 5) != -1 || errno != EACCES)
        return 6;
    if (_locking(first, _LK_UNLCK, 4) != -1 || errno != EACCES) return 7;
    if (_lseek(first, 7, SEEK_SET) != 7 ||
        _locking(first, _LK_NBLCK, 2) || _lseek(first, 2, SEEK_SET) != 2 ||
        _locking(first, _LK_UNLCK, 7) != -1 || errno != EACCES) return 21;
    if (_locking(first, _LK_UNLCK, 5)) return 8;
    if (_lseek(first, 7, SEEK_SET) != 7 || _locking(first, _LK_UNLCK, 2) ||
        _lseek(first, 2, SEEK_SET) != 2) return 22;
    if (_locking(second, _LK_LOCK, 5) || _locking(second, _LK_UNLCK, 5))
        return 9;
    if (_locking(first, _LK_RLCK, 5) || _locking(first, _LK_UNLCK, 5))
        return 10;
    if (_lseeki64(first, wide, SEEK_SET) != wide) return 11;
    saved = *stream;
    if (_locking(first, _LK_NBLCK, 7) || _telli64(first) != wide ||
        _filelength(first) != 4 || memcmp(&saved, stream, sizeof(saved)) ||
        _locking(first, _LK_UNLCK, 7)) return 12;
    readonly = _open(argv[1], _O_RDONLY | _O_BINARY);
    if (readonly < 0 || _locking(readonly, _LK_NBRLCK, 1) ||
        _locking(readonly, _LK_UNLCK, 1) || _close(readonly)) return 13;
    if (_locking(first, -1, 1) != -1 || errno != EINVAL ||
        _locking(first, _LK_LOCK, -1) != -1 || errno != EINVAL) return 14;
    errno = 123;
    if (_locking(first, _LK_UNLCK, 0) || errno != 123) return 15;
    if (_pipe(pipes, 0, _O_BINARY) ||
        _locking(pipes[0], _LK_NBLCK, 1) != -1 || errno != EINVAL ||
        _close(pipes[0]) || _close(pipes[1])) return 16;
    null_descriptor = _open("NUL", _O_RDWR | _O_BINARY);
    if (null_descriptor < 0 || _locking(null_descriptor, _LK_NBLCK, 1) != -1 ||
        errno != EINVAL || _close(null_descriptor)) return 17;
    if (_close(second) || fclose(stream)) return 18;
    if (_locking(first, _LK_LOCK, 1) != -1 || errno != EBADF ||
        _locking(-1, -1, -1) != -1 || errno != EBADF) return 19;
    if (remove(argv[1])) return 20;
    return 0;
}
