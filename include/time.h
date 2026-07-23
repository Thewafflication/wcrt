/**
 * @file time.h
 * @brief Declares C89 calendar and processor-time facilities.
 */

#ifndef WCRT_TIME_H
#define WCRT_TIME_H

#include <stddef.h>

/** @brief Milliseconds of process execution time. */
typedef long clock_t;
/** @brief Signed seconds since 1970-01-01 00:00:00 UTC. */
typedef long long time_t;

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
double difftime(time_t end, time_t beginning);
time_t mktime(struct tm *broken_down);
time_t time(time_t *destination);
char *asctime(const struct tm *broken_down);
char *ctime(const time_t *timer);
struct tm *gmtime(const time_t *timer);
struct tm *localtime(const time_t *timer);
size_t strftime(char *destination, size_t capacity, const char *format,
    const struct tm *broken_down);

#ifdef __cplusplus
}
#endif

#endif
