/**
 * @file file.h
 * @brief Defines WCRT stream state and Windows file primitives.
 */

#ifndef WCRT_INTERNAL_FILE_H
#define WCRT_INTERNAL_FILE_H

#include <stdio.h>

#define WCRT_FILE_READ 1
#define WCRT_FILE_WRITE 2
#define WCRT_FILE_APPEND 4
#define WCRT_FILE_BINARY 8
#define WCRT_FILE_OWNED 16
#define WCRT_FILE_DELETE 32

struct wcrt_file {
    void *handle;             /**< Windows handle. */
    unsigned int flags;       /**< Access and ownership flags. */
    int end_of_file;          /**< End-of-file indicator. */
    int error;                /**< Error indicator. */
    int pushback;             /**< Pending pushed-back byte or EOF. */
    int buffering;            /**< Requested buffering mode. */
    char *buffer;             /**< Caller-provided buffer. */
    size_t buffer_size;       /**< Caller-provided buffer size. */
    char delete_path[260];    /**< Path removed at close. */
};

int __wcrt_file_open(FILE *stream, const char *path, const char *mode);
int __wcrt_file_close(FILE *stream);
int __wcrt_file_read(FILE *stream, void *buffer, size_t count,
    size_t *transferred);
int __wcrt_file_write(FILE *stream, const void *buffer, size_t count,
    size_t *transferred);
int __wcrt_file_seek(FILE *stream, long long offset, int origin,
    long long *position);
int __wcrt_file_remove(const char *path);
int __wcrt_file_rename(const char *old_path, const char *new_path);
int __wcrt_file_temporary(char *path);
void __wcrt_file_initialize_standard(FILE *stream, int selector,
    unsigned int flags);

#endif
