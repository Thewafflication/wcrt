/**
 * @file utime.h
 * @brief Declares selected POSIX file timestamp updates.
 */

#ifndef WCRT_UTIME_H
#define WCRT_UTIME_H

#if defined(WCRT_POSIX)
#include <time.h>

/** @brief Last-access and last-modification seconds for utime. */
struct utimbuf {
    time_t actime;  /**< Last-access seconds since the Unix epoch. */
    time_t modtime; /**< Last-modification seconds since the Unix epoch. */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Updates a path's access and modification timestamps.
 * @param path Null-terminated path to update.
 * @param times Requested seconds, or null to select the current time.
 * @return Zero on success, or -1 with errno set on failure.
 */
int utime(const char *path, const struct utimbuf *times);

#ifdef __cplusplus
}
#endif
#endif

#endif
