/** @file sys/stat.h @brief Declares Microsoft file-status interfaces. */

#ifndef WCRT_SYS_STAT_H
#define WCRT_SYS_STAT_H

#if defined(WCRT_POSIX)
#include <sys/types.h>
#include <time.h>
#endif

#define _S_IFDIR 0x4000 /**< Directory file type. */
#define _S_IFREG 0x8000 /**< Regular file type. */
#define _S_IREAD 0x0100 /**< Owner read permission. */
#define _S_IWRITE 0x0080 /**< Owner write permission. */
#define _S_IEXEC 0x0040 /**< Owner execute/search permission. */

#pragma pack(push, 8)
/** @brief Microsoft file status with 64-bit size and timestamps. */
struct _stat64 {
    unsigned int st_dev;   /**< Drive identifier. */
    unsigned short st_ino; /**< File serial placeholder. */
    unsigned short st_mode; /**< File type and permission bits. */
    short st_nlink;        /**< Link count. */
    short st_uid;          /**< User identifier placeholder. */
    short st_gid;          /**< Group identifier placeholder. */
    unsigned int st_rdev;  /**< Special-device identifier. */
    long long st_size;     /**< File size in bytes. */
    long long st_atime;    /**< Last-access Unix timestamp. */
    long long st_mtime;    /**< Last-modification Unix timestamp. */
    long long st_ctime;    /**< Creation Unix timestamp. */
};
#pragma pack(pop)

#if defined(WCRT_POSIX)
#define S_IFMT 0xF000U /**< File type mask. */
#define S_IFDIR 0x4000U /**< Directory file type. */
#define S_IFREG 0x8000U /**< Regular file type. */
#define S_IRUSR 0x0100U /**< Owner read permission. */
#define S_IWUSR 0x0080U /**< Owner write permission. */
#define S_IXUSR 0x0040U /**< Owner execute or search permission. */
#define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR) /**< All owner permissions. */

/** @brief Determines whether a mode describes a directory. */
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
/** @brief Determines whether a mode describes a regular file. */
#define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)

/** @brief POSIX-facing file metadata with documented Windows mappings. */
struct stat {
    dev_t st_dev;          /**< Containing device identifier. */
    ino_t st_ino;          /**< File serial number, or zero when unavailable. */
    mode_t st_mode;        /**< File type and supported permission bits. */
    nlink_t st_nlink;      /**< Reported hard-link count. */
    uid_t st_uid;          /**< Owner identifier, or zero when unavailable. */
    gid_t st_gid;          /**< Group identifier, or zero when unavailable. */
    dev_t st_rdev;         /**< Special-device identifier. */
    off_t st_size;         /**< File size in bytes. */
    struct timespec st_atim; /**< Last-access timestamp. */
    struct timespec st_mtim; /**< Last-modification timestamp. */
    struct timespec st_ctim; /**< Windows creation timestamp. */
    time_t st_atime;       /**< Direct last-access seconds extension. */
    time_t st_mtime;       /**< Direct modification seconds extension. */
    time_t st_ctime;       /**< Direct Windows creation seconds extension. */
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Obtains 64-bit status for a path. */
int __cdecl _stat64(const char *path, struct _stat64 *result);
#if defined(__i386__) || defined(_M_IX86)
/** @brief Provides TinyCC's linker-compatible x86 status alias. */
int __cdecl _stat(const char *path, struct _stat64 *result);
#endif
#if defined(WCRT_POSIX)
/**
 * @brief Obtains file metadata through the selected POSIX adapter.
 * @param path Null-terminated path to query.
 * @param result Receives fully initialized metadata on success.
 * @return Zero on success, or -1 with errno set on failure.
 */
int stat(const char *path, struct stat *result);
#endif

#ifdef __cplusplus
}
#endif

#endif
