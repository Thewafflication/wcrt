/** @file utime.c @brief Checks explicit-width utime declarations. */
#include <sys/utime.h>
#ifndef WCRT_SYS_UTIME_H
#error <sys/utime.h> did not resolve to the WCRT header
#endif
/** @brief Verifies the explicit 32-bit time-buffer size. */
typedef char utime32_size_check[
    sizeof(struct __utimbuf32) == 8 ? 1 : -1];
/** @brief Verifies the explicit 64-bit time-buffer size. */
typedef char utime64_size_check[
    sizeof(struct __utimbuf64) == 16 ? 1 : -1];
/** @brief Verifies the 32-bit modification-time offset. */
typedef char utime32_offset_check[
    offsetof(struct __utimbuf32, modtime) == 4 ? 1 : -1];
/** @brief Verifies the 64-bit modification-time offset. */
typedef char utime64_offset_check[
    offsetof(struct __utimbuf64, modtime) == 8 ? 1 : -1];
/** @brief Type of _utime32. */
typedef int (__cdecl *utime32_fn)(const char *, struct __utimbuf32 *);
/** @brief Type of _utime64. */
typedef int (__cdecl *utime64_fn)(const char *, struct __utimbuf64 *);
/** @brief Forces type checking of _utime32. */
static utime32_fn utime32_address = _utime32;
/** @brief Forces type checking of _utime64. */
static utime64_fn utime64_address = _utime64;
