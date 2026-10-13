/** @file ms_find32.c @brief Implements narrow ANSI filename enumeration. */
#include <io.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

/** @brief Tracks only searches acquired by this runtime. */
struct wcrt_search {
    HANDLE handle;
    struct wcrt_search *next;
};
static struct wcrt_search *wcrt_searches;

/** @brief Validates ownership before passing opaque handles to Windows. */
static struct wcrt_search **wcrt_search_slot(__wcrt_intptr_t handle)
{
    struct wcrt_search **slot = &wcrt_searches;
    while (*slot && (__wcrt_intptr_t)(*slot)->handle != handle)
        slot = &(*slot)->next;
    return slot;
}

/** @brief Maps the selected filename-search error contract. */
static int wcrt_find_error(DWORD error)
{
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND ||
        error == ERROR_NO_MORE_FILES) return ENOENT;
    if (error == ERROR_NOT_ENOUGH_MEMORY || error == ERROR_OUTOFMEMORY)
        return ENOMEM;
    return EINVAL;
}

/** @brief Converts FILETIME without unsigned pre-epoch subtraction. */
static int wcrt_find_time(const FILETIME *value, long *result)
{
    unsigned long long ticks = ((unsigned long long)value->dwHighDateTime
        << 32) | value->dwLowDateTime;
    long long seconds;
    if (!ticks) { *result = -1; return 0; }
    seconds = (long long)(ticks / 10000000ULL) - 11644473600LL;
    if (seconds < -2147483647LL - 1 || seconds > 2147483647LL) {
        errno = ERANGE;
        return -1;
    }
    *result = (long)seconds;
    return 0;
}

/** @brief Commits a complete narrow record or leaves output unchanged. */
static int wcrt_find_record(const WIN32_FIND_DATAA *native, void *output)
{
    struct wcrt_finddata32 *result = (struct wcrt_finddata32 *)output;
    struct wcrt_finddata32 value;
    unsigned int length;
    memset(&value, 0, sizeof(value));
    if (native->nFileSizeHigh) { errno = ERANGE; return -1; }
    if (wcrt_find_time(&native->ftCreationTime, &value.time_create) ||
        wcrt_find_time(&native->ftLastAccessTime, &value.time_access) ||
        wcrt_find_time(&native->ftLastWriteTime, &value.time_write)) return -1;
    for (length = 0; length < sizeof(value.name); ++length)
        if (!native->cFileName[length]) break;
    if (length == sizeof(value.name)) { errno = EINVAL; return -1; }
    value.attrib = native->dwFileAttributes &
        (_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_SUBDIR | _A_ARCH);
    value.size = native->nFileSizeLow;
    memcpy(value.name, native->cFileName, length + 1);
    *result = value;
    return 0;
}

/** @brief Reuses narrow metadata checks without narrowing the file size. */
static int wcrt_find_wide_record(const WIN32_FIND_DATAA *native, void *output)
{
    WIN32_FIND_DATAA metadata = *native;
    struct wcrt_finddata32 narrow;
    struct wcrt_finddata32i64 value;
    if (native->nFileSizeHigh & 0x80000000UL) {
        errno = ERANGE;
        return -1;
    }
    metadata.nFileSizeHigh = 0;
    if (wcrt_find_record(&metadata, &narrow)) return -1;
    memset(&value, 0, sizeof(value));
    value.attrib = narrow.attrib;
    value.time_create = narrow.time_create;
    value.time_access = narrow.time_access;
    value.time_write = narrow.time_write;
    value.size = (wcrt_ms_int64)(((wcrt_ms_uint64)native->nFileSizeHigh << 32) |
        native->nFileSizeLow);
    memcpy(value.name, narrow.name, sizeof(value.name));
    memcpy(output, &value, sizeof(value));
    return 0;
}

/** @brief Acquires a native search and cleans up a failed first conversion. */
static __wcrt_intptr_t wcrt_start_search(const char *specification,
    void *result, size_t size,
    int (*convert)(const WIN32_FIND_DATAA *, void *))
{
    WIN32_FIND_DATAA native;
    HANDLE handle;
    struct wcrt_search *search;
    int saved = errno;
    if (!result || !specification || !*specification) {
        if (result) memset(result, 0, size);
        errno = EINVAL;
        return -1;
    }
    handle = FindFirstFileA(specification, &native);
    memset(result, 0, size);
    if (handle == INVALID_HANDLE_VALUE) {
        errno = wcrt_find_error(GetLastError());
        return -1;
    }
    if (convert(&native, result)) {
        int failure = errno;
        FindClose(handle);
        errno = failure;
        return -1;
    }
    search = (struct wcrt_search *)malloc(sizeof(*search));
    if (!search) {
        FindClose(handle);
        memset(result, 0, size);
        errno = ENOMEM;
        return -1;
    }
    search->handle = handle;
    search->next = wcrt_searches;
    wcrt_searches = search;
    errno = saved;
    return (__wcrt_intptr_t)handle;
}

/** @brief Advances a caller-owned search without closing the handle. */
static int wcrt_next_search(__wcrt_intptr_t handle, void *result,
    int (*convert)(const WIN32_FIND_DATAA *, void *))
{
    WIN32_FIND_DATAA native;
    int saved = errno;
    if (!result || !handle || handle == -1 || !*wcrt_search_slot(handle)) {
        errno = EINVAL;
        return -1;
    }
    if (!FindNextFileA((HANDLE)handle, &native)) {
        errno = wcrt_find_error(GetLastError());
        return -1;
    }
    if (convert(&native, result)) return -1;
    errno = saved;
    return 0;
}

/** @brief Starts a search producing unsigned 32-bit file sizes. */
__wcrt_intptr_t __cdecl wcrt_findfirst32(const char *specification,
    struct wcrt_finddata32 *result)
{
    return wcrt_start_search(specification, result, sizeof(*result),
        wcrt_find_record);
}
/** @brief Advances with narrow records, committing only on success. */
int __cdecl wcrt_findnext32(__wcrt_intptr_t handle,
    struct wcrt_finddata32 *result)
{
    return wcrt_next_search(handle, result, wcrt_find_record);
}
/** @brief Starts a search producing signed 64-bit file sizes. */
__wcrt_intptr_t __cdecl wcrt_findfirst32i64(const char *specification,
    struct wcrt_finddata32i64 *result)
{
    return wcrt_start_search(specification, result, sizeof(*result),
        wcrt_find_wide_record);
}
/** @brief Advances with wide-size records, committing only on success. */
int __cdecl wcrt_findnext32i64(__wcrt_intptr_t handle,
    struct wcrt_finddata32i64 *result)
{
    return wcrt_next_search(handle, result, wcrt_find_wide_record);
}

/** @brief Releases a caller-owned search, preserving errno on success. */
int __cdecl wcrt_findclose(__wcrt_intptr_t handle)
{
    int saved = errno;
    struct wcrt_search **slot;
    struct wcrt_search *search;
    if (!handle || handle == -1) {
        errno = EINVAL;
        return -1;
    }
    slot = wcrt_search_slot(handle);
    search = *slot;
    if (!search || !FindClose(search->handle)) {
        errno = EINVAL;
        return -1;
    }
    *slot = search->next;
    free(search);
    errno = saved;
    return 0;
}
