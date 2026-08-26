/**
 * @file sys/types.h
 * @brief Declares types for the selected POSIX compatibility profile.
 */

#ifndef WCRT_SYS_TYPES_H
#define WCRT_SYS_TYPES_H

#if defined(WCRT_POSIX)
/** @brief Windows device identifier used by POSIX adapters. */
typedef unsigned int dev_t;
/** @brief Source-compatible POSIX file serial number. */
typedef unsigned long long ino_t;
/** @brief File type and permission bit set. */
typedef unsigned int mode_t;
/** @brief File link count. */
typedef unsigned long nlink_t;
/** @brief Source-compatible POSIX user identifier. */
typedef unsigned int uid_t;
/** @brief Source-compatible POSIX group identifier. */
typedef unsigned int gid_t;
/** @brief Signed 64-bit file offset and size. */
typedef long long off_t;
#endif

#endif
