/** @file descriptor-locking.c @brief Checks range-locking declarations. */
#include <io.h>
#include <io.h>
#include <sys/locking.h>
#include <sys/locking.h>
#include <errno.h>
int (*lock_descriptor)(int, int, long) = _locking;
typedef char mode_values[_LK_UNLCK == 0 && _LK_LOCK == 1 &&
    _LK_NBLCK == 2 && _LK_RLCK == 3 && _LK_NBRLCK == 4 ? 1 : -1];
typedef char blocking_error[EDEADLOCK == 36 ? 1 : -1];
