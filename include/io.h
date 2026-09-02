/**
 * @file io.h
 * @brief Provides the Microsoft low-level I/O compatibility header.
 */

#ifndef WCRT_IO_H
#define WCRT_IO_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Removes one filesystem name. */
int _unlink(const char *path);

#ifdef __cplusplus
}
#endif

#endif
