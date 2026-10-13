/**
 * @file drive-query-errors.c
 * @brief Tests drive prefixes and provider failures without OS mutations.
 */

#include <errno.h>
#include <string.h>

#undef __declspec
#define __declspec(attribute)
#define GetCurrentDirectoryA test_current_directory
#define GetLogicalDrives test_logical_drives
#define GetLastError test_last_error
#include "../../src/platform/windows/directory.c"

static const char *current;
static unsigned long mask;
static unsigned long current_length;
static unsigned long error;

/** @brief Supplies the selected current-directory buffer partition. */
unsigned long WCRT_WINAPI test_current_directory(unsigned long size,
    char *buffer)
{
    if (current_length != 0) return current_length;
    if (current == NULL) return 0;
    if (strlen(current) < size) strcpy(buffer, current);
    return (unsigned long)strlen(current);
}

/** @brief Supplies a logical-drive mask without opening any drive. */
unsigned long WCRT_WINAPI test_logical_drives(void)
{
    return mask;
}

/** @brief Supplies an OS query error without altering process state. */
unsigned long WCRT_WINAPI test_last_error(void)
{
    return error;
}

int main(void)
{
    char path[] = "A:\\owned";
    char boundary[MAX_PATH];
    static const char *non_drives[] = {
        "\\\\server\\share", "\\rooted", "0:\\", "x", "?wrong"
    };
    int index;
    for (index = 0; index < 26; ++index) {
        current = path;
        path[0] = (char)('A' + index);
        errno = EDOM;
        if (_getdrive() != index + 1 || errno != EDOM) return 1;
        path[0] = (char)('a' + index);
        if (_getdrive() != index + 1 || errno != EDOM) return 2;
        mask = 1UL << index;
        if (_getdrives() != mask || errno != EDOM) return 3;
    }
    for (index = 0; index < 5; ++index) {
        current = non_drives[index];
        if (_getdrive() != 0 || errno != EDOM) return 4;
    }
    mask = 0;
    error = WCRT_ERROR_ACCESS_DENIED;
    if (_getdrives() != 0 || errno != EDOM) return 5;
    current = NULL;
    if (_getdrive() != 0 || errno != EACCES) return 6;
    memset(boundary, 'x', sizeof(boundary) - 1);
    boundary[0] = 'Z';
    boundary[1] = ':';
    boundary[MAX_PATH - 1] = '\0';
    current = boundary;
    errno = EDOM;
    if (_getdrive() != 26 || errno != EDOM) return 7;
    current_length = MAX_PATH;
    if (_getdrive() != 0 || errno != ERANGE) return 8;
    return 0;
}
