/** @file sys/stat.h @brief Declares Microsoft file-status interfaces. */

#ifndef WCRT_SYS_STAT_H
#define WCRT_SYS_STAT_H

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

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Obtains 64-bit status for a path. */
int __cdecl _stat64(const char *path, struct _stat64 *result);
#if defined(__i386__) || defined(_M_IX86)
/** @brief Provides TinyCC's linker-compatible x86 status alias. */
int __cdecl _stat(const char *path, struct _stat64 *result);
#endif

#ifdef __cplusplus
}
#endif

#endif
