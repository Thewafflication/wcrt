/** @file stat.c @brief Implements Microsoft 64-bit file status on Windows. */

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <windows.h>

/** @brief Converts a Windows file time to signed Unix seconds. */
static long long wcrt_unix_time(FILETIME value)
{
    ULARGE_INTEGER windows_time;
    windows_time.LowPart = value.dwLowDateTime;
    windows_time.HighPart = value.dwHighDateTime;
    return (long long)(windows_time.QuadPart / 10000000ULL) -
        11644473600LL;
}

int __cdecl _stat64(const char *path, struct _stat64 *result)
{
    WIN32_FILE_ATTRIBUTE_DATA data;
    unsigned long error;
    if (path == NULL || result == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &data)) {
        error = GetLastError();
        errno = error == ERROR_FILE_NOT_FOUND ||
            error == ERROR_PATH_NOT_FOUND ? ENOENT : EACCES;
        return -1;
    }
    memset(result, 0, sizeof(*result));
    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        result->st_mode = _S_IFDIR | _S_IREAD | _S_IEXEC;
    } else {
        result->st_mode = _S_IFREG | _S_IREAD;
    }
    if ((data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == 0) {
        result->st_mode |= _S_IWRITE;
    }
    result->st_nlink = 1;
    result->st_size = ((long long)data.nFileSizeHigh << 32) |
        data.nFileSizeLow;
    result->st_atime = wcrt_unix_time(data.ftLastAccessTime);
    result->st_mtime = wcrt_unix_time(data.ftLastWriteTime);
    result->st_ctime = wcrt_unix_time(data.ftCreationTime);
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
#endif
