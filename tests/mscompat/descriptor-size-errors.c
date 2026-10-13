/** @file descriptor-size-errors.c @brief Injects metadata size boundaries. */
#define WCRT_POSIX 1
#define _fstat64 test_size_status
#include "../../src/stdio.c"

static wcrt_ms_int64 provided_size;
static int provided_mode = S_IFREG;
static int provided_error;

/** @brief Supplies deterministic metadata without touching the filesystem. */
int test_size_status(int descriptor, struct _stat64 *status)
{
    if (descriptor != 7) {
        errno = EBADF;
        return -1;
    }
    if (provided_error) {
        errno = provided_error;
        return -1;
    }
    memset(status, 0, sizeof(*status));
    status->st_mode = (unsigned short)provided_mode;
    status->st_size = provided_size;
    return 0;
}

int main(void)
{
    provided_size = 2147483647LL;
    errno = EDOM;
    if (_filelength(7) != 2147483647L || errno != EDOM) return 1;
    provided_size = 2147483648LL;
    if (_filelength(7) != -1 || errno != EFBIG ||
        _filelengthi64(7) != provided_size) return 2;
    provided_size = 9223372036854775807LL;
    errno = EDOM;
    if (_filelengthi64(7) != provided_size || errno != EDOM ||
        _filelength(7) != -1 || errno != EFBIG) return 3;
    provided_size = -1;
    if (_filelengthi64(7) != -1 || errno != EIO ||
        _filelength(7) != -1 || errno != EIO) return 4;
    provided_size = 0;
    provided_mode = S_IFDIR;
    if (_filelengthi64(7) != -1 || errno != ESPIPE) return 5;
    provided_mode = S_IFCHR;
    if (_filelength(7) != -1 || errno != ESPIPE) return 6;
    provided_mode = S_IFIFO;
    if (_filelengthi64(7) != -1 || errno != ESPIPE) return 7;
    provided_error = EACCES;
    if (_filelength(7) != -1 || errno != EACCES) return 8;
    if (_filelengthi64(8) != -1 || errno != EBADF) return 9;
    return 0;
}
