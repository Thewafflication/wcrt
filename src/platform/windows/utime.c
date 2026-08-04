/** @file utime.c @brief Implements Microsoft file-time updates on Windows. */

#include <errno.h>
#include <sys/utime.h>
#include <windows.h>

/** @brief Maximum supported signed 64-bit Microsoft Unix timestamp. */
#define WCRT_UTIME64_MAX 32535215999LL

/**
 * @brief Converts Unix seconds to a Windows file time.
 * @param seconds Signed seconds since the Unix epoch.
 * @param result Receives the converted Windows value.
 * @return Nonzero when the input is representable.
 */
static int wcrt_file_time(long long seconds, FILETIME *result)
{
    ULARGE_INTEGER value;
    if (seconds < 0 || seconds > WCRT_UTIME64_MAX) return 0;
    value.QuadPart = ((unsigned long long)seconds + 11644473600ULL) *
        10000000ULL;
    result->dwLowDateTime = value.LowPart;
    result->dwHighDateTime = value.HighPart;
    return 1;
}

/** @brief Maps a Windows file error to a Microsoft CRT error category. */
static int wcrt_utime_error(unsigned long error)
{
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
        return ENOENT;
    }
    if (error == ERROR_TOO_MANY_OPEN_FILES) return EMFILE;
    return EACCES;
}

/** @brief Applies already validated timestamps to a path. */
static int wcrt_set_times(const char *path, long long access_time,
    long long modification_time, int use_current)
{
    WIN32_FILE_ATTRIBUTE_DATA attributes;
    FILETIME access_value;
    FILETIME modification_value;
    HANDLE file;
    unsigned long error;
    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &attributes)) {
        errno = wcrt_utime_error(GetLastError());
        return -1;
    }
    if ((attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ||
        (attributes.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0) {
        errno = EACCES;
        return -1;
    }
    if (use_current) {
        GetSystemTimeAsFileTime(&access_value);
        modification_value = access_value;
    } else if (!wcrt_file_time(access_time, &access_value) ||
        !wcrt_file_time(modification_time, &modification_value)) {
        errno = EINVAL;
        return -1;
    }
    file = CreateFileA(path, FILE_WRITE_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
        OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        errno = wcrt_utime_error(GetLastError());
        return -1;
    }
    if (!SetFileTime(file, NULL, &access_value, &modification_value)) {
        error = GetLastError();
        CloseHandle(file);
        errno = wcrt_utime_error(error);
        return -1;
    }
    CloseHandle(file);
    return 0;
}

int __cdecl _utime32(const char *path, struct __utimbuf32 *times)
{
    return wcrt_set_times(path, times == NULL ? 0 : times->actime,
        times == NULL ? 0 : times->modtime, times == NULL);
}

int __cdecl _utime64(const char *path, struct __utimbuf64 *times)
{
    return wcrt_set_times(path, times == NULL ? 0 : times->actime,
        times == NULL ? 0 : times->modtime, times == NULL);
}
