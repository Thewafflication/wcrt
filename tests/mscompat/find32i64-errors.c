/** @file find32i64-errors.c @brief Tests wide-size enumeration providers. */
#define main check_narrow_search_providers
#include "find32-errors.c"
#undef main

/** @brief Verifies wide-size boundaries and complete failed-next retention. */
int main(void)
{
    struct guarded_wide {
        unsigned long before;
        struct wcrt_finddata32i64 data;
        unsigned long after;
    } value;
    struct wcrt_finddata32i64 saved, zero;
    struct wcrt_finddata32 narrow, saved_narrow;
    __wcrt_intptr_t handle;
    int previous_calls;
    if (check_narrow_search_providers()) return 1;
    fail_allocate = fail_first = fail_next = fail_close = 0;
    supplied.nFileSizeHigh = 1;
    supplied.nFileSizeLow = 0;
    value.before = 0x12345678UL;
    value.after = 0xabcdef01UL;
    errno = 123;
    handle = wcrt_findfirst32i64("*", &value.data);
    if (handle != (__wcrt_intptr_t)supplied_handle || errno != 123 ||
        value.data.size != 4294967296LL) return 2;
    memset(&narrow, 0xa5, sizeof(narrow));
    saved_narrow = narrow;
    if (wcrt_findnext32(handle, &narrow) != -1 || errno != ERANGE ||
        memcmp(&narrow, &saved_narrow, sizeof(narrow))) return 3;
    supplied.nFileSizeHigh = 0x7fffffffUL;
    supplied.nFileSizeLow = 0xffffffffUL;
    errno = 123;
    if (wcrt_findnext32i64(handle, &value.data) || errno != 123 ||
        value.data.size != 9223372036854775807LL) return 4;
    saved = value.data;
    supplied.nFileSizeHigh = 0x80000000UL;
    if (wcrt_findnext32i64(handle, &value.data) != -1 || errno != ERANGE ||
        memcmp(&value.data, &saved, sizeof(saved))) return 5;
    fail_next = 1;
    error_value = ERROR_NO_MORE_FILES;
    if (wcrt_findnext32i64(handle, &value.data) != -1 || errno != ENOENT ||
        memcmp(&value.data, &saved, sizeof(saved)) || wcrt_findclose(handle))
        return 6;
    previous_calls = next_calls;
    if (wcrt_findnext32i64(handle, &value.data) != -1 || errno != EINVAL ||
        next_calls != previous_calls ||
        memcmp(&value.data, &saved, sizeof(saved))) return 7;
    memset(&zero, 0, sizeof(zero));
    previous_calls = close_calls;
    if (wcrt_findfirst32i64("*", &value.data) != -1 || errno != ERANGE ||
        close_calls != previous_calls + 1 ||
        memcmp(&value.data, &zero, sizeof(zero))) return 8;
    supplied.nFileSizeHigh = 1;
    fail_allocate = 1;
    previous_calls = close_calls;
    if (wcrt_findfirst32i64("*", &value.data) != -1 || errno != ENOMEM ||
        close_calls != previous_calls + 1 ||
        memcmp(&value.data, &zero, sizeof(zero))) return 9;
    if (value.before != 0x12345678UL || value.after != 0xabcdef01UL)
        return 10;
    return 0;
}
