/**
 * @file direct.h
 * @brief Provides Microsoft directory compatibility declarations.
 */

#ifndef WCRT_DIRECT_H
#define WCRT_DIRECT_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Creates one directory. */
int _mkdir(const char *path);
/** @brief Changes the process working directory. */
int _chdir(const char *path);
/** @brief Removes an empty directory. */
int _rmdir(const char *path);
/** @brief Returns the current narrow working directory. */
char *_getcwd(char *buffer, int size);
/** @brief Returns the current ASCII-letter drive number, or zero. */
int _getdrive(void);
/** @brief Returns the available logical-drive bitmask. */
unsigned long _getdrives(void);
/** @brief Selects a letter drive using Windows drive-relative state. */
int _chdrive(int drive);
/** @brief Queries the selected drive's current directory. */
char *_getdcwd(int drive, char *buffer, int size);

#ifdef __cplusplus
}
#endif

#endif
