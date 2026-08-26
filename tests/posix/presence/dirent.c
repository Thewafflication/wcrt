/** @file dirent.c @brief Checks selected POSIX directory declarations. */
#define WCRT_POSIX 1
#include <stddef.h>
#include <dirent.h>
#include <dirent.h>

#ifndef WCRT_DIRENT_H
#error <dirent.h> did not resolve to the WCRT header
#endif
#ifdef _POSIX_VERSION
#error A bounded WCRT profile must not advertise _POSIX_VERSION
#endif
#if DT_UNKNOWN != 0 || DT_DIR != 4 || DT_REG != 8
#error Selected directory entry types have unexpected values
#endif

/** @brief Verifies that the public name buffer includes its terminator. */
typedef char posix_dirent_name_size[
    sizeof(((struct dirent *)0)->d_name) == 256 ? 1 : -1];
/** @brief Verifies the inode field precedes the entry name. */
typedef char posix_dirent_member_order[
    offsetof(struct dirent, d_name) > offsetof(struct dirent, d_ino) ? 1 : -1];

/** @brief Selected directory-open signature. */
typedef DIR *(*posix_opendir_fn)(const char *);
/** @brief Selected directory-read signature. */
typedef struct dirent *(*posix_readdir_fn)(DIR *);
/** @brief Selected directory-rewind signature. */
typedef void (*posix_rewinddir_fn)(DIR *);
/** @brief Selected directory-close signature. */
typedef int (*posix_closedir_fn)(DIR *);
/** @brief Forces type checking of opendir. */
static posix_opendir_fn posix_opendir_address = opendir;
/** @brief Forces type checking of readdir. */
static posix_readdir_fn posix_readdir_address = readdir;
/** @brief Forces type checking of rewinddir. */
static posix_rewinddir_fn posix_rewinddir_address = rewinddir;
/** @brief Forces type checking of closedir. */
static posix_closedir_fn posix_closedir_address = closedir;
