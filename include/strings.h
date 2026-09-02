/** @file strings.h @brief Declares selected POSIX string interfaces. */

#ifndef WCRT_STRINGS_H
#define WCRT_STRINGS_H

#if defined(WCRT_POSIX)

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Compares strings without regard to case. */
int strcasecmp(const char *left, const char *right);
/** @brief Compares bounded strings without regard to case. */
int strncasecmp(const char *left, const char *right, size_t count);

#ifdef __cplusplus
}
#endif

#endif

#endif
