/**
 * @file sys/types.h
 * @brief Declares Microsoft and selected POSIX system types.
 */

#ifndef WCRT_SYS_TYPES_H
#define WCRT_SYS_TYPES_H

/** @brief Microsoft unsigned device identifier. */
typedef unsigned int _dev_t;
/** @brief Microsoft unsigned file serial number. */
typedef unsigned short _ino_t;
/** @brief Microsoft signed 32-bit file offset. */
typedef long _off_t;

#if defined(WCRT_MS_TYPES_ALIASES) && !defined(WCRT_POSIX)
/** @brief Microsoft device identifier without the underscore spelling. */
typedef _dev_t dev_t;
/** @brief Microsoft file serial without the underscore spelling. */
typedef _ino_t ino_t;
/** @brief Microsoft file offset without the underscore spelling. */
typedef _off_t off_t;
#endif

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
/** @brief Signed byte count returned by descriptor I/O. */
typedef __PTRDIFF_TYPE__ ssize_t;
/** @brief Signed microsecond component used by struct timeval. */
typedef long suseconds_t;
/** @brief Signed process identifier covering every Windows process ID. */
typedef long long pid_t;
#endif

#endif
