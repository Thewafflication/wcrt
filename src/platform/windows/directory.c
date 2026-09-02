/** @file directory.c @brief Implements Microsoft directory operations. */

#include <direct.h>
#include <errno.h>

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_ERROR_FILE_NOT_FOUND 2UL
#define WCRT_ERROR_PATH_NOT_FOUND 3UL
#define WCRT_ERROR_ACCESS_DENIED 5UL
#define WCRT_ERROR_ALREADY_EXISTS 183UL

__declspec(dllimport) int WCRT_WINAPI CreateDirectoryA(const char *path,
    void *security);
__declspec(dllimport) unsigned long WCRT_WINAPI GetLastError(void);

int _mkdir(const char *path)
{
    unsigned long error;

    if (path == 0) {
        errno = EINVAL;
        return -1;
    }
    if (CreateDirectoryA(path, 0)) return 0;
    error = GetLastError();
    if (error == WCRT_ERROR_ALREADY_EXISTS) {
        errno = EEXIST;
    } else if (error == WCRT_ERROR_FILE_NOT_FOUND ||
        error == WCRT_ERROR_PATH_NOT_FOUND) {
        errno = ENOENT;
    } else if (error == WCRT_ERROR_ACCESS_DENIED) {
        errno = EACCES;
    } else {
        errno = EACCES;
    }
    return -1;
}
