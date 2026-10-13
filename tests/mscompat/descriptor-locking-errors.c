/** @file descriptor-locking-errors.c @brief Provides lock failure controls. */
#define WCRT_MS_ERRNO 1
#include <io.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <windows.h>
__wcrt_intptr_t fixture_handle(int descriptor);
wcrt_ms_int64 fixture_position(int descriptor);
int fixture_status(int descriptor, struct _stat64 *status);
DWORD WINAPI fixture_type(HANDLE handle);
DWORD WINAPI fixture_error(void);
BOOL WINAPI fixture_lock(HANDLE handle, DWORD low, DWORD high,
    DWORD length, DWORD length_high);
BOOL WINAPI fixture_unlock(HANDLE handle, DWORD low, DWORD high,
    DWORD length, DWORD length_high);
void WINAPI fixture_sleep(DWORD milliseconds);
#define _get_osfhandle fixture_handle
#define _telli64 fixture_position
#define _fstat64 fixture_status
#define GetFileType fixture_type
#define GetLastError fixture_error
#define LockFile fixture_lock
#define UnlockFile fixture_unlock
#define Sleep fixture_sleep
#include "../../src/ms_locking.c"

static int invalid, status_failure, position_failure, bad_call;
static int locks, unlocks, sleeps, failures, fail_unlock;
static DWORD type_value = FILE_TYPE_DISK, error_value = ERROR_LOCK_VIOLATION;
static unsigned short mode_value = _S_IFREG;
static wcrt_ms_int64 position_value = 8589934591LL;
static DWORD length_value = 2147483647UL;
static __wcrt_intptr_t handle_value;

/** @brief Validates the live-descriptor precedence. */
__wcrt_intptr_t fixture_handle(int descriptor)
{
    if (descriptor != 7 || invalid) { errno = EBADF; return -1; }
    return handle_value;
}
/** @brief Returns a physical position without modifying it. */
wcrt_ms_int64 fixture_position(int descriptor)
{
    (void)descriptor;
    if (position_failure) { errno = EIO; return -1; }
    return position_value;
}
/** @brief Supplies a regular-file or rejected-directory status record. */
int fixture_status(int descriptor, struct _stat64 *status)
{
    (void)descriptor;
    if (status_failure) { errno = EIO; return -1; }
    status->st_mode = mode_value;
    return 0;
}
/** @brief Returns the selected device type. */
DWORD WINAPI fixture_type(HANDLE handle)
{
    if ((__wcrt_intptr_t)handle != handle_value) ++bad_call;
    return type_value;
}
/** @brief Returns the current provider error. */
DWORD WINAPI fixture_error(void) { return error_value; }
/** @brief Verifies all split native range arguments. */
static void verify_range(HANDLE handle, DWORD low, DWORD high,
    DWORD length, DWORD length_high)
{
    if ((__wcrt_intptr_t)handle != handle_value ||
        low != (DWORD)position_value ||
        high != (DWORD)((wcrt_ms_uint64)position_value >> 32) ||
        length != length_value || length_high) ++bad_call;
}
/** @brief Simulates a bounded number of contention failures. */
BOOL WINAPI fixture_lock(HANDLE handle, DWORD low, DWORD high,
    DWORD length, DWORD length_high)
{
    verify_range(handle, low, high, length, length_high);
    ++locks;
    return locks > failures;
}
/** @brief Simulates exact-range unlock success or failure. */
BOOL WINAPI fixture_unlock(HANDLE handle, DWORD low, DWORD high,
    DWORD length, DWORD length_high)
{
    verify_range(handle, low, high, length, length_high);
    ++unlocks;
    return !fail_unlock;
}
/** @brief Counts waits without consuming real wall-clock time. */
void WINAPI fixture_sleep(DWORD milliseconds)
{
    if (milliseconds != 1000) ++bad_call;
    ++sleeps;
    errno = 999;
}
/** @brief Resets only retry counters and seeded errno. */
static void reset(void) { locks = sleeps = 0; errno = 123; }

/** @brief Exercises contention, errors, validation and wide range arguments. */
int main(void)
{
    handle_value = (__wcrt_intptr_t)(1ULL <<
        (sizeof(__wcrt_intptr_t) * 8 - 1));
    reset();
    if (_locking(7, _LK_NBLCK, 2147483647L) || errno != 123 ||
        locks != 1 || sleeps || bad_call) return 1;
    failures = 99;
    reset();
    if (_locking(7, _LK_NBRLCK, 2147483647L) != -1 || errno != EACCES ||
        locks != 1 || sleeps) return 2;
    reset();
    if (_locking(7, _LK_LOCK, 2147483647L) != -1 || errno != EDEADLOCK ||
        locks != 10 || sleeps != 9) return 3;
    failures = 2;
    reset();
    if (_locking(7, _LK_RLCK, 2147483647L) || errno != 123 ||
        locks != 3 || sleeps != 2) return 4;
    failures = 99;
    error_value = ERROR_ACCESS_DENIED;
    reset();
    if (_locking(7, _LK_LOCK, 2147483647L) != -1 || errno != EACCES ||
        locks != 1 || sleeps) return 5;
    error_value = ERROR_INVALID_HANDLE;
    if (_locking(7, _LK_NBLCK, 2147483647L) != -1 || errno != EBADF)
        return 6;
    error_value = ERROR_NOT_ENOUGH_MEMORY;
    if (_locking(7, _LK_NBLCK, 2147483647L) != -1 || errno != ENOMEM)
        return 7;
    error_value = ERROR_INVALID_PARAMETER;
    if (_locking(7, _LK_NBLCK, 2147483647L) != -1 || errno != EINVAL)
        return 8;
    error_value = 12345;
    if (_locking(7, _LK_NBLCK, 2147483647L) != -1 || errno != EIO)
        return 9;
    fail_unlock = 1;
    error_value = ERROR_NOT_LOCKED;
    reset();
    if (_locking(7, _LK_UNLCK, 2147483647L) != -1 || errno != EACCES ||
        unlocks != 1 || locks || sleeps) return 10;
    fail_unlock = 0;
    errno = 123;
    if (_locking(7, _LK_UNLCK, 2147483647L) || errno != 123) return 11;
    position_value = 9223372036854775807LL;
    failures = 0;
    reset();
    if (_locking(7, _LK_NBLCK, 2147483647L) || errno != 123 || bad_call ||
        _locking(7, _LK_UNLCK, 2147483647L)) return 20;
    reset();
    if (_locking(7, _LK_UNLCK, 0) || errno != 123 || locks || sleeps)
        return 12;
    if (_locking(7, 5, 1) != -1 || errno != EINVAL ||
        _locking(7, _LK_LOCK, -2147483647L - 1) != -1 || errno != EINVAL)
        return 13;
    invalid = 1;
    if (_locking(7, -1, -1) != -1 || errno != EBADF) return 14;
    invalid = 0;
    type_value = FILE_TYPE_UNKNOWN;
    error_value = ERROR_INVALID_HANDLE;
    if (_locking(7, _LK_LOCK, 1) != -1 || errno != EBADF) return 21;
    type_value = FILE_TYPE_PIPE;
    if (_locking(7, _LK_LOCK, 1) != -1 || errno != EINVAL) return 15;
    type_value = FILE_TYPE_DISK;
    mode_value = _S_IFDIR;
    if (_locking(7, _LK_LOCK, 1) != -1 || errno != EINVAL) return 16;
    mode_value = _S_IFREG;
    status_failure = 1;
    if (_locking(7, _LK_LOCK, 1) != -1 || errno != EIO) return 17;
    status_failure = 0;
    position_failure = 1;
    if (_locking(7, _LK_LOCK, 1) != -1 || errno != EIO || locks || sleeps)
        return 18;
    if (bad_call) return 19;
    return 0;
}
