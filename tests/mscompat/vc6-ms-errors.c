/** @file vc6-ms-errors.c @brief Checks selected Microsoft error names. */
#define WCRT_MS_ERRNO 1
#include <errno.h>
#include <errno.h>
#include <io.h>
#include <stdio.h>

#if !defined(WCRT_POSIX)
#ifdef EPERM
#error Microsoft error-name selector exposed unrelated POSIX names
#endif
#ifdef ENOSYS
#error Microsoft error-name selector exposed unrelated POSIX names
#endif
#endif
typedef char ms_error_values[EIO == 5 && EFBIG == 27 ? 1 : -1];

/** @brief Checks runtime error writes through the existing public lvalue. */
int check_vc6_ms_errors(void)
{
    errno = EIO;
    if (errno != 5 || _telli64(-1) != -1 || errno != EBADF) return 1;
    errno = EFBIG;
    return errno != 27;
}
