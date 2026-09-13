/**
 * @file posix_files.c
 * @brief Implements selected POSIX file adapters on Windows.
 */

#define WCRT_POSIX 1

#include <errno.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <utime.h>

#include "../../internal/stat.h"

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

/** @brief Copies shared Windows metadata to the POSIX-facing structure. */
static void wcrt_posix_status(const struct wcrt_file_status *source,
    struct stat *result)
{
    result->st_dev = (dev_t)source->device;
    result->st_ino = (ino_t)source->inode;
    result->st_mode = (mode_t)source->mode;
    result->st_nlink = (nlink_t)source->links;
    result->st_rdev = (dev_t)source->device;
    result->st_size = (off_t)source->size;
    wcrt_posix_time(source->access_time, &result->st_atim,
        &result->st_atime);
    wcrt_posix_time(source->write_time, &result->st_mtim,
        &result->st_mtime);
    wcrt_posix_time(source->creation_time, &result->st_ctim,
        &result->st_ctime);
}

int stat(const char *path, struct stat *result)
{
    struct wcrt_file_status source;
    if (result == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(result, 0, sizeof(*result));
    if (__wcrt_status_path(path, &source) != 0) return -1;
    wcrt_posix_status(&source, result);
    return 0;
}

int fstat(int descriptor, struct stat *result)
{
    struct wcrt_file_status source;
    __wcrt_intptr_t handle;
    if (result == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(result, 0, sizeof(*result));
    handle = _get_osfhandle(descriptor);
    if (handle == (__wcrt_intptr_t)-1 ||
        __wcrt_status_handle((void *)handle, &source) != 0) return -1;
    wcrt_posix_status(&source, result);
    return 0;
}

int lstat(const char *path, struct stat *result)
{
    return stat(path, result);
}

int chmod(const char *path, mode_t mode)
{
    return _chmod(path, (int)mode);
}

int utime(const char *path, const struct utimbuf *times)
{
    struct __utimbuf64 source;
    if (times == NULL) return _utime64(path, NULL);
    source.actime = (long long)times->actime;
    source.modtime = (long long)times->modtime;
    return _utime64(path, &source);
}

int unlink(const char *path)
{
    return _unlink(path);
}

int access(const char *path, int mode)
{
    return _access(path, mode);
}

int mkdir(const char *path, mode_t mode)
{
    (void)mode;
    return _mkdir(path);
}
