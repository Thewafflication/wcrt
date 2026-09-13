/** @file profile.c @brief Checks selected POSIX error declarations. */
#define WCRT_POSIX 1
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#ifndef WCRT_ERRNO_H
#error <errno.h> did not resolve to the WCRT header
#endif
#ifndef WCRT_TIME_H
#error <time.h> did not resolve to the WCRT header
#endif
#ifndef _TIME_T_DEFINED
#error WCRT did not publish the Microsoft-compatible time_t guard
#endif
#ifdef _POSIX_VERSION
#error A bounded WCRT profile must not advertise _POSIX_VERSION
#endif
typedef int (*posix_isascii_fn)(int);
static posix_isascii_fn posix_isascii_address = isascii;
int wcrt_posix_profile_probe(void)
{
    return posix_isascii_address == 0;
}
#if EPERM != 1
#error EPERM does not match the selected Windows ABI
#endif
#if EINTR != 4
#error EINTR does not match the selected Windows ABI
#endif
#if EIO != 5
#error EIO does not match the selected Windows ABI
#endif
#if ENXIO != 6
#error ENXIO does not match the selected Windows ABI
#endif
#if EAGAIN != 11
#error EAGAIN does not match the selected Windows ABI
#endif
#if EBADF != 9
#error EBADF does not match the selected Windows ABI
#endif
#if EBUSY != 16
#error EBUSY does not match the selected Windows ABI
#endif
#if EEXIST != 17
#error EEXIST does not match the selected Windows ABI
#endif
#if ENOTDIR != 20
#error ENOTDIR does not match the selected Windows ABI
#endif
#if EISDIR != 21
#error EISDIR does not match the selected Windows ABI
#endif
#if EFBIG != 27
#error EFBIG does not match the selected Windows ABI
#endif
#if ENOSPC != 28
#error ENOSPC does not match the selected Windows ABI
#endif
#if ESPIPE != 29
#error ESPIPE does not match the selected Windows ABI
#endif
#if EPIPE != 32
#error EPIPE does not match the selected Windows ABI
#endif
#if ENOSYS != 40
#error ENOSYS does not match the selected Windows ABI
#endif
#if ENOTEMPTY != 41
#error ENOTEMPTY does not match the selected Windows ABI
#endif
