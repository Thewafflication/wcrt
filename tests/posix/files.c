/** @file files.c @brief Tests selected POSIX status and timestamp adapters. */
#define WCRT_POSIX 1
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>

/**
 * @brief Runs POSIX file adapter behavior checks.
 * @param argument_count Argument count.
 * @param arguments Test path in element one.
 * @return Zero on success, or a unique nonzero check identifier.
 */
int main(int argument_count, char **arguments)
{
    struct stat status;
    struct utimbuf requested;
    FILE *stream;
    time_t before;
    time_t after;
    if (argument_count != 2) return 1;

    stream = fopen(arguments[1], "wb");
    if (stream == NULL) return 2;
    if (fwrite("posix", 1, 5, stream) != 5 || fclose(stream) != 0) return 3;

    if (stat(arguments[1], &status) != 0) return 4;
    if (status.st_size != 5 || !S_ISREG(status.st_mode)) return 5;
    if ((status.st_mode & S_IRUSR) == 0) return 6;
    if (status.st_dev != 0 || status.st_ino != 0 || status.st_uid != 0 ||
        status.st_gid != 0 || status.st_rdev != 0) {
        return 7;
    }
    if (status.st_nlink != 1) return 8;
    if (status.st_atim.tv_nsec != 0 || status.st_mtim.tv_nsec != 0 ||
        status.st_ctim.tv_nsec != 0) return 9;
    if (status.st_atim.tv_sec != status.st_atime ||
        status.st_mtim.tv_sec != status.st_mtime ||
        status.st_ctim.tv_sec != status.st_ctime) return 10;

    requested.actime = 946684800LL;
    requested.modtime = 946684900LL;
    if (utime(arguments[1], &requested) != 0) return 11;
    if (stat(arguments[1], &status) != 0) return 12;
    if (status.st_atim.tv_sec != requested.actime ||
        status.st_mtim.tv_sec != requested.modtime) return 13;

    requested.actime = -1;
    errno = 0;
    if (utime(arguments[1], &requested) != -1 || errno != EINVAL) return 14;

    before = time(NULL) - 2;
    if (utime(arguments[1], NULL) != 0) return 15;
    after = time(NULL) + 2;
    if (stat(arguments[1], &status) != 0) return 16;
    if (status.st_mtime < before || status.st_mtime > after) return 17;

    errno = 0;
    if (stat("missing-directory/file", &status) != -1 ||
        errno != ENOENT) return 18;
    errno = 0;
    if (utime("missing-directory/file", NULL) != -1 ||
        errno != ENOENT) return 19;
    errno = 0;
    if (stat(NULL, &status) != -1 || errno != EINVAL) return 20;
    errno = 0;
    if (stat(arguments[1], NULL) != -1 || errno != EINVAL) return 21;
    errno = 0;
    if (utime(NULL, &requested) != -1 || errno != EINVAL) return 22;

    if (remove(arguments[1]) != 0) return 23;
    return 0;
}
