/** @file paths.c @brief Verifies selected POSIX path adapters. */
#define WCRT_POSIX 1
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argument_count, char **arguments)
{
    FILE *file;
    if (argument_count != 4) return 1;

    file = fopen(arguments[1], "wb");
    if (file == NULL || fclose(file) != 0) return 2;
    if (access(arguments[1], 0) != 0 || access(arguments[1], 4) != 0 ||
        access(arguments[1], 2) != 0 || access(arguments[1], 6) != 0) return 3;
    errno = 0;
    if (access(arguments[1], 1) != -1 || errno != EINVAL) return 4;
    if (unlink(arguments[1]) != 0) return 5;
    errno = 0;
    if (access(arguments[1], 0) != -1 || errno != ENOENT) return 6;

    if (mkdir(arguments[2], 0700) != 0) return 7;
    errno = 0;
    if (mkdir(arguments[2], 0777) != -1 || errno != EEXIST) return 8;
    errno = 0;
    if (mkdir(arguments[3], 0700) != -1 || errno != ENOENT) return 9;
    errno = 0;
    if (mkdir(NULL, 0700) != -1 || errno != EINVAL) return 10;
    return 0;
}
