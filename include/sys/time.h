/** @file sys/time.h @brief Declares selected POSIX wall-clock interfaces. */

#ifndef WCRT_SYS_TIME_H
#define WCRT_SYS_TIME_H

#if defined(WCRT_POSIX)

#include <sys/types.h>
#include <time.h>

struct timeval {
    time_t tv_sec; /**< Whole seconds since the Unix epoch. */
    suseconds_t tv_usec; /**< Microseconds within the second. */
};

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Obtains the current wall-clock time. */
int gettimeofday(struct timeval *time_value, void *timezone);

#ifdef __cplusplus
}
#endif

#endif

#endif
