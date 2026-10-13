/** @file calendar-time32.c @brief Exercises explicit narrow calendar APIs. */
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>

/** @brief Protects the exact four-byte timestamp passed across the ABI. */
struct calendar_guard {
    unsigned char before[8]; /**< Leading guard. */
    long value; /**< Narrow timestamp. */
    unsigned char after[8]; /**< Trailing guard. */
};

/** @brief Checks that neither neighboring guard was modified. */
static int calendar_guards(const struct calendar_guard *input)
{
    int index;
    for (index = 0; index < 8; ++index) {
        if (input->before[index] != 0xa5 || input->after[index] != 0xa5)
            return 0;
    }
    return 1;
}

/** @brief Checks storage, range, normalization and local epoch behavior. */
int check_calendar32(void)
{
    struct calendar_guard input;
    struct calendar_guard saved;
    struct tm *shared;
    struct tm *next;
    struct tm copy;
    struct tm original;
    char expected[26];
    long now;
    memset(&input, 0xa5, sizeof(input));
    input.value = 0;
    errno = EDOM;
    shared = wcrt_gmtime32(&input.value);
    if (shared == NULL || shared->tm_year != 70 || shared->tm_mon != 0 ||
        shared->tm_mday != 1 || shared->tm_wday != 4 || shared->tm_yday != 0 ||
        shared->tm_isdst != -1 || errno != EDOM || !calendar_guards(&input))
        return 1;
    input.value = LONG_MAX;
    next = wcrt_gmtime32(&input.value);
    if (next != shared || shared->tm_year != 138 || shared->tm_mon != 0 ||
        shared->tm_mday != 19 || shared->tm_hour != 3 || shared->tm_min != 14 ||
        shared->tm_sec != 7 || !calendar_guards(&input)) return 2;
    if (wcrt_difftime32(LONG_MAX, LONG_MIN) != 4294967295.0 ||
        wcrt_difftime32(LONG_MIN, LONG_MAX) != -4294967295.0 ||
        wcrt_difftime32(0, 0) != 0 || errno != EDOM) return 3;
    input.value = -1;
    saved = input;
    if (wcrt_gmtime32(&input.value) != NULL || errno != EINVAL ||
        wcrt_localtime32(&input.value) != NULL || errno != EINVAL ||
        wcrt_ctime32(&input.value) != NULL || errno != EINVAL ||
        memcmp(&saved, &input, sizeof(input))) return 4;
    if (wcrt_gmtime32(NULL) != NULL || errno != EINVAL ||
        wcrt_localtime32(NULL) != NULL || errno != EINVAL ||
        wcrt_ctime32(NULL) != NULL || errno != EINVAL ||
        wcrt_mktime32(NULL) != -1 || errno != EINVAL) return 5;
    input.value = 0;
    errno = EDOM;
    shared = wcrt_localtime32(&input.value);
    if (shared == NULL || shared->tm_year < 69 || shared->tm_year > 70 ||
        errno != EDOM) return 6;
    copy = *shared;
    memcpy(expected, asctime(&copy), sizeof(expected));
    if (wcrt_mktime32(&copy) != 0 || errno != EDOM ||
        strcmp(wcrt_ctime32(&input.value), expected)) return 7;
    input.value = LONG_MAX;
    shared = wcrt_localtime32(&input.value);
    if (shared == NULL) return 8;
    copy = *shared;
    if (wcrt_mktime32(&copy) != LONG_MAX || errno != EDOM) return 9;
    memset(&copy, 0, sizeof(copy));
    copy.tm_year = 100;
    copy.tm_mday = 32;
    copy.tm_hour = 12;
    copy.tm_isdst = -1;
    if (wcrt_mktime32(&copy) <= 0 || copy.tm_year != 100 ||
        copy.tm_mon != 1 || copy.tm_mday != 1 || copy.tm_wday != 2) return 10;
    copy.tm_year = 139;
    original = copy;
    if (wcrt_mktime32(&copy) != -1 || errno != EINVAL ||
        memcmp(&copy, &original, sizeof(copy))) return 11;
    copy.tm_year = INT_MAX;
    copy.tm_mon = INT_MIN;
    original = copy;
    if (wcrt_mktime32(&copy) != -1 || errno != EINVAL ||
        memcmp(&copy, &original, sizeof(copy))) return 12;
    errno = EDOM;
    now = wcrt_time32(&input.value);
    if (now < 0 || input.value != now || errno != EDOM ||
        !calendar_guards(&input) || wcrt_time32(NULL) < now) return 13;
    return 0;
}
