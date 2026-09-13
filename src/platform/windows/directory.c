/**
 * @file directory.c
 * @brief Implements narrow Windows directory and path operations.
 */

#define WCRT_POSIX 1

#include <direct.h>
#include <errno.h>
#include <io.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_ERROR_FILE_NOT_FOUND 2UL
#define WCRT_ERROR_PATH_NOT_FOUND 3UL
#define WCRT_ERROR_ACCESS_DENIED 5UL
#define WCRT_ERROR_ALREADY_EXISTS 183UL
#define WCRT_ERROR_INSUFFICIENT_BUFFER 122UL

__declspec(dllimport) int WCRT_WINAPI CreateDirectoryA(const char *path,
    void *security);
__declspec(dllimport) unsigned long WCRT_WINAPI GetLastError(void);
__declspec(dllimport) unsigned long WCRT_WINAPI GetCurrentDirectoryA(
    unsigned long size, char *buffer);
__declspec(dllimport) unsigned long WCRT_WINAPI GetFullPathNameA(
    const char *path, unsigned long size, char *buffer, char **file_part);

/** @brief Maps path-query failures to the public error vocabulary. */
static void wcrt_path_error(unsigned long error)
{
    errno = error == WCRT_ERROR_INSUFFICIENT_BUFFER ? ERANGE : EACCES;
}

int _mkdir(const char *path)
{
    unsigned long error;

    if (path == 0) {
        errno = EINVAL;
        return -1;
    }
    if (CreateDirectoryA(path, 0)) return 0;
    error = GetLastError();
    if (error == WCRT_ERROR_ALREADY_EXISTS) {
        errno = EEXIST;
    } else if (error == WCRT_ERROR_FILE_NOT_FOUND ||
        error == WCRT_ERROR_PATH_NOT_FOUND) {
        errno = ENOENT;
    } else if (error == WCRT_ERROR_ACCESS_DENIED) {
        errno = EACCES;
    } else {
        errno = EACCES;
    }
    return -1;
}

char *_getcwd(char *buffer, int size)
{
    char *result = buffer;
    unsigned long length;
    int allocated = 0;
    if (size <= 0) size = buffer == NULL ? MAX_PATH : 0;
    if (size <= 0) {
        errno = EINVAL;
        return NULL;
    }
    if (result == NULL) {
        result = (char *)malloc((size_t)size);
        if (result == NULL) {
            errno = ENOMEM;
            return NULL;
        }
        allocated = 1;
    }
    length = GetCurrentDirectoryA((unsigned long)size, result);
    if (length == 0 || length >= (unsigned long)size) {
        wcrt_path_error(length >= (unsigned long)size ?
            WCRT_ERROR_INSUFFICIENT_BUFFER : GetLastError());
        if (allocated) free(result);
        return NULL;
    }
    return result;
}

char *getcwd(char *buffer, size_t size)
{
    if (size > 2147483647U) size = 2147483647U;
    return _getcwd(buffer, (int)size);
}

char *_fullpath(char *absolute, const char *relative, size_t size)
{
    char *result = absolute;
    unsigned long length;
    int allocated = 0;
    if (relative == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (size == 0) size = absolute == NULL ? MAX_PATH : 0;
    if (size == 0 || size > 0xffffffffUL) {
        errno = EINVAL;
        return NULL;
    }
    if (result == NULL) {
        result = (char *)malloc(size);
        if (result == NULL) {
            errno = ENOMEM;
            return NULL;
        }
        allocated = 1;
    }
    length = GetFullPathNameA(relative, (unsigned long)size, result, NULL);
    if (length == 0 || length >= (unsigned long)size) {
        wcrt_path_error(length >= (unsigned long)size ?
            WCRT_ERROR_INSUFFICIENT_BUFFER : GetLastError());
        if (allocated) free(result);
        return NULL;
    }
    return result;
}

char *realpath(const char *path, char *resolved)
{
    if (path == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (_access(path, 0) != 0) return NULL;
    return _fullpath(resolved, path, PATH_MAX);
}
