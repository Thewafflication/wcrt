/**
 * @file wchar_string.c
 * @brief Implements wide strings, memory, and numeric conversions.
 */

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>

size_t wcslen(const wchar_t *string)
{
    const wchar_t *current = string;
    while (*current != 0) ++current;
    return (size_t)(current - string);
}

wchar_t *wcscpy(wchar_t *destination, const wchar_t *source)
{
    wchar_t *result = destination;
    while ((*destination++ = *source++) != 0) { }
    return result;
}

wchar_t *wcsncpy(wchar_t *destination, const wchar_t *source, size_t count)
{
    wchar_t *result = destination;
    while (count != 0 && *source != 0) {
        *destination++ = *source++;
        --count;
    }
    while (count-- != 0) *destination++ = 0;
    return result;
}

wchar_t *wcscat(wchar_t *destination, const wchar_t *source)
{
    wchar_t *result = destination;
    while (*destination != 0) ++destination;
    wcscpy(destination, source);
    return result;
}

wchar_t *wcsncat(wchar_t *destination, const wchar_t *source, size_t count)
{
    wchar_t *result = destination;
    while (*destination != 0) ++destination;
    while (count-- != 0 && *source != 0) *destination++ = *source++;
    *destination = 0;
    return result;
}

int wcscmp(const wchar_t *left, const wchar_t *right)
{
    while (*left == *right && *left != 0) {
        ++left;
        ++right;
    }
    return *left < *right ? -1 : (*left > *right ? 1 : 0);
}

int wcsncmp(const wchar_t *left, const wchar_t *right, size_t count)
{
    while (count != 0 && *left == *right && *left != 0) {
        ++left;
        ++right;
        --count;
    }
    if (count == 0) return 0;
    return *left < *right ? -1 : (*left > *right ? 1 : 0);
}

int wcscoll(const wchar_t *left, const wchar_t *right)
{
    return wcscmp(left, right);
}

size_t wcsxfrm(wchar_t *destination, const wchar_t *source, size_t count)
{
    size_t length = wcslen(source);
    size_t copied = 0;

    while (copied < count && copied <= length) {
        destination[copied] = source[copied];
        ++copied;
    }
    return length;
}

wchar_t *wcschr(const wchar_t *string, wchar_t character)
{
    do {
        if (*string == character) return (wchar_t *)string;
    } while (*string++ != 0);
    return (wchar_t *)0;
}

wchar_t *wcsrchr(const wchar_t *string, wchar_t character)
{
    const wchar_t *result = (const wchar_t *)0;
    do {
        if (*string == character) result = string;
    } while (*string++ != 0);
    return (wchar_t *)result;
}

size_t wcsspn(const wchar_t *string, const wchar_t *accept)
{
    size_t count = 0;
    while (string[count] != 0 && wcschr(accept, string[count]) != 0) ++count;
    return count;
}

size_t wcscspn(const wchar_t *string, const wchar_t *reject)
{
    size_t count = 0;
    while (string[count] != 0 && wcschr(reject, string[count]) == 0) ++count;
    return count;
}

wchar_t *wcspbrk(const wchar_t *string, const wchar_t *accept)
{
    while (*string != 0) {
        if (wcschr(accept, *string) != 0) return (wchar_t *)string;
        ++string;
    }
    return (wchar_t *)0;
}

wchar_t *wcsstr(const wchar_t *haystack, const wchar_t *needle)
{
    size_t length = wcslen(needle);
    if (length == 0) return (wchar_t *)haystack;
    while (*haystack != 0) {
        if (wcsncmp(haystack, needle, length) == 0) return (wchar_t *)haystack;
        ++haystack;
    }
    return (wchar_t *)0;
}

wchar_t *wcstok(wchar_t *string, const wchar_t *separators, wchar_t **state)
{
    wchar_t *start;

    if (string == (wchar_t *)0) string = *state;
    string += wcsspn(string, separators);
    if (*string == 0) {
        *state = string;
        return (wchar_t *)0;
    }
    start = string;
    string += wcscspn(string, separators);
    if (*string != 0) *string++ = 0;
    *state = string;
    return start;
}

wchar_t *wmemchr(const wchar_t *memory, wchar_t character, size_t count)
{
    while (count-- != 0) {
        if (*memory == character) return (wchar_t *)memory;
        ++memory;
    }
    return (wchar_t *)0;
}

int wmemcmp(const wchar_t *left, const wchar_t *right, size_t count)
{
    while (count-- != 0) {
        if (*left != *right) return *left < *right ? -1 : 1;
        ++left;
        ++right;
    }
    return 0;
}

wchar_t *wmemcpy(wchar_t *destination, const wchar_t *source, size_t count)
{
    wchar_t *result = destination;
    while (count-- != 0) *destination++ = *source++;
    return result;
}

wchar_t *wmemmove(wchar_t *destination, const wchar_t *source, size_t count)
{
    wchar_t *result = destination;
    if ((uintptr_t)destination < (uintptr_t)source) {
        while (count-- != 0) *destination++ = *source++;
    } else if ((uintptr_t)destination > (uintptr_t)source) {
        destination += count;
        source += count;
        while (count-- != 0) *--destination = *--source;
    }
    return result;
}

wchar_t *wmemset(wchar_t *destination, wchar_t character, size_t count)
{
    wchar_t *result = destination;
    while (count-- != 0) *destination++ = character;
    return result;
}

static char *wcrt_narrow_wide_number(const wchar_t *string, size_t *length)
{
    char *narrow;
    size_t index;

    *length = wcslen(string);
    if (*length == (size_t)-1) {
        errno = ERANGE;
        return (char *)0;
    }
    narrow = (char *)malloc(*length + 1);
    if (narrow == (char *)0) {
        errno = ENOMEM;
        return (char *)0;
    }
    for (index = 0; index < *length; ++index) {
        narrow[index] = string[index] <= 0xff ? (char)string[index] : 0x7f;
    }
    narrow[*length] = '\0';
    return narrow;
}

#define WCRT_WIDE_NUMBER(name, narrow_name, type) \
type name(const wchar_t *string, wchar_t **end_pointer, int base) \
{ \
    size_t length; \
    char *narrow = wcrt_narrow_wide_number(string, &length); \
    char *end; \
    type result; \
    (void)length; \
    if (narrow == (char *)0) { \
        if (end_pointer != (wchar_t **)0) *end_pointer = (wchar_t *)string; \
        return (type)0; \
    } \
    result = narrow_name(narrow, &end, base); \
    if (end_pointer != (wchar_t **)0) *end_pointer = \
        (wchar_t *)string + (end - narrow); \
    free(narrow); \
    return result; \
}

WCRT_WIDE_NUMBER(wcstol, strtol, long)
WCRT_WIDE_NUMBER(wcstoul, strtoul, unsigned long)
WCRT_WIDE_NUMBER(wcstoll, strtoll, long long)
WCRT_WIDE_NUMBER(wcstoull, strtoull, unsigned long long)
WCRT_WIDE_NUMBER(wcstoimax, strtoimax, intmax_t)
WCRT_WIDE_NUMBER(wcstoumax, strtoumax, uintmax_t)

#define WCRT_WIDE_FLOAT(name, narrow_name, type) \
type name(const wchar_t *string, wchar_t **end_pointer) \
{ \
    size_t length; \
    char *narrow = wcrt_narrow_wide_number(string, &length); \
    char *end; \
    type result; \
    (void)length; \
    if (narrow == (char *)0) { \
        if (end_pointer != (wchar_t **)0) *end_pointer = (wchar_t *)string; \
        return (type)0; \
    } \
    result = narrow_name(narrow, &end); \
    if (end_pointer != (wchar_t **)0) *end_pointer = \
        (wchar_t *)string + (end - narrow); \
    free(narrow); \
    return result; \
}

WCRT_WIDE_FLOAT(wcstod, strtod, double)
WCRT_WIDE_FLOAT(wcstof, strtof, float)
WCRT_WIDE_FLOAT(wcstold, strtold, long double)
