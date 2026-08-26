/** @file dirent.h @brief Declares selected POSIX directory interfaces. */

#ifndef WCRT_DIRENT_H
#define WCRT_DIRENT_H

#if defined(WCRT_POSIX)

#include <sys/types.h>

/** @brief Maximum storage reserved for one directory-entry name. */
#define WCRT_DIRENT_NAME_MAX 255

/** @brief Entry type is unavailable. */
#define DT_UNKNOWN 0
/** @brief Entry names a directory. */
#define DT_DIR 4
/** @brief Entry names a regular file. */
#define DT_REG 8

/** @brief One entry returned by readdir. */
struct dirent {
    ino_t d_ino; /**< File serial number, or zero when unavailable. */
    unsigned char d_type; /**< Selected DT_* entry type. */
    char d_name[WCRT_DIRENT_NAME_MAX + 1]; /**< Null-terminated name. */
};

/** @brief Opaque directory stream. */
typedef struct __wcrt_DIR DIR;

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Opens a directory stream for path. */
DIR *opendir(const char *path);
/** @brief Returns the next entry, or NULL at end of stream or on error. */
struct dirent *readdir(DIR *directory);
/** @brief Repositions a directory stream to its first entry. */
void rewinddir(DIR *directory);
/** @brief Closes a directory stream. */
int closedir(DIR *directory);

#ifdef __cplusplus
}
#endif

#endif

#endif
