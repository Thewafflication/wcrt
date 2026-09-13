/** @file mkdir.c @brief Verifies Microsoft directory creation behavior. */
#include <direct.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

int main(int argument_count, char **arguments)
{
    char current[MAX_PATH];
    char absolute[MAX_PATH];
    char *allocated;
    if (argument_count != 3) return 1;
    if (_mkdir(arguments[1]) != 0) return 2;
    errno = 0;
    if (_mkdir(arguments[1]) != -1 || errno != EEXIST) return 3;
    errno = 0;
    if (_mkdir(arguments[2]) != -1 || errno != ENOENT) return 4;
    errno = 0;
    if (_mkdir(0) != -1 || errno != EINVAL) return 5;
    if (_getcwd(current, sizeof(current)) == NULL ||
        _fullpath(absolute, ".", sizeof(absolute)) == NULL ||
        strcmp(current, absolute) != 0) return 6;
    allocated = _getcwd(NULL, 0);
    if (allocated == NULL || strcmp(allocated, current) != 0) return 7;
    free(allocated);
    allocated = _fullpath(NULL, ".", 0);
    if (allocated == NULL || strcmp(allocated, current) != 0) return 8;
    free(allocated);
    errno = 0;
    if (_getcwd(current, 1) != NULL || errno != ERANGE) return 9;
    errno = 0;
    if (_fullpath(absolute, ".", 1) != NULL || errno != ERANGE) return 10;
    return 0;
}
