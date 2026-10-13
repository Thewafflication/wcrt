/**
 * @file drive-directory-errors.c
 * @brief Tests alternate drive resolution without real drive changes.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static void *test_allocate(size_t size);
#undef __declspec
#define __declspec(attribute)
#define GetCurrentDirectoryA test_current_directory
#define GetLogicalDrives test_logical_drives
#define GetFullPathNameA test_full_path
#define SetCurrentDirectoryA test_change_directory
#define GetLastError test_last_error
#define malloc test_allocate
#include "../../src/platform/windows/directory.c"
#undef malloc

static unsigned long mask = (1UL << 2) | (1UL << 25);
static unsigned long error;
static int operation_ok = 1;
static int change_calls;
static int full_calls;
static int input_ok;
static int fail_allocation;

/** @brief Injects allocation failure into the directory query backend. */
static void *test_allocate(size_t size)
{
    return fail_allocation ? NULL : malloc(size);
}

/** @brief Supplies current directory C without process mutation. */
unsigned long WCRT_WINAPI test_current_directory(unsigned long size,
    char *buffer)
{
    if (size > 8) strcpy(buffer, "C:\\owned");
    return 8;
}

/** @brief Supplies only two available drives in the test model. */
unsigned long WCRT_WINAPI test_logical_drives(void)
{
    return mask;
}

/** @brief Resolves the fake inactive drive's remembered directory. */
unsigned long WCRT_WINAPI test_full_path(const char *path,
    unsigned long size, char *buffer, char **file_part)
{
    (void)file_part;
    ++full_calls;
    input_ok = strcmp(path, "Z:.") == 0;
    if (error != 0) return 0;
    if (size > 9) strcpy(buffer, "Z:\\stored");
    return size > 9 ? 9 : 10;
}

/** @brief Records the requested drive and supplies a change result. */
int WCRT_WINAPI test_change_directory(const char *path)
{
    ++change_calls;
    input_ok = strcmp(path, "Z:") == 0;
    return operation_ok;
}

/** @brief Supplies a deterministic OS error. */
unsigned long WCRT_WINAPI test_last_error(void)
{
    return error;
}

int main(void)
{
    char buffer[32];
    char *allocated;
    int index;
    int invalid[] = {-1, 0, 27, 2147483647};
    errno = EDOM;
    if (_getdcwd(26, buffer, sizeof(buffer)) != buffer ||
        strcmp(buffer, "Z:\\stored") || !input_ok || errno != EDOM)
        return 1;
    allocated = _getdcwd(26, NULL, 10);
    if (allocated == NULL || strcmp(allocated, "Z:\\stored") ||
        errno != EDOM) return 2;
    free(allocated);
    if (_getdcwd(3, buffer, sizeof(buffer)) != buffer ||
        strcmp(buffer, "C:\\owned") || full_calls != 2) return 3;
    if (_chdrive(26) != 0 || !input_ok || errno != EDOM) return 4;
    change_calls = 0;
    for (index = 0; index < 4; ++index) {
        if (_chdrive(invalid[index]) != -1 || errno != EACCES ||
            change_calls != 0) return 5;
    }
    if (_getdcwd(-1, buffer, sizeof(buffer)) != NULL || errno != EACCES ||
        _getdcwd(1, buffer, sizeof(buffer)) != NULL || errno != EACCES)
        return 6;
    errno = EDOM;
    if (_getdcwd(26, buffer, 9) != NULL || errno != ERANGE) return 7;
    operation_ok = 0;
    error = WCRT_ERROR_ACCESS_DENIED;
    if (_chdrive(26) != -1 || errno != EACCES || !input_ok) return 8;
    if (_getdcwd(26, buffer, sizeof(buffer)) != NULL || errno != EACCES)
        return 9;
    error = WCRT_ERROR_INSUFFICIENT_BUFFER;
    if (_getdcwd(26, buffer, sizeof(buffer)) != NULL || errno != ERANGE)
        return 10;
    error = 0;
    fail_allocation = 1;
    if (_getdcwd(26, NULL, 10) != NULL || errno != ENOMEM) return 11;
    return 0;
}
