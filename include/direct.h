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

#ifdef __cplusplus
}
#endif

#endif
