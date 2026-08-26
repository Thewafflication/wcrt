/** @file profile.c @brief Checks selected POSIX error declarations. */
#define WCRT_POSIX 1
#include <errno.h>
#include <time.h>
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
#if EFBIG != 27
#error EFBIG does not match the selected Windows ABI
#endif
#if ENOSYS != 40
#error ENOSYS does not match the selected Windows ABI
#endif
