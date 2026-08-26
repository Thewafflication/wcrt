/**
 * @file dirent.c
 * @brief Implements selected POSIX directory streams on Windows.
 */

#define WCRT_POSIX 1

#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_FILE_ATTRIBUTE_DIRECTORY 0x10UL
#define WCRT_ERROR_FILE_NOT_FOUND 2UL
#define WCRT_ERROR_PATH_NOT_FOUND 3UL
#define WCRT_ERROR_ACCESS_DENIED 5UL
#define WCRT_ERROR_NO_MORE_FILES 18UL
#define WCRT_ERROR_NOT_ENOUGH_MEMORY 8UL
#define WCRT_ERROR_TOO_MANY_OPEN_FILES 4UL
#define WCRT_INVALID_HANDLE ((void *)(long long)-1)
#define WCRT_FIND_NAME_CAPACITY 260

/** @brief Windows 64-bit time split into two words. */
struct wcrt_filetime {
    unsigned long low;
    unsigned long high;
};

/**
 * @brief FindFirstFileA result layout used without an SDK header dependency.
 */
struct wcrt_find_data {
    unsigned long attributes;
    struct wcrt_filetime creation_time;
    struct wcrt_filetime access_time;
    struct wcrt_filetime write_time;
    unsigned long size_high;
    unsigned long size_low;
    unsigned long reserved0;
    unsigned long reserved1;
    char name[WCRT_FIND_NAME_CAPACITY];
    char alternate_name[14];
};

/** @brief Private state associated with a selected directory stream. */
struct __wcrt_DIR {
    void *handle;
    int pending;
    struct wcrt_find_data data;
    struct dirent entry;
    char pattern[1];
};

__declspec(dllimport) void *WCRT_WINAPI FindFirstFileA(
    const char *pattern, struct wcrt_find_data *data);
__declspec(dllimport) int WCRT_WINAPI FindNextFileA(
    void *handle, struct wcrt_find_data *data);
__declspec(dllimport) int WCRT_WINAPI FindClose(void *handle);
__declspec(dllimport) unsigned long WCRT_WINAPI GetLastError(void);

/** @brief Maps directory-related Windows failures to selected errno values. */
static void wcrt_dirent_error(unsigned long error)
{
    if (error == WCRT_ERROR_FILE_NOT_FOUND ||
        error == WCRT_ERROR_PATH_NOT_FOUND) {
        errno = ENOENT;
    } else if (error == WCRT_ERROR_ACCESS_DENIED) {
        errno = EACCES;
    } else if (error == WCRT_ERROR_NOT_ENOUGH_MEMORY) {
        errno = ENOMEM;
    } else if (error == WCRT_ERROR_TOO_MANY_OPEN_FILES) {
        errno = EMFILE;
    } else {
        errno = EIO;
    }
}

/** @brief Opens or reopens the native search owned by a directory stream. */
static int wcrt_dirent_begin(DIR *directory)
{
    directory->handle = FindFirstFileA(directory->pattern,
        &directory->data);
    if (directory->handle == WCRT_INVALID_HANDLE) {
        directory->handle = NULL;
        directory->pending = 0;
        wcrt_dirent_error(GetLastError());
        return -1;
    }
    directory->pending = 1;
    return 0;
}

DIR *opendir(const char *path)
{
    DIR *directory;
    size_t length;
    size_t pattern_length;
    int separator;
    if (path == NULL || *path == '\0') {
        errno = ENOENT;
        return NULL;
    }
    length = strlen(path);
    separator = path[length - 1] != '/' && path[length - 1] != '\\';
    pattern_length = length + (separator ? 2U : 1U);
    directory = (DIR *)malloc(sizeof(*directory) + pattern_length);
    if (directory == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(directory->pattern, path, length);
    if (separator) directory->pattern[length++] = '\\';
    directory->pattern[length++] = '*';
    directory->pattern[length] = '\0';
    directory->handle = NULL;
    directory->pending = 0;
    if (wcrt_dirent_begin(directory) != 0) {
        free(directory);
        return NULL;
    }
    return directory;
}

struct dirent *readdir(DIR *directory)
{
    size_t index;
    unsigned long error;
    if (directory == NULL || directory->handle == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (directory->pending) {
        directory->pending = 0;
    } else if (!FindNextFileA(directory->handle, &directory->data)) {
        error = GetLastError();
        if (error != WCRT_ERROR_NO_MORE_FILES) wcrt_dirent_error(error);
        return NULL;
    }
    directory->entry.d_ino = 0;
    directory->entry.d_type =
        (directory->data.attributes & WCRT_FILE_ATTRIBUTE_DIRECTORY) ?
        DT_DIR : DT_REG;
    for (index = 0; index < WCRT_DIRENT_NAME_MAX &&
        directory->data.name[index] != '\0'; ++index) {
        directory->entry.d_name[index] = directory->data.name[index];
    }
    directory->entry.d_name[index] = '\0';
    return &directory->entry;
}

void rewinddir(DIR *directory)
{
    if (directory == NULL) {
        errno = EINVAL;
        return;
    }
    if (directory->handle != NULL) FindClose(directory->handle);
    directory->handle = NULL;
    directory->pending = 0;
    wcrt_dirent_begin(directory);
}

int closedir(DIR *directory)
{
    int result;
    if (directory == NULL || directory->handle == NULL) {
        errno = EINVAL;
        return -1;
    }
    result = FindClose(directory->handle);
    if (!result) wcrt_dirent_error(GetLastError());
    directory->handle = NULL;
    free(directory);
    return result ? 0 : -1;
}
