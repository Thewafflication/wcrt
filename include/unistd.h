/** @file unistd.h @brief Declares selected POSIX process interfaces. */

#ifndef WCRT_UNISTD_H
#define WCRT_UNISTD_H

#if defined(WCRT_POSIX)

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Returns the calling process identifier. */
pid_t getpid(void);
/** @brief Removes one filesystem name. */
int unlink(const char *path);
/** @brief Tests path accessibility. */
int access(const char *path, int mode);
int close(int descriptor);
ssize_t read(int descriptor, void *buffer, size_t count);
ssize_t write(int descriptor, const void *buffer, size_t count);
off_t lseek(int descriptor, off_t offset, int origin);
int isatty(int descriptor);
int fsync(int descriptor);
int dup(int descriptor);
int dup2(int descriptor, int target);

#ifdef __cplusplus
}
#endif

#endif

#endif
