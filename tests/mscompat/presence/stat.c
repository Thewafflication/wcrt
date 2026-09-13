/** @file stat.c @brief Checks Microsoft file-status declarations. */
#include <stddef.h>
#include <sys/stat.h>
#ifndef WCRT_SYS_STAT_H
#error <sys/stat.h> did not resolve to the WCRT header
#endif
/** @brief Verifies the Microsoft _stat64 structure size. */
typedef char stat64_size_check[sizeof(struct _stat64) == 56 ? 1 : -1];
/** @brief Verifies the 64-bit file-size member offset. */
typedef char stat64_size_offset_check[
    offsetof(struct _stat64, st_size) == 24 ? 1 : -1];
/** @brief Verifies the 64-bit access-time member offset. */
typedef char stat64_time_offset_check[
    offsetof(struct _stat64, st_atime) == 32 ? 1 : -1];
/** @brief Type of _stat64 and the compatible x86 alias. */
typedef int (__cdecl *stat64_fn)(const char *, struct _stat64 *);
/** @brief Forces type checking of _stat64. */
static stat64_fn stat64_address = _stat64;
/** @brief Type of descriptor-based _fstat64. */
typedef int (__cdecl *fstat64_fn)(int, struct _stat64 *);
/** @brief Forces type checking of _fstat64. */
static fstat64_fn fstat64_address = _fstat64;
/** @brief Type of Microsoft permission changes. */
typedef int (__cdecl *chmod_fn)(const char *, int);
/** @brief Forces type checking of _chmod. */
static chmod_fn chmod_address = _chmod;
#if defined(__i386__) || defined(_M_IX86)
/** @brief Forces type checking of the x86 _stat alias. */
static stat64_fn stat_address = _stat;
/** @brief Forces type checking of the x86 _fstat alias. */
static fstat64_fn fstat_address = _fstat;
#endif
