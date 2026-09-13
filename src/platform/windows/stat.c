/** @file stat.c @brief Implements Microsoft 64-bit file status on Windows. */

#define WCRT_POSIX 1

#include <errno.h>
#include <io.h>
#include <string.h>
#include <sys/stat.h>
#include <windows.h>

#include "../../internal/stat.h"

/** @brief Converts a Windows file time to signed Unix seconds. */
static long long wcrt_unix_time(FILETIME value)
{
    ULARGE_INTEGER windows_time;
    windows_time.LowPart = value.dwLowDateTime;
    windows_time.HighPart = value.dwHighDateTime;
    return (long long)(windows_time.QuadPart / 10000000ULL) -
        11644473600LL;
}

/** @brief Maps Windows status failures to WCRT errno. */
static void wcrt_status_error(unsigned long error)
{
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND ||
        error == ERROR_INVALID_NAME) errno = ENOENT;
    else if (error == ERROR_INVALID_HANDLE) errno = EBADF;
    else errno = EACCES;
}

int __wcrt_status_handle(void *handle, struct wcrt_file_status *result)
{
    BY_HANDLE_FILE_INFORMATION data;
    unsigned long type;
    if (handle == NULL || handle == INVALID_HANDLE_VALUE || result == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(result, 0, sizeof(*result));
    type = GetFileType(handle);
    if (type == FILE_TYPE_PIPE) {
        result->mode = S_IFIFO | S_IRUSR | S_IWUSR;
        result->links = 1;
        return 0;
    }
    if (type == FILE_TYPE_CHAR) {
        result->mode = S_IFCHR | S_IRUSR | S_IWUSR;
        result->links = 1;
        return 0;
    }
    if (type != FILE_TYPE_DISK || !GetFileInformationByHandle(handle, &data)) {
        wcrt_status_error(GetLastError());
        return -1;
    }
    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        result->mode = S_IFDIR | S_IRUSR | S_IXUSR;
    } else {
        result->mode = S_IFREG | S_IRUSR;
    }
    if ((data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == 0) {
        result->mode |= S_IWUSR;
    }
    result->device = data.dwVolumeSerialNumber;
    result->inode = ((unsigned long long)data.nFileIndexHigh << 32) |
        data.nFileIndexLow;
    result->links = data.nNumberOfLinks;
    result->size = ((long long)data.nFileSizeHigh << 32) |
        data.nFileSizeLow;
    result->access_time = wcrt_unix_time(data.ftLastAccessTime);
    result->write_time = wcrt_unix_time(data.ftLastWriteTime);
    result->creation_time = wcrt_unix_time(data.ftCreationTime);
    return 0;
}

int __wcrt_status_path(const char *path, struct wcrt_file_status *result)
{
    void *handle;
    int status;
    if (path == NULL || result == NULL) {
        errno = EINVAL;
        return -1;
    }
    handle = CreateFileA(path, 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        wcrt_status_error(GetLastError());
        return -1;
    }
    status = __wcrt_status_handle(handle, result);
    CloseHandle(handle);
    return status;
}

/** @brief Copies shared metadata to the fixed Microsoft ABI. */
static void wcrt_status_microsoft(const struct wcrt_file_status *source,
    struct _stat64 *result)
{
    result->st_dev = source->device;
    result->st_ino = (unsigned short)source->inode;
    result->st_mode = (unsigned short)source->mode;
    result->st_nlink = (short)source->links;
    result->st_rdev = source->device;
    result->st_size = source->size;
    result->st_atime = source->access_time;
    result->st_mtime = source->write_time;
    result->st_ctime = source->creation_time;
}

int __cdecl _stat64(const char *path, struct _stat64 *result)
{
    struct wcrt_file_status status;
    if (result == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(result, 0, sizeof(*result));
    if (__wcrt_status_path(path, &status) != 0) return -1;
    wcrt_status_microsoft(&status, result);
    return 0;
}

int __cdecl _fstat64(int descriptor, struct _stat64 *result)
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
    wcrt_status_microsoft(&status, result);
    return 0;
}

int _access(const char *path, int mode)
{
    struct _stat64 status;
    unsigned short required = 0;

    if (path == NULL || (mode & ~6) != 0) {
        errno = EINVAL;
        return -1;
    }
    if (_stat64(path, &status) != 0) return -1;
    if ((mode & 4) != 0) required |= _S_IREAD;
    if ((mode & 2) != 0) required |= _S_IWRITE;
    if ((status.st_mode & required) != required) {
        errno = EACCES;
        return -1;
    }
    return 0;
}

#if defined(__i386__) || defined(_M_IX86)
int __cdecl _stat(const char *path, struct _stat64 *result)
{
    return _stat64(path, result);
}

int __cdecl _fstat(int descriptor, struct _stat64 *result)
{
    return _fstat64(descriptor, result);
}
#endif
