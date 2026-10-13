/** @file find32-errors.c @brief Tests enumeration boundary providers. */
#include <io.h>
#include <errno.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
HANDLE WINAPI fixture_first(LPCSTR name, LPWIN32_FIND_DATAA result);
BOOL WINAPI fixture_next(HANDLE handle, LPWIN32_FIND_DATAA result);
BOOL WINAPI fixture_close(HANDLE handle);
DWORD WINAPI fixture_error(void);
void *fixture_allocate(size_t size);
void fixture_free(void *pointer);
#define FindFirstFileA fixture_first
#define FindNextFileA fixture_next
#define FindClose fixture_close
#define GetLastError fixture_error
#define malloc fixture_allocate
#define free fixture_free
#include "../../src/ms_find32.c"

static WIN32_FIND_DATAA supplied;
static DWORD error_value;
static int first_calls, next_calls, close_calls, fail_first, fail_next;
static int fail_close;
static int fail_allocate, freed;
static struct wcrt_search allocated;
static HANDLE supplied_handle;

/** @brief Supplies one controlled ownership entry or allocation failure. */
void *fixture_allocate(size_t size)
{
    if (size != sizeof(allocated) || fail_allocate) return NULL;
    return &allocated;
}
/** @brief Verifies ownership entries are freed after successful close only. */
void fixture_free(void *pointer)
{
    if (pointer == &allocated) ++freed;
}

/** @brief Copies the provider record or returns a selected native failure. */
HANDLE WINAPI fixture_first(LPCSTR name, LPWIN32_FIND_DATAA result)
{
    (void)name;
    ++first_calls;
    *result = supplied;
    return fail_first ? INVALID_HANDLE_VALUE : supplied_handle;
}
/** @brief Advances without silently closing the provider search. */
BOOL WINAPI fixture_next(HANDLE handle, LPWIN32_FIND_DATAA result)
{
    if (handle != supplied_handle) return FALSE;
    ++next_calls;
    *result = supplied;
    return !fail_next;
}
/** @brief Overwrites errno to prove cleanup error preservation. */
BOOL WINAPI fixture_close(HANDLE handle)
{
    (void)handle;
    ++close_calls;
    errno = 999;
    return !fail_close;
}
/** @brief Returns the controlled provider error. */
DWORD WINAPI fixture_error(void) { return error_value; }

/** @brief Produces a FILETIME for a representable signed Unix second. */
static FILETIME timestamp(long long seconds)
{
    unsigned long long ticks = (seconds + 11644473600LL) * 10000000ULL;
    FILETIME result;
    result.dwLowDateTime = (DWORD)ticks;
    result.dwHighDateTime = (DWORD)(ticks >> 32);
    return result;
}

/** @brief Verifies narrowing, handle widths, attributes and cleanup. */
int main(void)
{
    struct guarded {
        unsigned long before;
        struct wcrt_finddata32 result;
        unsigned long after;
    } value;
    __wcrt_intptr_t handle;
    int closed;
    memset(&supplied, 0, sizeof(supplied));
    strcpy(supplied.cFileName, "sample");
    supplied.nFileSizeLow = 0xffffffffUL;
    supplied.ftLastWriteTime = timestamp(2147483647LL);
    supplied.ftLastAccessTime = timestamp(-2147483647LL - 1);
    supplied.dwFileAttributes = 0xffffffffUL;
    supplied_handle = (HANDLE)(__wcrt_intptr_t)(1ULL <<
        (sizeof(__wcrt_intptr_t) * 8 - 1));
    value.before = 0x12345678UL;
    value.after = 0xabcdef01UL;
    errno = 123;
    handle = wcrt_findfirst32("*", &value.result);
    if (handle != (__wcrt_intptr_t)supplied_handle || errno != 123 ||
        value.result.size != 0xffffffffUL || value.result.attrib != 55 ||
        value.result.time_create != -1 ||
        value.result.time_access != (-2147483647L - 1) ||
        value.result.time_write != 2147483647L) return 1;
    supplied.nFileSizeHigh = 1;
    if (wcrt_findfirst32("*", &value.result) != -1 || errno != ERANGE ||
        close_calls != 1 || value.result.size || value.result.name[0]) return 2;
    closed = close_calls;
    if (wcrt_findnext32(handle, &value.result) != -1 || errno != ERANGE ||
        close_calls != closed) return 3;
    supplied.nFileSizeHigh = 0;
    supplied.ftLastWriteTime = timestamp(2147483648LL);
    if (wcrt_findnext32(handle, &value.result) != -1 || errno != ERANGE)
        return 4;
    supplied.ftLastWriteTime = timestamp(0);
    supplied.ftLastAccessTime = timestamp(-2147483649LL);
    if (wcrt_findnext32(handle, &value.result) != -1 || errno != ERANGE)
        return 13;
    supplied.ftLastAccessTime = timestamp(0);
    memset(supplied.cFileName, 'a', sizeof(supplied.cFileName));
    if (wcrt_findnext32(handle, &value.result) != -1 || errno != EINVAL)
        return 5;
    fail_next = 1;
    error_value = ERROR_NO_MORE_FILES;
    if (wcrt_findnext32(handle, &value.result) != -1 || errno != ENOENT ||
        close_calls != closed) return 6;
    fail_first = 1;
    error_value = ERROR_NOT_ENOUGH_MEMORY;
    if (wcrt_findfirst32("*", &value.result) != -1 || errno != ENOMEM)
        return 7;
    error_value = ERROR_INVALID_HANDLE;
    if (wcrt_findfirst32("*", &value.result) != -1 || errno != EINVAL)
        return 8;
    closed = first_calls;
    if (wcrt_findfirst32(NULL, &value.result) != -1 || errno != EINVAL ||
        wcrt_findfirst32("", &value.result) != -1 ||
        wcrt_findfirst32("*", NULL) != -1 || first_calls != closed) return 9;
    closed = next_calls;
    if (wcrt_findnext32(0, &value.result) != -1 ||
        wcrt_findnext32(-1, &value.result) != -1 ||
        wcrt_findnext32(handle, NULL) != -1 || next_calls != closed) return 10;
    fail_close = 1;
    if (wcrt_findclose(handle) != -1 || errno != EINVAL || freed)
        return 14;
    fail_first = 0;
    supplied.nFileSizeHigh = 1;
    if (wcrt_findfirst32("*", &value.result) != -1 || errno != ERANGE)
        return 15;
    fail_close = 0;
    errno = 123;
    if (wcrt_findclose(handle) || errno != 123 || freed != 1 ||
        wcrt_findclose(-1) != -1 || errno != EINVAL) return 11;
    closed = close_calls;
    if (wcrt_findclose(handle) != -1 || errno != EINVAL ||
        close_calls != closed) return 16;
    closed = next_calls;
    if (wcrt_findnext32(handle, &value.result) != -1 || errno != EINVAL ||
        next_calls != closed) return 17;
    supplied.nFileSizeHigh = 0;
    strcpy(supplied.cFileName, "valid");
    fail_allocate = 1;
    closed = close_calls;
    if (wcrt_findfirst32("*", &value.result) != -1 || errno != ENOMEM ||
        close_calls != closed + 1 || value.result.size || value.result.name[0])
        return 18;
    if (value.before != 0x12345678UL || value.after != 0xabcdef01UL)
        return 12;
    return 0;
}
