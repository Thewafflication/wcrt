/** @file sys/stat.h @brief Declares Microsoft file-status interfaces. */

#ifndef WCRT_SYS_STAT_H
#define WCRT_SYS_STAT_H

#include <wcrt/ms_types.h>
#include <sys/types.h>

#if defined(WCRT_VC6_STAT32)
#if !defined(__i386__) && !defined(_M_IX86)
#error WCRT_VC6_STAT32 requires an x86 consumer
#endif
#define _stat wcrt_stat32
#define _fstat wcrt_fstat32
#endif

#if defined(WCRT_POSIX)
#include <time.h>
#endif

#define _S_IFDIR 0x4000 /**< Directory file type. */
#define _S_IFREG 0x8000 /**< Regular file type. */
#define _S_IREAD 0x0100 /**< Owner read permission. */
#define _S_IWRITE 0x0080 /**< Owner write permission. */
#define _S_IEXEC 0x0040 /**< Owner execute/search permission. */

#pragma pack(push, 8)
/** @brief Explicit WCRT file status with signed 32-bit size and times. */
struct wcrt_stat32 {
    unsigned int st_dev;   /**< Windows volume serial. */
    unsigned short st_ino; /**< Low inode bits. */
    unsigned short st_mode; /**< File type and permission bits. */
    short st_nlink;        /**< Narrow link count. */
    short st_uid;          /**< Zero user placeholder. */
    short st_gid;          /**< Zero group placeholder. */
    unsigned int st_rdev;  /**< Windows volume serial. */
    long st_size;          /**< Signed 32-bit file size. */
    long st_atime;         /**< Signed 32-bit access timestamp. */
    long st_mtime;         /**< Signed 32-bit modification timestamp. */
    long st_ctime;         /**< Signed 32-bit creation timestamp. */
};
/** @brief Microsoft file status with 64-bit size and timestamps. */
struct _stat64 {
    unsigned int st_dev;   /**< Drive identifier. */
    unsigned short st_ino; /**< File serial placeholder. */
    unsigned short st_mode; /**< File type and permission bits. */
    short st_nlink;        /**< Link count. */
    short st_uid;          /**< User identifier placeholder. */
    short st_gid;          /**< Group identifier placeholder. */
    unsigned int st_rdev;  /**< Special-device identifier. */
    wcrt_ms_int64 st_size;  /**< File size in bytes. */
    wcrt_ms_int64 st_atime; /**< Last-access Unix timestamp. */
    wcrt_ms_int64 st_mtime; /**< Last-modification Unix timestamp. */
    wcrt_ms_int64 st_ctime; /**< Creation Unix timestamp. */
};
#pragma pack(pop)

#if defined(WCRT_POSIX)
#define S_IFMT 0xF000U /**< File type mask. */
#define S_IFIFO 0x1000U /**< Named or anonymous pipe file type. */
#define S_IFCHR 0x2000U /**< Character-device file type. */
#define S_IFDIR 0x4000U /**< Directory file type. */
#define S_IFBLK 0x6000U /**< Block-device file type. */
#define S_IFREG 0x8000U /**< Regular file type. */
#define S_IFLNK 0xA000U /**< Symbolic-link file type. */
#define S_IFSOCK 0xC000U /**< Socket file type. */
#define S_IRUSR 0x0100U /**< Owner read permission. */
#define S_IWUSR 0x0080U /**< Owner write permission. */
#define S_IXUSR 0x0040U /**< Owner execute or search permission. */
#define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR) /**< All owner permissions. */

/** @brief Determines whether a mode describes a directory. */
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
/** @brief Determines whether a mode describes a regular file. */
#define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
/** @brief Determines whether a mode describes a pipe. */
#define S_ISFIFO(mode) (((mode) & S_IFMT) == S_IFIFO)
/** @brief Determines whether a mode describes a character device. */
#define S_ISCHR(mode) (((mode) & S_IFMT) == S_IFCHR)
/** @brief Determines whether a mode describes a block device. */
#define S_ISBLK(mode) (((mode) & S_IFMT) == S_IFBLK)
/** @brief Determines whether a mode describes a symbolic link. */
#define S_ISLNK(mode) (((mode) & S_IFMT) == S_IFLNK)
/** @brief Determines whether a mode describes a socket. */
#define S_ISSOCK(mode) (((mode) & S_IFMT) == S_IFSOCK)

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
/** @brief Obtains 64-bit status for an open descriptor. */
int __cdecl _fstat64(int descriptor, struct _stat64 *result);
/** @brief Obtains metadata through the explicit 32-bit WCRT ABI. */
int __cdecl wcrt_stat32(const char *path, struct wcrt_stat32 *result);
/** @brief Obtains descriptor metadata through the 32-bit WCRT ABI. */
int __cdecl wcrt_fstat32(int descriptor, struct wcrt_stat32 *result);
/** @brief Maps write permission to the Windows read-only attribute. */
int __cdecl _chmod(const char *path, int mode);
#if (defined(__i386__) || defined(_M_IX86)) && !defined(WCRT_VC6_STAT32)
/** @brief Provides TinyCC's linker-compatible x86 status alias. */
int __cdecl _stat(const char *path, struct _stat64 *result);
/** @brief Provides TinyCC's linker-compatible x86 descriptor-status alias. */
int __cdecl _fstat(int descriptor, struct _stat64 *result);
#endif
#if defined(WCRT_POSIX)
/**
 * @brief Obtains file metadata through the selected POSIX adapter.
 * @param path Null-terminated path to query.
 * @param result Receives fully initialized metadata on success.
 * @return Zero on success, or -1 with errno set on failure.
 */
int stat(const char *path, struct stat *result);
/** @brief Obtains file metadata for an open descriptor. */
int fstat(int descriptor, struct stat *result);
/** @brief Obtains metadata using the selected Windows link behavior. */
int lstat(const char *path, struct stat *result);
/** @brief Changes the selected Windows-mapped permission bits. */
int chmod(const char *path, mode_t mode);
/** @brief Creates one directory; Windows ignores permission mode bits. */
int mkdir(const char *path, mode_t mode);
#endif

#ifdef __cplusplus
}
#endif

#endif
