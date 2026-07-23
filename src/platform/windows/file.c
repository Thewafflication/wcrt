/**
 * @file file.c
 * @brief Implements WCRT streams with Windows 2000 file APIs.
 */

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
#define WCRT_OPEN_EXISTING 3UL
#define WCRT_OPEN_ALWAYS 4UL
#define WCRT_NORMAL_ATTRIBUTE 0x80UL
#define WCRT_INVALID_HANDLE ((void *)(long long)-1)
#define WCRT_INVALID_POSITION 0xffffffffUL

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
    stream->buffering = _IOFBF;
    stream->buffer = NULL;
    stream->buffer_size = 0;
    stream->delete_path[0] = '\0';
    if (flags & WCRT_FILE_APPEND) {
        __wcrt_file_seek(stream, 0, SEEK_END, NULL);
    }
    return 0;
}

int __wcrt_file_close(FILE *stream)
{
    int result = 0;
    if (stream->handle != NULL && (stream->flags & WCRT_FILE_OWNED)) {
        result = CloseHandle(stream->handle) ? 0 : -1;
    }
    stream->handle = NULL;
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
    *transferred = amount;
    return result ? 0 : -1;
}

int __wcrt_file_write(FILE *stream, const void *buffer, size_t count,
    size_t *transferred)
{
    unsigned long amount = 0;
    unsigned long request = count > 0xffffffffUL ? 0xffffffffUL :
        (unsigned long)count;
    int result = WriteFile(stream->handle, buffer, request, &amount, NULL);
    *transferred = amount;
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

int __wcrt_file_remove(const char *path)
{
    return DeleteFileA(path) ? 0 : -1;
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
    unsigned int flags)
{
    if (stream->handle == NULL) {
        stream->handle = GetStdHandle((unsigned long)selector);
        stream->flags = flags;
        stream->pushback = EOF;
        stream->buffering = selector == -12 ? _IONBF : _IOLBF;
    }
}
