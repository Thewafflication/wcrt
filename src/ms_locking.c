/** @file ms_locking.c @brief Implements descriptor byte-range locking. */
#define WCRT_MS_ERRNO 1
#include <io.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <windows.h>

/** @brief Maps native range-lock failures without losing contention errors. */
static int wcrt_lock_error(DWORD error)
{
    if (error == ERROR_INVALID_HANDLE) return EBADF;
    if (error == ERROR_INVALID_PARAMETER) return EINVAL;
    if (error == ERROR_NOT_ENOUGH_MEMORY || error == ERROR_OUTOFMEMORY)
        return ENOMEM;
    if (error == ERROR_ACCESS_DENIED || error == ERROR_LOCK_VIOLATION ||
        error == ERROR_NOT_LOCKED || error == ERROR_SHARING_VIOLATION)
        return EACCES;
    return EIO;
}

/** @brief Borrows a disk handle without seeking or changing stream state. */
int _locking(int descriptor, int mode, long count)
{
    int saved = errno;
    __wcrt_intptr_t native = _get_osfhandle(descriptor);
    wcrt_ms_int64 position;
    struct _stat64 status;
    DWORD type, low, high, error;
    int attempt, maximum;
    if (native == -1) return -1;
    if (mode < _LK_UNLCK || mode > _LK_NBRLCK || count < 0) {
        errno = EINVAL;
        return -1;
    }
    type = GetFileType((HANDLE)native);
    if (type == FILE_TYPE_UNKNOWN) {
        errno = wcrt_lock_error(GetLastError());
        return -1;
    }
    if (type != FILE_TYPE_DISK) { errno = EINVAL; return -1; }
    if (_fstat64(descriptor, &status)) return -1;
    if ((status.st_mode & 0xf000U) != _S_IFREG) {
        errno = EINVAL;
        return -1;
    }
    position = _telli64(descriptor);
    if (position < 0) return -1;
    if (!count) { errno = saved; return 0; }
    low = (DWORD)position;
    high = (DWORD)((wcrt_ms_uint64)position >> 32);
    if (mode == _LK_UNLCK) {
        if (!UnlockFile((HANDLE)native, low, high, (DWORD)count, 0)) {
            errno = wcrt_lock_error(GetLastError());
            return -1;
        }
        errno = saved;
        return 0;
    }
    maximum = mode == _LK_LOCK || mode == _LK_RLCK ? 10 : 1;
    for (attempt = 0; attempt < maximum; ++attempt) {
        if (LockFile((HANDLE)native, low, high, (DWORD)count, 0)) {
            errno = saved;
            return 0;
        }
        error = GetLastError();
        if (error != ERROR_LOCK_VIOLATION || maximum == 1) {
            errno = wcrt_lock_error(error);
            return -1;
        }
        if (attempt + 1 < maximum) Sleep(1000);
    }
    errno = EDEADLOCK;
    return -1;
}
