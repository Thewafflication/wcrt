/** @file unlink.c @brief Verifies Microsoft file removal behavior. */
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
    if (file == 0) return 2;
    if (fputs("wcrt", file) == EOF || fclose(file) != 0) return 3;
    if (_unlink(path) != 0) return 4;
    errno = 0;
    if (_unlink(path) != -1 || errno != ENOENT) return 5;
    errno = 0;
    if (_unlink(0) != -1 || errno != EINVAL) return 6;
    return 0;
}
