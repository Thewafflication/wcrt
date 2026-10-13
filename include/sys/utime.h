/**
 * @file sys/utime.h
 * @brief Declares Microsoft explicit-width file-time update interfaces.
 */

#ifndef WCRT_SYS_UTIME_H
#define WCRT_SYS_UTIME_H

#include <wcrt/ms_types.h>

#if defined(WCRT_VC6_UTIME32)
#if !defined(__i386__) && !defined(_M_IX86)
#error WCRT_VC6_UTIME32 requires an x86 consumer
#endif
#define _utime _utime32
#define _futime _futime32
#define _utimbuf __utimbuf32
#endif

/** @brief Pair of signed 32-bit Unix timestamps. */
struct __utimbuf32 {
    long actime;  /**< Last-access time. */
    long modtime; /**< Last-modification time. */
};

/** @brief Pair of signed 64-bit Unix timestamps. */
struct __utimbuf64 {
    wcrt_ms_int64 actime;  /**< Last-access time. */
    wcrt_ms_int64 modtime; /**< Last-modification time. */
};

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Sets file times from signed 32-bit Unix timestamps. */
int __cdecl _utime32(const char *path, struct __utimbuf32 *times);
/** @brief Sets file times from signed 64-bit Unix timestamps. */
int __cdecl _utime64(const char *path, struct __utimbuf64 *times);
/** @brief Updates a borrowed regular-file descriptor with 32-bit times. */
int __cdecl _futime32(int descriptor, struct __utimbuf32 *times);
/** @brief Updates a borrowed regular-file descriptor with 64-bit times. */
int __cdecl _futime64(int descriptor, struct __utimbuf64 *times);

#ifdef __cplusplus
}
#endif

#endif
