/**
 * @file file.c
 * @brief Implements WCRT streams with Windows 2000 file APIs.
 */

#define WCRT_POSIX 1

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "../../internal/file.h"

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_GENERIC_READ 0x80000000UL
#define WCRT_GENERIC_WRITE 0x40000000UL
#define WCRT_SHARE_READ 1UL
#define WCRT_SHARE_WRITE 2UL
#define WCRT_CREATE_ALWAYS 2UL
#define WCRT_CREATE_NEW 1UL
#define WCRT_OPEN_EXISTING 3UL
#define WCRT_OPEN_ALWAYS 4UL
#define WCRT_TRUNCATE_EXISTING 5UL
#define WCRT_NORMAL_ATTRIBUTE 0x80UL
#define WCRT_INVALID_HANDLE ((void *)(long long)-1)
#define WCRT_INVALID_POSITION 0xffffffffUL
#define WCRT_ERROR_FILE_NOT_FOUND 2UL
#define WCRT_ERROR_PATH_NOT_FOUND 3UL
#define WCRT_ERROR_ACCESS_DENIED 5UL
#define WCRT_ERROR_INVALID_HANDLE 6UL
#define WCRT_ERROR_NOT_ENOUGH_MEMORY 8UL
#define WCRT_ERROR_SHARING_VIOLATION 32UL
#define WCRT_ERROR_FILE_EXISTS 80UL
#define WCRT_ERROR_BROKEN_PIPE 109UL
#define WCRT_ERROR_DISK_FULL 112UL
#define WCRT_ERROR_ALREADY_EXISTS 183UL
#define WCRT_DUPLICATE_SAME_ACCESS 2UL

__declspec(dllimport) void *WCRT_WINAPI CreateFileA(const char *path,
    unsigned long access, unsigned long sharing, void *security,
    unsigned long creation, unsigned long attributes, void *template_file);
__declspec(dllimport) int WCRT_WINAPI CloseHandle(void *handle);
__declspec(dllimport) int WCRT_WINAPI ReadFile(void *handle, void *buffer,
    unsigned long count, unsigned long *read_count, void *overlapped);
__declspec(dllimport) int WCRT_WINAPI WriteFile(void *handle,
    const void *buffer, unsigned long count, unsigned long *write_count,
    void *overlapped);
__declspec(dllimport) unsigned long WCRT_WINAPI SetFilePointer(void *handle,
    long low, long *high, unsigned long origin);
__declspec(dllimport) int WCRT_WINAPI DeleteFileA(const char *path);
__declspec(dllimport) int WCRT_WINAPI MoveFileA(const char *old_path,
    const char *new_path);
__declspec(dllimport) unsigned long WCRT_WINAPI GetTempPathA(
    unsigned long capacity, char *path);
__declspec(dllimport) unsigned int WCRT_WINAPI GetTempFileNameA(
    const char *path, const char *prefix, unsigned int unique, char *name);
__declspec(dllimport) void *WCRT_WINAPI GetStdHandle(unsigned long selector);
__declspec(dllimport) unsigned long WCRT_WINAPI GetLastError(void);
__declspec(dllimport) int WCRT_WINAPI FlushFileBuffers(void *handle);
__declspec(dllimport) int WCRT_WINAPI GetConsoleMode(void *handle,
    unsigned long *mode);
__declspec(dllimport) void *WCRT_WINAPI GetCurrentProcess(void);
__declspec(dllimport) int WCRT_WINAPI DuplicateHandle(void *source_process,
    void *source, void *target_process, void **target,
    unsigned long access, int inherit, unsigned long options);

/** @brief Maps file-operation failures to the WCRT error vocabulary. */
static void wcrt_file_error(unsigned long error)
{
    if (error == WCRT_ERROR_FILE_NOT_FOUND ||
        error == WCRT_ERROR_PATH_NOT_FOUND) errno = ENOENT;
    else if (error == WCRT_ERROR_ACCESS_DENIED ||
        error == WCRT_ERROR_SHARING_VIOLATION) errno = EACCES;
    else if (error == WCRT_ERROR_FILE_EXISTS ||
        error == WCRT_ERROR_ALREADY_EXISTS) errno = EEXIST;
    else if (error == WCRT_ERROR_INVALID_HANDLE) errno = EBADF;
    else if (error == WCRT_ERROR_NOT_ENOUGH_MEMORY) errno = ENOMEM;
    else if (error == WCRT_ERROR_BROKEN_PIPE) errno = EPIPE;
    else if (error == WCRT_ERROR_DISK_FULL) errno = ENOSPC;
    else errno = EIO;
}

int __wcrt_file_open(FILE *stream, const char *path, const char *mode)
{
    unsigned long access = 0;
    unsigned long creation;
    unsigned int flags = WCRT_FILE_OWNED;
    char first;

    if (stream == NULL || path == NULL || mode == NULL || *mode == '\0') {
        return -1;
    }
    first = *mode;
    if (first == 'r') {
        access = WCRT_GENERIC_READ;
        creation = WCRT_OPEN_EXISTING;
        flags |= WCRT_FILE_READ;
    } else if (first == 'w') {
        access = WCRT_GENERIC_WRITE;
        creation = WCRT_CREATE_ALWAYS;
        flags |= WCRT_FILE_WRITE;
    } else if (first == 'a') {
        access = WCRT_GENERIC_WRITE;
        creation = WCRT_OPEN_ALWAYS;
        flags |= WCRT_FILE_WRITE | WCRT_FILE_APPEND;
    } else {
        return -1;
    }
    if (strchr(mode, '+') != NULL) {
        access = WCRT_GENERIC_READ | WCRT_GENERIC_WRITE;
        flags |= WCRT_FILE_READ | WCRT_FILE_WRITE;
    }
    if (strchr(mode, 'b') != NULL) {
        flags |= WCRT_FILE_BINARY;
    }
    stream->handle = CreateFileA(path, access,
        WCRT_SHARE_READ | WCRT_SHARE_WRITE, NULL, creation,
        WCRT_NORMAL_ATTRIBUTE, NULL);
    if (stream->handle == WCRT_INVALID_HANDLE) {
        stream->handle = NULL;
        return -1;
    }
    stream->flags = flags;
    stream->end_of_file = 0;
    stream->error = 0;
    stream->pushback = EOF;
    stream->orientation = WCRT_ORIENTATION_NONE;
    stream->wide_state.__value = 0;
    stream->wide_state.__bytes = 0;
    stream->wide_state.__state = 0;
    stream->wide_pushback = WEOF;
    stream->has_wide_pushback = 0;
    stream->buffering = _IOFBF;
    stream->buffer = NULL;
    stream->buffer_size = 0;
    stream->delete_path[0] = '\0';
    if (flags & WCRT_FILE_APPEND) {
        __wcrt_file_seek(stream, 0, SEEK_END, NULL);
    }
    return 0;
}

int __wcrt_file_open_flags(FILE *stream, const char *path, int open_flags)
{
    unsigned long access;
    unsigned long creation = WCRT_OPEN_EXISTING;
    unsigned int flags = WCRT_FILE_OWNED;
    int access_mode = open_flags & 3;
    int supported = _O_WRONLY | _O_RDWR | _O_APPEND | _O_CREAT | _O_TRUNC |
        _O_EXCL | _O_TEXT | _O_BINARY;
    if (stream == NULL || path == NULL || access_mode == 3 ||
        (open_flags & ~supported) != 0 ||
        ((open_flags & _O_TEXT) && (open_flags & _O_BINARY))) {
        errno = EINVAL;
        return -1;
    }
    if (access_mode == _O_RDONLY) {
        access = WCRT_GENERIC_READ;
        flags |= WCRT_FILE_READ;
    } else if (access_mode == _O_WRONLY) {
        access = WCRT_GENERIC_WRITE;
        flags |= WCRT_FILE_WRITE;
    } else {
        access = WCRT_GENERIC_READ | WCRT_GENERIC_WRITE;
        flags |= WCRT_FILE_READ | WCRT_FILE_WRITE;
    }
    if (open_flags & _O_CREAT) {
        creation = (open_flags & _O_EXCL) ? WCRT_CREATE_NEW :
            ((open_flags & _O_TRUNC) ? WCRT_CREATE_ALWAYS : WCRT_OPEN_ALWAYS);
    } else if (open_flags & _O_TRUNC) {
        creation = WCRT_TRUNCATE_EXISTING;
    }
    if (open_flags & _O_APPEND) flags |= WCRT_FILE_APPEND;
    if (open_flags & _O_BINARY) flags |= WCRT_FILE_BINARY;
    stream->handle = CreateFileA(path, access,
        WCRT_SHARE_READ | WCRT_SHARE_WRITE, NULL, creation,
        WCRT_NORMAL_ATTRIBUTE, NULL);
    if (stream->handle == WCRT_INVALID_HANDLE) {
        unsigned long error = GetLastError();
        stream->handle = NULL;
        wcrt_file_error(error);
        return -1;
    }
    stream->flags = flags;
    stream->end_of_file = 0;
    stream->error = 0;
    stream->pushback = EOF;
    stream->orientation = WCRT_ORIENTATION_NONE;
    __wcrt_reset_stream_conversion(stream);
    stream->buffering = _IOFBF;
    stream->buffer = NULL;
    stream->buffer_size = 0;
    stream->delete_path[0] = '\0';
    if (flags & WCRT_FILE_APPEND) __wcrt_file_seek(stream, 0, SEEK_END, NULL);
    return 0;
}

int __wcrt_file_close(FILE *stream)
{
    int result = 0;
    if (stream->handle != NULL && (stream->flags & WCRT_FILE_OWNED)) {
        result = CloseHandle(stream->handle) ? 0 : -1;
    }
    stream->handle = NULL;
    stream->descriptor = -1;
    stream->orientation = WCRT_ORIENTATION_NONE;
    stream->pushback = EOF;
    stream->wide_pushback = WEOF;
    stream->has_wide_pushback = 0;
    if (stream->delete_path[0] != '\0') {
        DeleteFileA(stream->delete_path);
    }
    return result;
}

int __wcrt_file_read(FILE *stream, void *buffer, size_t count,
    size_t *transferred)
{
    unsigned long amount = 0;
    unsigned long request = count > 0xffffffffUL ? 0xffffffffUL :
        (unsigned long)count;
    int result = ReadFile(stream->handle, buffer, request, &amount, NULL);
    unsigned long error = result ? 0 : GetLastError();
    *transferred = amount;
    if (!result && error == WCRT_ERROR_BROKEN_PIPE) return 0;
    if (!result) wcrt_file_error(error);
    return result ? 0 : -1;
}

int __wcrt_file_write(FILE *stream, const void *buffer, size_t count,
    size_t *transferred)
{
    unsigned long amount = 0;
    unsigned long request = count > 0xffffffffUL ? 0xffffffffUL :
        (unsigned long)count;
    int result;
    if (stream->flags & WCRT_FILE_APPEND)
        __wcrt_file_seek(stream, 0, SEEK_END, NULL);
    result = WriteFile(stream->handle, buffer, request, &amount, NULL);
    *transferred = amount;
    if (!result) wcrt_file_error(GetLastError());
    return result ? 0 : -1;
}

int __wcrt_file_seek(FILE *stream, long long offset, int origin,
    long long *position)
{
    long high = (long)(offset >> 32);
    unsigned long low = SetFilePointer(stream->handle, (long)offset, &high,
        (unsigned long)origin);
    if (low == WCRT_INVALID_POSITION && high == -1) {
        return -1;
    }
    if (position != NULL) {
        *position = ((long long)high << 32) | low;
    }
    return 0;
}

int __wcrt_file_flush(FILE *stream)
{
    if (FlushFileBuffers(stream->handle)) return 0;
    wcrt_file_error(GetLastError());
    return -1;
}

int __wcrt_file_is_terminal(FILE *stream)
{
    unsigned long mode;
    return GetConsoleMode(stream->handle, &mode) ? 1 : 0;
}

int __wcrt_file_duplicate(FILE *source, FILE *target)
{
    void *process = GetCurrentProcess();
    void *handle;
    unsigned int flags;
    if (!DuplicateHandle(process, source->handle, process, &handle, 0, 0,
        WCRT_DUPLICATE_SAME_ACCESS)) {
        wcrt_file_error(GetLastError());
        return -1;
    }
    flags = (source->flags & (WCRT_FILE_READ | WCRT_FILE_WRITE |
        WCRT_FILE_APPEND | WCRT_FILE_BINARY)) | WCRT_FILE_OWNED;
    __wcrt_file_close(target);
    memset(target, 0, sizeof(*target));
    target->handle = handle;
    target->flags = flags;
    target->pushback = EOF;
    target->orientation = WCRT_ORIENTATION_NONE;
    target->wide_pushback = WEOF;
    target->buffering = _IOFBF;
    return 0;
}

int __wcrt_file_remove(const char *path)
{
    unsigned long error;
    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (DeleteFileA(path)) return 0;
    error = GetLastError();
    if (error == WCRT_ERROR_FILE_NOT_FOUND ||
        error == WCRT_ERROR_PATH_NOT_FOUND) {
        errno = ENOENT;
    } else if (error == WCRT_ERROR_ACCESS_DENIED ||
        error == WCRT_ERROR_SHARING_VIOLATION) {
        errno = EACCES;
    } else {
        errno = EACCES;
    }
    return -1;
}

int _unlink(const char *path)
{
    return __wcrt_file_remove(path);
}

int __wcrt_file_rename(const char *old_path, const char *new_path)
{
    return MoveFileA(old_path, new_path) ? 0 : -1;
}

int __wcrt_file_temporary(char *path)
{
    char directory[260];
    unsigned long length = GetTempPathA(sizeof(directory), directory);
    if (length == 0 || length >= sizeof(directory)) {
        return -1;
    }
    return GetTempFileNameA(directory, "wcr", 0, path) == 0 ? -1 : 0;
}

void __wcrt_file_initialize_standard(FILE *stream, int selector,
    int descriptor, unsigned int flags)
{
    if (stream->handle == NULL && stream->descriptor != -1) {
        stream->handle = GetStdHandle((unsigned long)selector);
        stream->descriptor = descriptor;
        stream->flags = flags;
        stream->pushback = EOF;
        stream->orientation = WCRT_ORIENTATION_NONE;
        stream->wide_state.__value = 0;
        stream->wide_state.__bytes = 0;
        stream->wide_state.__state = 0;
        stream->wide_pushback = WEOF;
        stream->has_wide_pushback = 0;
        stream->end_of_file = 0;
        stream->error = 0;
        stream->buffering = selector == -12 ? _IONBF : _IOLBF;
    }
}
