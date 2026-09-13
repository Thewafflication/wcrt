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
/** @brief Returns the current narrow working directory. */
char *_getcwd(char *buffer, int size);

#ifdef __cplusplus
}
#endif

#endif
