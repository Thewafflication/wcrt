/** @file utime-errors.c @brief Tests current-time ranges and Windows errors. */
#include <errno.h>
#include <string.h>
#define GetFileAttributesExA test_time_attributes
#define GetSystemTimeAsFileTime test_time_clock
#define CreateFileA test_time_open
#define SetFileTime test_time_write
#define CloseHandle test_time_close
#define GetLastError test_time_error
#define GetFileType test_time_type
#define _get_osfhandle test_time_descriptor
#define _fstat64 test_time_status
#include "../../src/platform/windows/utime.c"

static unsigned long long clock_ticks = 137919572480000000ULL;
static unsigned long attributes;
static unsigned long native_error;
static int attribute_failure;
static int open_failure;
static int write_failure;
static int open_calls;
static int write_calls;
static int close_calls;
static FILETIME written;
static FILETIME written_access;
static int provider_contract_error;
static int descriptor_failure;
static DWORD descriptor_type = FILE_TYPE_DISK;
static int clock_calls;
static int descriptor_directory;
static int descriptor_status_failure;

/** @brief Supplies regular/directory metadata or a controlled query error. */
int __cdecl test_time_status(int descriptor, struct _stat64 *status)
{
    if (descriptor != 7 || descriptor_status_failure) {
        errno = EACCES;
        return -1;
    }
    memset(status, 0, sizeof(*status));
    status->st_mode = descriptor_directory ? _S_IFDIR : _S_IFREG;
    return 0;
}

/** @brief Supplies a borrowed descriptor handle or controlled lookup error. */
__wcrt_intptr_t test_time_descriptor(int descriptor)
{
    if (descriptor != 7 || descriptor_failure) {
        errno = EBADF;
        return (__wcrt_intptr_t)-1;
    }
    return (__wcrt_intptr_t)7;
}

/** @brief Supplies the borrowed handle type without dereferencing it. */
DWORD WINAPI test_time_type(HANDLE handle)
{
    if (handle != (HANDLE)7) provider_contract_error = 1;
    return descriptor_type;
}

/** @brief Supplies attributes or a retained query failure. */
BOOL WINAPI test_time_attributes(LPCSTR path,
    GET_FILEEX_INFO_LEVELS level, LPVOID result)
{
    WIN32_FILE_ATTRIBUTE_DATA *data = result;
    if (path == NULL || level != GetFileExInfoStandard || result == NULL) {
        provider_contract_error = 1;
    }
    if (attribute_failure) return 0;
    memset(data, 0, sizeof(*data));
    data->dwFileAttributes = attributes;
    return 1;
}

/** @brief Supplies a clock without changing the machine's time. */
VOID WINAPI test_time_clock(LPFILETIME result)
{
    ++clock_calls;
    result->dwLowDateTime = (DWORD)clock_ticks;
    result->dwHighDateTime = (DWORD)(clock_ticks >> 32);
}

/** @brief Counts opens while supplying a controlled fake handle. */
HANDLE WINAPI test_time_open(LPCSTR path, DWORD access, DWORD sharing,
    LPSECURITY_ATTRIBUTES security, DWORD creation, DWORD flags,
    HANDLE template)
{
    if (strcmp(path, "owned") || access != FILE_WRITE_ATTRIBUTES ||
        sharing != (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE) ||
        security != NULL || creation != OPEN_EXISTING || flags != 0 ||
        template != NULL) provider_contract_error = 1;
    ++open_calls;
    return open_failure ? INVALID_HANDLE_VALUE : (HANDLE)7;
}

/** @brief Records submitted timestamps without modifying a file. */
BOOL WINAPI test_time_write(HANDLE handle, const FILETIME *creation,
    const FILETIME *access, const FILETIME *modification)
{
    if (handle != (HANDLE)7 || creation != NULL || access == NULL ||
        modification == NULL) provider_contract_error = 1;
    ++write_calls;
    written = *modification;
    written_access = *access;
    return !write_failure;
}

/** @brief Counts closes and deliberately changes the native error. */
BOOL WINAPI test_time_close(HANDLE handle)
{
    if (handle != (HANDLE)7) provider_contract_error = 1;
    ++close_calls;
    native_error = ERROR_INVALID_HANDLE;
    return 1;
}

/** @brief Returns the selected native failure code. */
DWORD WINAPI test_time_error(void)
{
    return native_error;
}

int main(void)
{
    struct __utimbuf32 explicit32;
    struct __utimbuf64 explicit64;
    if (_utime32("owned", NULL) != -1 || errno != EINVAL ||
        open_calls != 0 || write_calls != 0) return 1;
    clock_ticks = 137919572479999999ULL;
    errno = EDOM;
    if (_utime32("owned", NULL) != 0 || errno != EDOM ||
        written.dwLowDateTime != (DWORD)clock_ticks ||
        written.dwHighDateTime != (DWORD)(clock_ticks >> 32)) return 2;
    clock_ticks = 116444735999999999ULL;
    open_calls = 0;
    if (_utime64("owned", NULL) != -1 || errno != EINVAL ||
        open_calls != 0) return 3;
    clock_ticks = 116444736000000000ULL;
    if (_utime32("owned", NULL) != 0) return 4;
    clock_ticks = 441796895999999999ULL;
    if (_utime64("owned", NULL) != 0) return 5;
    clock_ticks = 441796896000000000ULL;
    open_calls = 0;
    if (_utime64("owned", NULL) != -1 || errno != EINVAL ||
        open_calls != 0) return 6;
    clock_ticks = 116444736000000000ULL;
    if (_utime32(NULL, NULL) != -1 || errno != EINVAL) return 7;
    attribute_failure = 1;
    native_error = ERROR_FILE_NOT_FOUND;
    if (_utime64("missing", NULL) != -1 || errno != ENOENT) return 8;
    attribute_failure = 0;
    attributes = FILE_ATTRIBUTE_READONLY;
    if (_utime32("owned", NULL) != -1 || errno != EACCES) return 9;
    attributes = FILE_ATTRIBUTE_DIRECTORY;
    if (_utime64("owned", NULL) != -1 || errno != EACCES) return 10;
    attributes = 0;
    explicit32.actime = 2147483647L;
    explicit32.modtime = 2147483647L;
    if (_utime32("owned", &explicit32) != 0) return 13;
    explicit32.actime = -1;
    open_calls = 0;
    if (_utime32("owned", &explicit32) != -1 || errno != EINVAL ||
        open_calls != 0) return 14;
    explicit64.actime = WCRT_UTIME64_MAX;
    explicit64.modtime = WCRT_UTIME64_MAX;
    if (_utime64("owned", &explicit64) != 0) return 15;
    explicit64.modtime = WCRT_UTIME64_MAX + 1;
    open_calls = 0;
    if (_utime64("owned", &explicit64) != -1 || errno != EINVAL ||
        open_calls != 0) return 16;
    explicit64.actime = -1;
    explicit64.modtime = 0;
    if (_utime64("owned", &explicit64) != -1 || errno != EINVAL ||
        open_calls != 0) return 17;
    open_failure = 1;
    native_error = ERROR_TOO_MANY_OPEN_FILES;
    if (_utime32("owned", NULL) != -1 || errno != EMFILE) return 11;
    open_failure = 0;
    write_failure = 1;
    native_error = ERROR_ACCESS_DENIED;
    close_calls = 0;
    if (_utime64("owned", NULL) != -1 || errno != EACCES ||
        close_calls != 1) return 12;
    write_failure = 0;
    explicit64.actime = 0;
    explicit64.modtime = WCRT_UTIME64_MAX;
    errno = EDOM;
    if (_utime64("owned", &explicit64) != 0 || errno != EDOM ||
        written_access.dwLowDateTime != (DWORD)116444736000000000ULL ||
        written_access.dwHighDateTime !=
            (DWORD)(116444736000000000ULL >> 32) ||
        written.dwLowDateTime != (DWORD)441796895990000000ULL ||
        written.dwHighDateTime !=
            (DWORD)(441796895990000000ULL >> 32)) return 18;
    explicit32.actime = 0;
    explicit32.modtime = -1;
    open_calls = write_calls = close_calls = 0;
    if (_utime32("owned", &explicit32) != -1 || errno != EINVAL ||
        open_calls || write_calls || close_calls) return 19;
    explicit64.actime = 0;
    explicit64.modtime = -1;
    if (_utime64("owned", &explicit64) != -1 || errno != EINVAL ||
        open_calls || write_calls || close_calls) return 20;
    attribute_failure = 1;
    native_error = ERROR_PATH_NOT_FOUND;
    if (_utime64("missing", &explicit64) != -1 || errno != ENOENT ||
        open_calls || write_calls || close_calls) return 21;
    attribute_failure = 0;
    open_failure = 1;
    native_error = ERROR_FILE_NOT_FOUND;
    if (_utime64("owned", NULL) != -1 || errno != ENOENT ||
        open_calls != 1 || write_calls || close_calls) return 22;
    native_error = ERROR_ACCESS_DENIED;
    if (_utime32("owned", NULL) != -1 || errno != EACCES ||
        open_calls != 2 || write_calls || close_calls) return 23;
    open_failure = 0;
    write_failure = 1;
    native_error = ERROR_PATH_NOT_FOUND;
    if (_utime64("owned", NULL) != -1 || errno != ENOENT ||
        write_calls != 1 || close_calls != 1 ||
        native_error != ERROR_INVALID_HANDLE) return 24;
    if (provider_contract_error) return 25;
    open_calls = write_calls = close_calls = clock_calls = 0;
    write_failure = 0;
    explicit32.actime = 0;
    explicit32.modtime = 2147483647L;
    errno = EDOM;
    if (_futime32(7, &explicit32) != 0 || errno != EDOM ||
        open_calls || close_calls || clock_calls || write_calls != 1 ||
        written_access.dwLowDateTime != (DWORD)116444736000000000ULL ||
        written.dwLowDateTime != (DWORD)137919572470000000ULL ||
        written.dwHighDateTime !=
            (DWORD)(137919572470000000ULL >> 32)) return 26;
    write_calls = 0;
    clock_ticks = 137919572480000000ULL;
    if (_futime32(7, NULL) != -1 || errno != EINVAL ||
        open_calls || write_calls || close_calls || clock_calls != 1)
        return 27;
    clock_ticks = 441796896000000000ULL;
    if (_futime64(7, NULL) != -1 || errno != EINVAL || write_calls) return 28;
    clock_ticks = 137919572479999999ULL;
    if (_futime32(7, NULL) != 0 ||
        written.dwLowDateTime != (DWORD)clock_ticks ||
        memcmp(&written, &written_access, sizeof(written))) return 29;
    write_calls = clock_calls = 0;
    descriptor_type = FILE_TYPE_PIPE;
    if (_futime32(7, NULL) != -1 || errno != EINVAL ||
        write_calls || clock_calls) return 30;
    descriptor_type = FILE_TYPE_CHAR;
    if (_futime64(7, NULL) != -1 || errno != EINVAL ||
        write_calls || clock_calls) return 31;
    descriptor_type = FILE_TYPE_UNKNOWN;
    if (_futime64(7, NULL) != -1 || errno != EBADF ||
        write_calls || clock_calls) return 32;
    descriptor_type = FILE_TYPE_DISK;
    descriptor_failure = 1;
    if (_futime32(7, &explicit32) != -1 || errno != EBADF ||
        write_calls || clock_calls) return 33;
    descriptor_failure = 0;
    explicit64.actime = 32535216000LL;
    explicit64.modtime = 0;
    if (_futime64(7, &explicit64) != -1 || errno != EINVAL ||
        write_calls || clock_calls) return 34;
    explicit64.actime = 0;
    explicit64.modtime = 32535215999LL;
    write_failure = 1;
    native_error = ERROR_ACCESS_DENIED;
    if (_futime64(7, &explicit64) != -1 || errno != EACCES ||
        open_calls || close_calls || write_calls != 1 ||
        provider_contract_error) return 35;
    write_failure = 0;
    write_calls = clock_calls = 0;
    descriptor_directory = 1;
    if (_futime64(7, NULL) != -1 || errno != EINVAL ||
        open_calls || close_calls || write_calls || clock_calls) return 36;
    descriptor_directory = 0;
    descriptor_status_failure = 1;
    if (_futime32(7, NULL) != -1 || errno != EACCES ||
        open_calls || close_calls || write_calls || clock_calls) return 37;
    return 0;
}
