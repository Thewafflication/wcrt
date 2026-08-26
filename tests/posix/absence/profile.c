/** @file profile.c @brief Checks strict isolation of POSIX error names. */
#include <errno.h>
#include <time.h>
#include <errno.h>
#include <time.h>

#ifdef EPERM
#error EPERM leaked into the strict WCRT profile
#endif
#ifdef EINTR
#error EINTR leaked into the strict WCRT profile
#endif
#ifdef EIO
#error EIO leaked into the strict WCRT profile
#endif
#ifdef ENXIO
#error ENXIO leaked into the strict WCRT profile
#endif
#ifdef EAGAIN
#error EAGAIN leaked into the strict WCRT profile
#endif
#ifdef EFBIG
#error EFBIG leaked into the strict WCRT profile
#endif
#ifdef ENOSYS
#error ENOSYS leaked into the strict WCRT profile
#endif
#ifdef _POSIX_VERSION
#error The strict WCRT profile advertised _POSIX_VERSION
#endif
