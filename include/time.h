/**
 * @file time.h
 * @brief Declares C89 calendar and processor-time facilities.
 */

#if defined(WCRT_VC6_TIME32)
#if !defined(__i386__) && !defined(_M_IX86)
#error WCRT_VC6_TIME32 requires an x86 consumer
#endif
#if defined(WCRT_POSIX)
#error WCRT_VC6_TIME32 cannot select the wide POSIX timestamp ABI
#endif
#endif

#ifndef WCRT_TIME_H
#define WCRT_TIME_H

#include <stddef.h>
#include <wcrt/restrict.h>
#include <wcrt/ms_types.h>

/** @brief Milliseconds of process execution time. */
typedef long clock_t;
/** @brief Signed seconds since 1970-01-01 00:00:00 UTC. */
#if defined(WCRT_VC6_TIME32)
typedef long time_t;
#define time wcrt_time32
#define mktime wcrt_mktime32
#define difftime wcrt_difftime32
#define gmtime wcrt_gmtime32
#define localtime wcrt_localtime32
#define ctime wcrt_ctime32
#else
typedef wcrt_ms_int64 time_t;
#endif

/** @brief Prevents Microsoft-compatible headers from redefining time_t. */
#ifndef _TIME_T_DEFINED
#define _TIME_T_DEFINED
#endif

#if defined(WCRT_POSIX)
/** @brief POSIX seconds and nanoseconds timestamp. */
struct timespec {
    time_t tv_sec; /**< Whole seconds. */
    long tv_nsec;  /**< Nanoseconds within the second. */
};
#endif

#define CLOCKS_PER_SEC 1000L

/** @brief Broken-down calendar time. */
struct tm {
    int tm_sec;   /**< Seconds after the minute. */
    int tm_min;   /**< Minutes after the hour. */
    int tm_hour;  /**< Hours since midnight. */
    int tm_mday;  /**< Day of the month. */
    int tm_mon;   /**< Months since January. */
    int tm_year;  /**< Years since 1900. */
    int tm_wday;  /**< Days since Sunday. */
    int tm_yday;  /**< Days since January 1. */
    int tm_isdst; /**< Positive in daylight time, zero otherwise. */
};

#ifdef __cplusplus
extern "C" {
#endif

clock_t clock(void);
#if !defined(WCRT_VC6_TIME32)
double difftime(time_t end, time_t beginning);
time_t mktime(struct tm *broken_down);
time_t time(time_t *destination);
char *ctime(const time_t *timer);
struct tm *gmtime(const time_t *timer);
struct tm *localtime(const time_t *timer);
#endif
char *asctime(const struct tm *broken_down);
size_t strftime(char *WCRT_RESTRICT destination, size_t capacity,
    const char *WCRT_RESTRICT format,
    const struct tm *WCRT_RESTRICT broken_down);

/** @brief Returns checked narrow current time, writing exactly one long. */
long __cdecl wcrt_time32(long *destination);
/** @brief Normalizes a copied calendar and commits only a narrow success. */
long __cdecl wcrt_mktime32(struct tm *broken_down);
/** @brief Computes narrow-time differences without integer overflow. */
double __cdecl wcrt_difftime32(long end, long beginning);
/** @brief Converts a nonnegative narrow Unix timestamp to shared UTC fields. */
struct tm *__cdecl wcrt_gmtime32(const long *timer);
/** @brief Converts a nonnegative narrow timestamp using Windows local time. */
struct tm *__cdecl wcrt_localtime32(const long *timer);
/** @brief Formats a narrow timestamp into the existing shared text buffer. */
char *__cdecl wcrt_ctime32(const long *timer);

#ifdef __cplusplus
}
#endif

#endif
