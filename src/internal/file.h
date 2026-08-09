/**
 * @file file.h
 * @brief Defines WCRT stream state and Windows file primitives.
 */

#ifndef WCRT_INTERNAL_FILE_H
#define WCRT_INTERNAL_FILE_H

#include <stdio.h>
#include <wchar.h>

#define WCRT_FILE_READ 1
#define WCRT_FILE_WRITE 2
#define WCRT_FILE_APPEND 4
#define WCRT_FILE_BINARY 8
#define WCRT_FILE_OWNED 16
#define WCRT_FILE_DELETE 32

#define WCRT_ORIENTATION_NONE 0
#define WCRT_ORIENTATION_BYTE (-1)
#define WCRT_ORIENTATION_WIDE 1

struct wcrt_file {
    void *handle;             /**< Windows handle. */
    int descriptor;           /**< Microsoft-compatible file descriptor. */
    unsigned int flags;       /**< Access and ownership flags. */
    int end_of_file;          /**< End-of-file indicator. */
    int error;                /**< Error indicator. */
    int pushback;             /**< Pending pushed-back byte or EOF. */
    int orientation;          /**< Zero, byte-negative, or wide-positive. */
    mbstate_t wide_state;     /**< Per-stream C-locale conversion state. */
    wint_t wide_pushback;     /**< Pending pushed-back wide character. */
    int has_wide_pushback;    /**< Whether wide_pushback is occupied. */
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
    int descriptor, unsigned int flags);
void __wcrt_prepare_stream(FILE *stream);
int __wcrt_orient_stream(FILE *stream, int mode);
int __wcrt_require_orientation(FILE *stream, int orientation);
void __wcrt_reset_stream_conversion(FILE *stream);
int __wcrt_vfwscanf_c_locale(FILE *stream, const char *format,
    va_list arguments);

#endif
