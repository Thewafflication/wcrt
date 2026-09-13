/**
 * @file io.h
 * @brief Provides the Microsoft low-level I/O compatibility header.
 */

#ifndef WCRT_IO_H
#define WCRT_IO_H

#include <stdio.h>

#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || \
    defined(_M_ARM64)
typedef long long __wcrt_intptr_t;
#else
typedef long __wcrt_intptr_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Removes one filesystem name. */
int _unlink(const char *path);
/** @brief Tests Microsoft path accessibility. */
int _access(const char *path, int mode);
/** @brief Returns the native Windows handle behind a WCRT descriptor. */
__wcrt_intptr_t _get_osfhandle(int descriptor);
int _close(int descriptor);
int _read(int descriptor, void *buffer, unsigned int count);
int _write(int descriptor, const void *buffer, unsigned int count);
long _lseek(int descriptor, long offset, int origin);
long _tell(int descriptor);
int _commit(int descriptor);
int _isatty(int descriptor);
FILE *_fdopen(int descriptor, const char *mode);
int _dup(int descriptor);
int _dup2(int descriptor, int target);
int _pipe(int descriptors[2], unsigned int size, int text_mode);

#ifdef __cplusplus
}
#endif

#endif
