/**
 * @file io.h
 * @brief Provides the Microsoft low-level I/O compatibility header.
 */

#if defined(WCRT_VC6_FIND32) && (defined(__x86_64__) || \
    defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64))
#error WCRT_VC6_FIND32 requires x86
#endif

#ifndef WCRT_IO_H
#define WCRT_IO_H

#include <stdio.h>
#include <wcrt/ms_types.h>

#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || \
    defined(_M_ARM64)
typedef long long __wcrt_intptr_t;
#else
typedef long __wcrt_intptr_t;
#endif

/** @brief Unsigned 32-bit Microsoft filename-search size type. */
typedef unsigned long _fsize_t;
/** @brief Contains checked narrow filename-search metadata. */
#pragma pack(push, 8)
struct wcrt_finddata32 {
    unsigned int attrib;
    long time_create;
    long time_access;
    long time_write;
    _fsize_t size;
    char name[260];
};
/** @brief Contains narrow timestamps and a checked signed wide file size. */
struct wcrt_finddata32i64 {
    unsigned int attrib;
    long time_create;
    long time_access;
    long time_write;
    wcrt_ms_int64 size;
    char name[260];
};
#pragma pack(pop)
#define _A_NORMAL 0
#define _A_RDONLY 1
#define _A_HIDDEN 2
#define _A_SYSTEM 4
#define _A_SUBDIR 16
#define _A_ARCH 32

#ifdef WCRT_VC6_FIND32
#define _finddata_t wcrt_finddata32
#define _findfirst wcrt_findfirst32
#define _findnext wcrt_findnext32
#define _findclose wcrt_findclose
#define _finddatai64_t wcrt_finddata32i64
#define _findfirsti64 wcrt_findfirst32i64
#define _findnexti64 wcrt_findnext32i64
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Starts an ANSI filename search with checked narrow metadata. */
__wcrt_intptr_t __cdecl wcrt_findfirst32(const char *specification,
    struct wcrt_finddata32 *result);
/** @brief Advances a search without closing it on exhaustion or failure. */
int __cdecl wcrt_findnext32(__wcrt_intptr_t handle,
    struct wcrt_finddata32 *result);
/** @brief Starts a narrow-time search with a signed 64-bit file size. */
__wcrt_intptr_t __cdecl wcrt_findfirst32i64(const char *specification,
    struct wcrt_finddata32i64 *result);
/** @brief Advances a wide-size search, retaining output on any failure. */
int __cdecl wcrt_findnext32i64(__wcrt_intptr_t handle,
    struct wcrt_finddata32i64 *result);
/** @brief Releases a successful filename search handle. */
int __cdecl wcrt_findclose(__wcrt_intptr_t handle);

/** @brief Removes one filesystem name. */
int _unlink(const char *path);
/** @brief Tests Microsoft path accessibility. */
int _access(const char *path, int mode);
/** @brief Returns the native Windows handle behind a WCRT descriptor. */
__wcrt_intptr_t _get_osfhandle(int descriptor);
/** @brief Transfers an exclusively owned native handle into a descriptor. */
int _open_osfhandle(__wcrt_intptr_t handle, int flags);
int _close(int descriptor);
/** @brief Opens a descriptor with explicit sharing and creation permissions. */
int _sopen(const char *path, int flags, int sharing, ...);
/** @brief Creates or truncates a shared read/write text descriptor. */
int _creat(const char *path, int permissions);
/** @brief Replaces the process creation mask and returns its previous value. */
int _umask(int permissions);
int _read(int descriptor, void *buffer, unsigned int count);
int _write(int descriptor, const void *buffer, unsigned int count);
long _lseek(int descriptor, long offset, int origin);
long _tell(int descriptor);
/** @brief Repositions a seekable descriptor with a signed 64-bit offset. */
wcrt_ms_int64 _lseeki64(int descriptor, wcrt_ms_int64 offset, int origin);
/** @brief Queries physical position without clearing stream state. */
wcrt_ms_int64 _telli64(int descriptor);
/** @brief Queries regular-file physical size with checked 32-bit narrowing. */
long _filelength(int descriptor);
/** @brief Queries regular-file physical size as a signed 64-bit integer. */
wcrt_ms_int64 _filelengthi64(int descriptor);
/** @brief Tests whether physical position exactly equals regular-file size. */
int _eof(int descriptor);
/** @brief Resizes a writable regular file, preserving physical position. */
int _chsize(int descriptor, long size);
/** @brief Locks or unlocks a regular-file range at the physical position. */
int _locking(int descriptor, int mode, long count);
int _commit(int descriptor);
int _isatty(int descriptor);
/** @brief Selects text or binary translation and returns the previous mode. */
int _setmode(int descriptor, int mode);
FILE *_fdopen(int descriptor, const char *mode);
int _dup(int descriptor);
int _dup2(int descriptor, int target);
int _pipe(int descriptors[2], unsigned int size, int text_mode);

#ifdef __cplusplus
}
#endif

#endif
