/**
 * @file drive-directory.c
 * @brief Tests current-drive adapters inside an owned directory.
 */

#include <direct.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main(int argument_count, char **arguments)
{
    char saved[_MAX_PATH];
    char current[_MAX_PATH];
    char buffer[_MAX_PATH + 2];
    char *allocated;
    int drive;
    int result = 0;
    int length;

    if (argument_count != 2 || _getcwd(saved, _MAX_PATH) != saved)
        return 1;
    if (_mkdir(arguments[1]) != 0) return 2;
    if (_chdir(arguments[1]) != 0) {
        _rmdir(arguments[1]);
        return 3;
    }
    if (_getcwd(current, _MAX_PATH) != current) {
        _chdir(saved);
        _rmdir(arguments[1]);
        return 4;
    }
    drive = _getdrive();
    errno = EDOM;
    if (_getdcwd(0, buffer, _MAX_PATH) != buffer ||
        strcmp(buffer, current) || errno != EDOM) result = 5;
    if (drive != 0) {
        if (_getdcwd(drive, buffer, _MAX_PATH) != buffer ||
            strcmp(buffer, current) || _chdrive(drive) != 0 ||
            errno != EDOM || _getcwd(buffer, _MAX_PATH) != buffer ||
            strcmp(buffer, current)) result = 6;
    }
    allocated = _getdcwd(0, NULL, _MAX_PATH);
    if (allocated == NULL || strcmp(allocated, current)) result = 7;
    free(allocated);
    length = (int)strlen(current);
    memset(buffer, '!', sizeof(buffer));
    if (_getdcwd(0, buffer + 1, length + 1) != buffer + 1 ||
        strcmp(buffer + 1, current) || buffer[0] != '!' ||
        buffer[length + 2] != '!') result = 8;
    errno = 0;
    if (_getdcwd(0, buffer, length) != NULL || errno != ERANGE) result = 9;
    errno = 0;
    if (_getdcwd(0, buffer, 0) != NULL || errno != EINVAL) result = 10;
    errno = 0;
    if (_getdcwd(0, NULL, -1) != NULL || errno != EINVAL) result = 11;
    errno = 0;
    if (_getdcwd(27, buffer, _MAX_PATH) != NULL || errno != EACCES)
        result = 12;
    errno = 0;
    if (_chdrive(0) != -1 || errno != EACCES ||
        _getcwd(buffer, _MAX_PATH) != buffer || strcmp(buffer, current))
        result = 13;
    if (_chdir(saved) != 0) return 14;
    if (_rmdir(arguments[1]) != 0) return 15;
    return result;
}
