/**
 * @file sys/utime.h
 * @brief Declares Microsoft explicit-width file-time update interfaces.
 */

#ifndef WCRT_SYS_UTIME_H
#define WCRT_SYS_UTIME_H

/** @brief Pair of signed 32-bit Unix timestamps. */
struct __utimbuf32 {
    long actime;  /**< Last-access time. */
    long modtime; /**< Last-modification time. */
};

/** @brief Pair of signed 64-bit Unix timestamps. */
struct __utimbuf64 {
    long long actime;  /**< Last-access time. */
    long long modtime; /**< Last-modification time. */
};

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Sets file times from signed 32-bit Unix timestamps. */
int __cdecl _utime32(const char *path, struct __utimbuf32 *times);
/** @brief Sets file times from signed 64-bit Unix timestamps. */
int __cdecl _utime64(const char *path, struct __utimbuf64 *times);

#ifdef __cplusplus
}
#endif

#endif
