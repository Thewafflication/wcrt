/**
 * @file posix_files.c
 * @brief Implements selected POSIX file adapters on Windows.
 */

#define WCRT_POSIX 1

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <utime.h>

/**
 * @brief Copies one second-resolution timestamp into both public forms.
 * @param value Source seconds since the Unix epoch.
 * @param precise Receives the POSIX timespec representation.
 * @param direct Receives the WPM migration seconds representation.
 */
static void wcrt_posix_time(long long value, struct timespec *precise,
    time_t *direct)
{
    precise->tv_sec = (time_t)value;
    precise->tv_nsec = 0;
    *direct = (time_t)value;
}

int stat(const char *path, struct stat *result)
{
    struct _stat64 source;
    if (result == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(result, 0, sizeof(*result));
    if (_stat64(path, &source) != 0) return -1;

    result->st_dev = (dev_t)source.st_dev;
    result->st_ino = (ino_t)source.st_ino;
    result->st_mode = (mode_t)source.st_mode;
    result->st_nlink = (nlink_t)source.st_nlink;
    result->st_uid = (uid_t)source.st_uid;
    result->st_gid = (gid_t)source.st_gid;
    result->st_rdev = (dev_t)source.st_rdev;
    result->st_size = (off_t)source.st_size;
    wcrt_posix_time(source.st_atime, &result->st_atim,
        &result->st_atime);
    wcrt_posix_time(source.st_mtime, &result->st_mtim,
        &result->st_mtime);
    wcrt_posix_time(source.st_ctime, &result->st_ctim,
        &result->st_ctime);
    return 0;
}

int utime(const char *path, const struct utimbuf *times)
{
    struct __utimbuf64 source;
    if (times == NULL) return _utime64(path, NULL);
    source.actime = (long long)times->actime;
    source.modtime = (long long)times->modtime;
    return _utime64(path, &source);
}
