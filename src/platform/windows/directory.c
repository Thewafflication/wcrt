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
#include <string.h>
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
#define WCRT_ERROR_DIR_NOT_EMPTY 145UL
#define WCRT_ERROR_DIRECTORY 267UL
#define WCRT_ERROR_SHARING_VIOLATION 32UL
#define WCRT_INVALID_FILE_ATTRIBUTES 0xffffffffUL
#define WCRT_FILE_ATTRIBUTE_DIRECTORY 0x10UL

__declspec(dllimport) int WCRT_WINAPI CreateDirectoryA(const char *path,
    void *security);
__declspec(dllimport) int WCRT_WINAPI SetCurrentDirectoryA(const char *path);
__declspec(dllimport) int WCRT_WINAPI RemoveDirectoryA(const char *path);
__declspec(dllimport) unsigned long WCRT_WINAPI GetFileAttributesA(
    const char *path);
__declspec(dllimport) unsigned long WCRT_WINAPI GetLastError(void);
__declspec(dllimport) unsigned long WCRT_WINAPI GetCurrentDirectoryA(
    unsigned long size, char *buffer);
__declspec(dllimport) unsigned long WCRT_WINAPI GetLogicalDrives(void);
__declspec(dllimport) unsigned long WCRT_WINAPI GetFullPathNameA(
    const char *path, unsigned long size, char *buffer, char **file_part);

/** @brief Maps path-query failures to the public error vocabulary. */
static void wcrt_path_error(unsigned long error)
{
    errno = error == WCRT_ERROR_INSUFFICIENT_BUFFER ? ERANGE : EACCES;
}

int _getdrive(void)
{
    char current[MAX_PATH];
    unsigned long length = GetCurrentDirectoryA(MAX_PATH, current);
    unsigned char letter;
    if (length == 0 || length >= MAX_PATH) {
        wcrt_path_error(length >= MAX_PATH ?
            WCRT_ERROR_INSUFFICIENT_BUFFER : GetLastError());
        return 0;
    }
    if (length < 2 || current[1] != ':') return 0;
    letter = (unsigned char)current[0];
    if (letter >= 'a' && letter <= 'z') letter -= 'a' - 'A';
    return letter >= 'A' && letter <= 'Z' ? letter - 'A' + 1 : 0;
}

unsigned long _getdrives(void)
{
    return GetLogicalDrives();
}

int _chdrive(int drive)
{
    char path[3];
    if (drive < 1 || drive > 26) {
        errno = EACCES;
        return -1;
    }
    path[0] = (char)('A' + drive - 1);
    path[1] = ':';
    path[2] = '\0';
    if (SetCurrentDirectoryA(path)) return 0;
    errno = EACCES;
    return -1;
}

char *_getdcwd(int drive, char *buffer, int size)
{
    char relative[4];
    if (size <= 0) {
        errno = EINVAL;
        return NULL;
    }
    if (drive < 0 || drive > 26 || (drive != 0 &&
        !(GetLogicalDrives() & (1UL << (drive - 1))))) {
        errno = EACCES;
        return NULL;
    }
    if (drive == 0 || drive == _getdrive()) return _getcwd(buffer, size);
    relative[0] = (char)('A' + drive - 1);
    relative[1] = ':';
    relative[2] = '.';
    relative[3] = '\0';
    return _fullpath(buffer, relative, (size_t)size);
}

int _chdir(const char *path)
{
    unsigned long error;
    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (SetCurrentDirectoryA(path)) return 0;
    error = GetLastError();
    errno = error == WCRT_ERROR_ACCESS_DENIED ||
        error == WCRT_ERROR_SHARING_VIOLATION ? EACCES : ENOENT;
    return -1;
}

/** @brief Recognizes canonical drive and UNC share root paths. */
static int wcrt_directory_root(const char *path)
{
    const char *cursor;
    if (path[0] != '\0' && path[1] == ':' && path[2] == '\\' &&
        path[3] == '\0') return 1;
    if (path[0] != '\\' || path[1] != '\\') return 0;
    cursor = path + 2;
    while (*cursor != '\0' && *cursor != '\\') ++cursor;
    if (*cursor == '\0' || cursor == path + 2) return 0;
    ++cursor;
    if (*cursor == '\0' || *cursor == '\\') return 0;
    while (*cursor != '\0' && *cursor != '\\') ++cursor;
    while (*cursor == '\\') ++cursor;
    return *cursor == '\0';
}

/** @brief Protects current/root directories within the legacy path model. */
static int wcrt_directory_protected(const char *path)
{
    char absolute[MAX_PATH];
    char current[MAX_PATH];
    unsigned long length;
    size_t absolute_size;
    size_t current_size;

    length = GetFullPathNameA(path, MAX_PATH, absolute, NULL);
    if (length == 0 || length >= MAX_PATH) return 0;
    if (wcrt_directory_root(absolute)) {
        unsigned long attributes = GetFileAttributesA(absolute);
        return attributes != WCRT_INVALID_FILE_ATTRIBUTES &&
            (attributes & WCRT_FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    length = GetCurrentDirectoryA(MAX_PATH, current);
    if (length == 0 || length >= MAX_PATH) return 0;
    absolute_size = strlen(absolute);
    current_size = strlen(current);
    while (absolute_size > 0 && absolute[absolute_size - 1] == '\\')
        --absolute_size;
    while (current_size > 0 && current[current_size - 1] == '\\')
        --current_size;
    return absolute_size == current_size &&
        _strnicmp(absolute, current, absolute_size) == 0;
}

int _rmdir(const char *path)
{
    unsigned long error;
    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (*path == '\0') {
        errno = ENOENT;
        return -1;
    }
    if (wcrt_directory_protected(path)) {
        errno = ENOTEMPTY;
        return -1;
    }
    if (RemoveDirectoryA(path)) return 0;
    error = GetLastError();
    if (error == WCRT_ERROR_DIR_NOT_EMPTY || error == WCRT_ERROR_DIRECTORY) {
        errno = ENOTEMPTY;
    } else if (error == WCRT_ERROR_ACCESS_DENIED ||
        error == WCRT_ERROR_SHARING_VIOLATION) {
        errno = EACCES;
    } else {
        errno = ENOENT;
    }
    return -1;
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
