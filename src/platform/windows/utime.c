/** @file utime.c @brief Implements Microsoft file-time updates on Windows. */

#include <errno.h>
#include <io.h>
#include <sys/stat.h>
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

/** @brief Converts explicit/current times within the selected range. */
static int wcrt_time_values(long long access_time, long long modification_time,
    int use_current, long long maximum, FILETIME *access_value,
    FILETIME *modification_value)
{
    if (use_current) {
        ULARGE_INTEGER current;
        GetSystemTimeAsFileTime(access_value);
        current.LowPart = access_value->dwLowDateTime;
        current.HighPart = access_value->dwHighDateTime;
        if (current.QuadPart < 116444736000000000ULL ||
            current.QuadPart / 10000000ULL - 11644473600ULL >
            (unsigned long long)maximum) {
            errno = EINVAL;
            return -1;
        }
        *modification_value = *access_value;
    } else if (access_time > maximum || modification_time > maximum ||
        !wcrt_file_time(access_time, access_value) ||
        !wcrt_file_time(modification_time, modification_value)) {
        errno = EINVAL;
        return -1;
    }
    return 0;
}

/** @brief Applies checked timestamps to a named regular file. */
static int wcrt_set_times(const char *path, long long access_time,
    long long modification_time, int use_current, long long maximum)
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
    if (wcrt_time_values(access_time, modification_time, use_current, maximum,
        &access_value, &modification_value) != 0) return -1;
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
        times == NULL ? 0 : times->modtime, times == NULL, 2147483647LL);
}

int __cdecl _utime64(const char *path, struct __utimbuf64 *times)
{
    return wcrt_set_times(path, times == NULL ? 0 : times->actime,
        times == NULL ? 0 : times->modtime, times == NULL, WCRT_UTIME64_MAX);
}

/** @brief Updates a borrowed disk handle without changing descriptor state. */
static int wcrt_set_descriptor_times(int descriptor, long long access_time,
    long long modification_time, int use_current, long long maximum)
{
    __wcrt_intptr_t native = _get_osfhandle(descriptor);
    FILETIME access_value;
    FILETIME modification_value;
    int type;
    struct _stat64 status;
    if (native == (__wcrt_intptr_t)-1) return -1;
    type = (int)GetFileType((HANDLE)native);
    if (type != FILE_TYPE_DISK && type != FILE_TYPE_CHAR &&
        type != FILE_TYPE_PIPE) {
        errno = EBADF;
        return -1;
    }
    if (type != FILE_TYPE_DISK) {
        errno = EINVAL;
        return -1;
    }
    if (_fstat64(descriptor, &status) != 0) return -1;
    if ((status.st_mode & 0xf000U) != _S_IFREG) {
        errno = EINVAL;
        return -1;
    }
    if (wcrt_time_values(access_time, modification_time, use_current, maximum,
        &access_value, &modification_value) != 0) return -1;
    if (!SetFileTime((HANDLE)native, NULL, &access_value,
        &modification_value)) {
        errno = wcrt_utime_error(GetLastError());
        return -1;
    }
    return 0;
}

int __cdecl _futime32(int descriptor, struct __utimbuf32 *times)
{
    return wcrt_set_descriptor_times(descriptor,
        times == NULL ? 0 : times->actime,
        times == NULL ? 0 : times->modtime, times == NULL, 2147483647LL);
}

int __cdecl _futime64(int descriptor, struct __utimbuf64 *times)
{
    return wcrt_set_descriptor_times(descriptor,
        times == NULL ? 0 : times->actime,
        times == NULL ? 0 : times->modtime, times == NULL, WCRT_UTIME64_MAX);
}
