/**
 * @file wchar_time.c
 * @brief Implements C-locale wide time formatting.
 */

#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

size_t wcsftime(wchar_t *destination, size_t capacity,
    const wchar_t *format, const struct tm *broken_down)
{
    size_t format_length;
    size_t index;
    size_t result;
    char *narrow_format;
    char *narrow_result;

    if (capacity == 0 || destination == (wchar_t *)0 ||
        format == (const wchar_t *)0 || broken_down == (const struct tm *)0) {
        return 0;
    }
    format_length = wcslen(format);
    if (format_length == (size_t)-1) {
        errno = ERANGE;
        return 0;
    }
    narrow_format = (char *)malloc(format_length + 1);
    narrow_result = (char *)malloc(capacity);
    if (narrow_format == (char *)0 || narrow_result == (char *)0) {
        free(narrow_format);
        free(narrow_result);
        errno = ENOMEM;
        return 0;
    }
    for (index = 0; index < format_length; ++index) {
        if ((wint_t)format[index] > 0xffU) {
            free(narrow_format);
            free(narrow_result);
            errno = EILSEQ;
            return 0;
        }
        narrow_format[index] = (char)format[index];
    }
    narrow_format[format_length] = '\0';
    narrow_result[0] = 1;
    result = strftime(narrow_result, capacity, narrow_format, broken_down);
    if (result != 0 || narrow_result[0] == '\0') {
        for (index = 0; index <= result; ++index) {
            destination[index] = (wchar_t)(unsigned char)narrow_result[index];
        }
    }
    free(narrow_format);
    free(narrow_result);
    return result;
}
