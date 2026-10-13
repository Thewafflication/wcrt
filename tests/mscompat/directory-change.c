/**
 * @file directory-change.c
 * @brief Tests owned directory changes and removal error partitions.
 */

#include <direct.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argument_count, char **arguments)
{
    char original[MAX_PATH];
    char absolute[MAX_PATH];
    char current[MAX_PATH];
    FILE *stream;
    int failure = 0;

    if (argument_count != 2) return 1;
    if (_getcwd(original, sizeof(original)) == NULL) return 2;
    if (_fullpath(absolute, arguments[1], sizeof(absolute)) == NULL) return 3;
    if (_mkdir(absolute) != 0) return 4;
    errno = EDOM;
    if (_chdir(absolute) != 0 || errno != EDOM) return 5;
    if (_getcwd(current, sizeof(current)) == NULL ||
        _stricmp(current, absolute) != 0) failure = 6;
    errno = EDOM;
    if (!failure && (_mkdir("child") != 0 || errno != EDOM)) failure = 7;
    if (!failure && _chdir("child") != 0) failure = 8;
    errno = 0;
    if (!failure && (_rmdir(".") != -1 || errno != ENOTEMPTY)) failure = 9;
    if (!failure && _chdir("..") != 0) failure = 10;
    errno = EDOM;
    if (!failure && (_rmdir("child") != 0 || errno != EDOM)) failure = 11;
    errno = 0;
    if (!failure && (_chdir("missing") != -1 || errno != ENOENT)) failure = 12;
    errno = 0;
    if (!failure && (_rmdir("missing") != -1 || errno != ENOENT)) failure = 13;
    errno = 0;
    if (!failure && (_chdir(NULL) != -1 || errno != EINVAL)) failure = 14;
    errno = 0;
    if (!failure && (_rmdir(NULL) != -1 || errno != EINVAL)) failure = 15;
    if (!failure && (_rmdir("") != -1 || errno != ENOENT)) failure = 27;
    stream = fopen("owned.txt", "wb");
    if (stream == NULL) failure = 16;
    else if (fclose(stream) != 0) failure = 17;
    errno = 0;
    if (!failure && (_chdir("owned.txt") != -1 || errno != ENOENT))
        failure = 18;
    errno = 0;
    if (!failure && (_rmdir("owned.txt") != -1 || errno != ENOTEMPTY))
        failure = 19;
    if (_chdir(original) != 0) return 20;
    errno = 0;
    if (!failure && (_rmdir(absolute) != -1 || errno != ENOTEMPTY))
        failure = 21;
    if (_chdir(absolute) != 0) return 22;
    if (remove("owned.txt") != 0 && !failure) failure = 23;
    if (_chdir(original) != 0) return 24;
    errno = EDOM;
    if (_rmdir(absolute) != 0 && !failure) failure = 25;
    if (errno != EDOM && !failure) failure = 26;
    return failure;
}
