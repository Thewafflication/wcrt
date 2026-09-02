/** @file access.c @brief Verifies Microsoft path-access behavior. */
#include <errno.h>
#include <io.h>
#include <stdio.h>

int main(int argument_count, char **arguments)
{
    FILE *file;
    const char *path;

    if (argument_count != 2) return 1;
    path = arguments[1];
    file = fopen(path, "wb");
    if (file == 0 || fclose(file) != 0) return 2;
    if (_access(path, 0) != 0 || _access(path, 4) != 0 ||
        _access(path, 2) != 0 || _access(path, 6) != 0) return 3;
    errno = 0;
    if (_access(path, 1) != -1 || errno != EINVAL) return 4;
    if (_unlink(path) != 0) return 5;
    errno = 0;
    if (_access(path, 0) != -1 || errno != ENOENT) return 6;
    errno = 0;
    if (_access(0, 0) != -1 || errno != EINVAL) return 7;
    return 0;
}
