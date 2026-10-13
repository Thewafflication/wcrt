/**
 * @file directory-errors.c
 * @brief Verifies root protection and OS error mapping without OS mutations.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#undef __declspec
#define __declspec(attribute)
#define GetFullPathNameA test_full_path
#define GetCurrentDirectoryA test_current_path
#define GetFileAttributesA test_attributes
#define RemoveDirectoryA test_remove_directory
#define SetCurrentDirectoryA test_change_directory
#define GetLastError test_last_error
#include "../../src/platform/windows/directory.c"

/** @brief Canonical path supplied by the deterministic backend. */
static const char *absolute_path;
/** @brief Current directory supplied by the deterministic backend. */
static const char *current_path = "C:\\owned";
/** @brief Directory attributes supplied by the deterministic backend. */
static unsigned long attributes = WCRT_FILE_ATTRIBUTE_DIRECTORY;
/** @brief Configurable Windows error and mutation success. */
static unsigned long last_error;
static int operation_ok;
static int remove_calls;

/** @brief Supplies a canonical path without filesystem lookup. */
unsigned long WCRT_WINAPI test_full_path(const char *path, unsigned long size,
    char *buffer, char **file_part)
{
    size_t length = strlen(absolute_path);
    (void)path;
    (void)file_part;
    if (length < size) strcpy(buffer, absolute_path);
    return (unsigned long)length;
}

/** @brief Supplies the current directory without changing process state. */
unsigned long WCRT_WINAPI test_current_path(unsigned long size, char *buffer)
{
    size_t length = strlen(current_path);
    if (length < size) strcpy(buffer, current_path);
    return (unsigned long)length;
}

/** @brief Supplies configurable attributes for syntactic root paths. */
unsigned long WCRT_WINAPI test_attributes(const char *path)
{
    (void)path;
    return attributes;
}

/** @brief Counts attempted directory removals without doing them. */
int WCRT_WINAPI test_remove_directory(const char *path)
{
    (void)path;
    ++remove_calls;
    return operation_ok;
}

/** @brief Supplies a controlled directory-change result. */
int WCRT_WINAPI test_change_directory(const char *path)
{
    (void)path;
    return operation_ok;
}

/** @brief Supplies the controlled last Windows error. */
unsigned long WCRT_WINAPI test_last_error(void)
{
    return last_error;
}

int main(void)
{
    static const char *protected_paths[] = {
        "C:\\", "\\\\server\\share", "\\\\server\\share\\",
        "C:\\owned", "c:\\owned\\"
    };
    static const unsigned long errors[] = { 145, 267, 5, 32, 2, 3 };
    static const int expected[] = {
        ENOTEMPTY, ENOTEMPTY, EACCES, EACCES, ENOENT, ENOENT
    };
    size_t index;

    for (index = 0; index < sizeof(protected_paths) /
        sizeof(protected_paths[0]);
        ++index) {
        absolute_path = protected_paths[index];
        remove_calls = 0;
        errno = 0;
        if (_rmdir("input") != -1 || errno != ENOTEMPTY || remove_calls != 0)
            return 1;
    }
    absolute_path = "\\\\absent\\share";
    attributes = WCRT_INVALID_FILE_ATTRIBUTES;
    last_error = 3;
    remove_calls = 0;
    if (_rmdir("input") != -1 || errno != ENOENT || remove_calls != 1)
        return 2;
    absolute_path = "C:\\other";
    for (index = 0; index < sizeof(errors) / sizeof(errors[0]); ++index) {
        last_error = errors[index];
        errno = 0;
        if (_rmdir("input") != -1 || errno != expected[index]) return 3;
        if (_chdir("input") != -1 || errno !=
            (errors[index] == 5 || errors[index] == 32 ? EACCES : ENOENT))
            return 4;
    }
    operation_ok = 1;
    errno = EDOM;
    if (_rmdir("input") != 0 || errno != EDOM ||
        _chdir("input") != 0 || errno != EDOM) return 5;
    remove_calls = 0;
    if (_rmdir(NULL) != -1 || errno != EINVAL || remove_calls != 0) return 6;
    return 0;
}
