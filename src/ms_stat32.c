/**
 * @file ms_stat32.c
 * @brief Implements the explicit narrow file-status source route.
 */

#include <errno.h>
#include <io.h>
#include <string.h>
#include <sys/stat.h>

#include "internal/stat.h"

/** @brief Checks the inclusive signed 32-bit timestamp range. */
static int wcrt_stat32_time(long long value)
{
    return value >= -2147483647LL - 1LL && value <= 2147483647LL;
}

/** @brief Validates all size/time fields before copying any narrow field. */
static int wcrt_stat32_copy(const struct wcrt_file_status *source,
    struct wcrt_stat32 *result)
{
    if (source->size < 0 || source->size > 2147483647LL ||
        !wcrt_stat32_time(source->access_time) ||
        !wcrt_stat32_time(source->write_time) ||
        !wcrt_stat32_time(source->creation_time)) {
        errno = ERANGE;
        return -1;
    }
    result->st_dev = source->device;
    result->st_ino = (unsigned short)source->inode;
    result->st_mode = (unsigned short)source->mode;
    result->st_nlink = (short)source->links;
    result->st_rdev = source->device;
    result->st_size = (long)source->size;
    result->st_atime = (long)source->access_time;
    result->st_mtime = (long)source->write_time;
    result->st_ctime = (long)source->creation_time;
    return 0;
}

int __cdecl wcrt_stat32(const char *path, struct wcrt_stat32 *result)
{
    struct wcrt_file_status status;
    if (result == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(result, 0, sizeof(*result));
    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (__wcrt_status_path(path, &status) != 0) return -1;
    return wcrt_stat32_copy(&status, result);
}

int __cdecl wcrt_fstat32(int descriptor, struct wcrt_stat32 *result)
{
    struct wcrt_file_status status;
    __wcrt_intptr_t handle;
    if (result == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(result, 0, sizeof(*result));
    handle = _get_osfhandle(descriptor);
    if (handle == (__wcrt_intptr_t)-1 ||
        __wcrt_status_handle((void *)handle, &status) != 0) return -1;
    return wcrt_stat32_copy(&status, result);
}
