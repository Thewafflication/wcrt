/** @file unistd.h @brief Declares selected POSIX process interfaces. */

#ifndef WCRT_UNISTD_H
#define WCRT_UNISTD_H

#if defined(WCRT_POSIX)

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Returns the calling process identifier. */
pid_t getpid(void);

#ifdef __cplusplus
}
#endif

#endif

#endif
