/**
 * @file wchar_scan.c
 * @brief Implements C99 wide formatted input for the C-locale encoding.
 */

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

#include "internal/file.h"

static int wcrt_wscan_byte(wchar_t character, char *result)
{
    if ((wint_t)character > 0xffU) return -1;
    *result = (char)(unsigned char)character;
    return 0;
}

static int wcrt_wscan_format(const wchar_t *wide, char **narrow_result)
{
    size_t length = wcslen(wide);
    size_t index;
    char *narrow;

    if (length == (size_t)-1) return -1;
    narrow = (char *)malloc(length + 1U);
    if (narrow == (char *)0) return -1;
    for (index = 0; index < length; ++index) {
        if (wcrt_wscan_byte(wide[index], &narrow[index]) != 0) {
            free(narrow);
            errno = EILSEQ;
            return -1;
        }
    }
    narrow[length] = '\0';
    *narrow_result = narrow;
    return 0;
}

enum wcrt_wscan_length {
    WCRT_WSCAN_NONE,
    WCRT_WSCAN_HH,
    WCRT_WSCAN_H,
    WCRT_WSCAN_L,
    WCRT_WSCAN_LL,
    WCRT_WSCAN_J,
    WCRT_WSCAN_Z,
    WCRT_WSCAN_T,
    WCRT_WSCAN_CAPITAL_L
};

static int wcrt_wscan_width(const wchar_t **format)
{
    int result = 0;
    while (**format >= L'0' && **format <= L'9') {
        int digit = *(*format)++ - L'0';
        if (result > (INT_MAX - digit) / 10) result = INT_MAX;
        else result = result * 10 + digit;
    }
    return result;
}

static void wcrt_wscan_store_signed(va_list *arguments,
    enum wcrt_wscan_length length, long long value)
{
    if (length == WCRT_WSCAN_HH) *va_arg(*arguments, signed char *) =
        (signed char)value;
    else if (length == WCRT_WSCAN_H) *va_arg(*arguments, short *) =
        (short)value;
    else if (length == WCRT_WSCAN_L) *va_arg(*arguments, long *) =
        (long)value;
    else if (length == WCRT_WSCAN_LL) *va_arg(*arguments, long long *) = value;
    else if (length == WCRT_WSCAN_J) *va_arg(*arguments, intmax_t *) =
        (intmax_t)value;
    else if (length == WCRT_WSCAN_Z || length == WCRT_WSCAN_T) {
        *va_arg(*arguments, ptrdiff_t *) = (ptrdiff_t)value;
    } else *va_arg(*arguments, int *) = (int)value;
}

static void wcrt_wscan_store_unsigned(va_list *arguments,
    enum wcrt_wscan_length length, unsigned long long value)
{
    if (length == WCRT_WSCAN_HH) *va_arg(*arguments, unsigned char *) =
        (unsigned char)value;
    else if (length == WCRT_WSCAN_H) *va_arg(*arguments, unsigned short *) =
        (unsigned short)value;
    else if (length == WCRT_WSCAN_L) *va_arg(*arguments, unsigned long *) =
        (unsigned long)value;
    else if (length == WCRT_WSCAN_LL) {
        *va_arg(*arguments, unsigned long long *) = value;
    } else if (length == WCRT_WSCAN_J) *va_arg(*arguments, uintmax_t *) =
        (uintmax_t)value;
    else if (length == WCRT_WSCAN_Z || length == WCRT_WSCAN_T) {
        *va_arg(*arguments, size_t *) = (size_t)value;
    } else *va_arg(*arguments, unsigned int *) = (unsigned int)value;
}

static void wcrt_wscan_store_count(va_list *arguments,
    enum wcrt_wscan_length length, size_t value)
{
    if (length == WCRT_WSCAN_HH) *va_arg(*arguments, signed char *) =
        (signed char)value;
    else if (length == WCRT_WSCAN_H) *va_arg(*arguments, short *) =
        (short)value;
    else if (length == WCRT_WSCAN_L) *va_arg(*arguments, long *) =
        (long)value;
    else if (length == WCRT_WSCAN_LL) *va_arg(*arguments, long long *) =
        (long long)value;
    else if (length == WCRT_WSCAN_J) *va_arg(*arguments, intmax_t *) =
        (intmax_t)value;
    else if (length == WCRT_WSCAN_Z || length == WCRT_WSCAN_T) {
        *va_arg(*arguments, ptrdiff_t *) = (ptrdiff_t)value;
    }
    else *va_arg(*arguments, int *) = (int)value;
}

static int wcrt_wscan_take(const wchar_t **source, int *remaining)
{
    int character;
    if (*remaining == 0 || **source == 0) return -1;
    character = **source;
    ++*source;
    if (*remaining != INT_MAX) --*remaining;
    return character;
}

static int wcrt_wscan_digit(wchar_t character, int base)
{
    int digit;
    if (character >= L'0' && character <= L'9') digit = character - L'0';
    else if (character >= L'a' && character <= L'f') {
        digit = character - L'a' + 10;
    } else if (character >= L'A' && character <= L'F') {
        digit = character - L'A' + 10;
    } else return 0;
    return digit < base;
}

static const wchar_t *wcrt_wscan_integer_end(const wchar_t *source,
    int width, wchar_t conversion)
{
    const wchar_t *current = source;
    int remaining = width == 0 ? INT_MAX : width;
    int base = conversion == L'o' ? 8 :
        (conversion == L'x' || conversion == L'X' || conversion == L'p' ?
        16 : 10);
    if (remaining != 0 && (*current == L'+' || *current == L'-')) {
        (void)wcrt_wscan_take(&current, &remaining);
    }
    if (remaining != 0 && *current == L'0' &&
        (conversion == L'i' || base == 16)) {
        (void)wcrt_wscan_take(&current, &remaining);
        if (remaining != 0 && (*current == L'x' || *current == L'X')) {
            (void)wcrt_wscan_take(&current, &remaining);
            base = 16;
        } else if (conversion == L'i') base = 8;
    } else if (conversion == L'i') base = 10;
    while (remaining != 0 && wcrt_wscan_digit(*current, base)) {
        (void)wcrt_wscan_take(&current, &remaining);
    }
    return current;
}

static int wcrt_wscan_ascii(wchar_t character, int lower)
{
    if (character >= L'A' && character <= L'Z') character += L'a' - L'A';
    return character == (wchar_t)lower;
}

static void wcrt_wscan_word(const wchar_t **source, int *remaining,
    const char *word)
{
    while (*word != '\0' && *remaining != 0 &&
        wcrt_wscan_ascii(**source, *word)) {
        (void)wcrt_wscan_take(source, remaining);
        ++word;
    }
}

static const wchar_t *wcrt_wscan_float_end(const wchar_t *source, int width)
{
    const wchar_t *current = source;
    int remaining = width == 0 ? INT_MAX : width;
    int hexadecimal = 0;
    if (remaining != 0 && (*current == L'+' || *current == L'-')) {
        (void)wcrt_wscan_take(&current, &remaining);
    }
    if (remaining != 0 && wcrt_wscan_ascii(*current, 'i')) {
        wcrt_wscan_word(&current, &remaining, "infinity");
        return current;
    }
    if (remaining != 0 && wcrt_wscan_ascii(*current, 'n')) {
        wcrt_wscan_word(&current, &remaining, "nan");
        if (remaining != 0 && *current == L'(') {
            (void)wcrt_wscan_take(&current, &remaining);
            while (remaining != 0 && ((*current >= L'0' && *current <= L'9') ||
                (*current >= L'a' && *current <= L'z') ||
                (*current >= L'A' && *current <= L'Z') || *current == L'_')) {
                (void)wcrt_wscan_take(&current, &remaining);
            }
            if (remaining != 0 && *current == L')') {
                (void)wcrt_wscan_take(&current, &remaining);
            }
        }
        return current;
    }
    if (remaining != 0 && *current == L'0') {
        (void)wcrt_wscan_take(&current, &remaining);
        if (remaining != 0 && (*current == L'x' || *current == L'X')) {
            (void)wcrt_wscan_take(&current, &remaining);
            hexadecimal = 1;
        }
    }
    while (remaining != 0 && wcrt_wscan_digit(*current,
        hexadecimal ? 16 : 10)) {
        (void)wcrt_wscan_take(&current, &remaining);
    }
    if (remaining != 0 && *current == L'.') {
        (void)wcrt_wscan_take(&current, &remaining);
        while (remaining != 0 && wcrt_wscan_digit(*current,
            hexadecimal ? 16 : 10)) {
            (void)wcrt_wscan_take(&current, &remaining);
        }
    }
    if (remaining != 0 && ((hexadecimal && (*current == L'p' ||
        *current == L'P')) || (!hexadecimal && (*current == L'e' ||
        *current == L'E')))) {
        (void)wcrt_wscan_take(&current, &remaining);
        if (remaining != 0 && (*current == L'+' || *current == L'-')) {
            (void)wcrt_wscan_take(&current, &remaining);
        }
        while (remaining != 0 && *current >= L'0' && *current <= L'9') {
            (void)wcrt_wscan_take(&current, &remaining);
        }
    }
    return current;
}

static wchar_t *wcrt_wscan_item_copy(const wchar_t *start,
    const wchar_t *end)
{
    size_t length = (size_t)(end - start);
    wchar_t *item;
    if (length > ((size_t)-1 / sizeof(wchar_t)) - 1U) return (wchar_t *)0;
    item = (wchar_t *)malloc((length + 1U) * sizeof(wchar_t));
    if (item == (wchar_t *)0) return (wchar_t *)0;
    wmemcpy(item, start, length);
    item[length] = 0;
    return item;
}

static int wcrt_wscan_integer(const wchar_t **source, int suppress,
    int width, enum wcrt_wscan_length length, wchar_t conversion,
    va_list *arguments)
{
    const wchar_t *start = *source;
    const wchar_t *item_end = wcrt_wscan_integer_end(start, width, conversion);
    wchar_t *item;
    wchar_t *converted_end;
    int base = conversion == L'o' ? 8 :
        (conversion == L'x' || conversion == L'X' || conversion == L'p' ?
        16 : (conversion == L'i' ? 0 : 10));
    int success = 0;
    *source = item_end;
    if (item_end == start) return *start == 0 ? -2 : 0;
    item = wcrt_wscan_item_copy(start, item_end);
    if (item == (wchar_t *)0) return -1;
    if (conversion == L'd' || conversion == L'i') {
        long long value = wcstoll(item, &converted_end, base);
        success = converted_end == item + (item_end - start);
        if (success && !suppress) {
            wcrt_wscan_store_signed(arguments, length, value);
        }
    } else {
        unsigned long long value = wcstoull(item, &converted_end, base);
        success = converted_end == item + (item_end - start);
        if (success && !suppress) {
            if (conversion == L'p') {
                *va_arg(*arguments, void **) = (void *)(size_t)value;
            } else wcrt_wscan_store_unsigned(arguments, length, value);
        }
    }
    free(item);
    return success ? (suppress ? 2 : 1) : 0;
}

static int wcrt_wscan_float(const wchar_t **source, int suppress, int width,
    enum wcrt_wscan_length length, va_list *arguments)
{
    const wchar_t *start = *source;
    const wchar_t *item_end = wcrt_wscan_float_end(start, width);
    wchar_t *item;
    wchar_t *converted_end;
    int success = 0;
    *source = item_end;
    if (item_end == start) return *start == 0 ? -2 : 0;
    item = wcrt_wscan_item_copy(start, item_end);
    if (item == (wchar_t *)0) return -1;
    if (length == WCRT_WSCAN_CAPITAL_L) {
        long double value = wcstold(item, &converted_end);
        success = converted_end == item + (item_end - start);
        if (success && !suppress) *va_arg(*arguments, long double *) = value;
    } else if (length == WCRT_WSCAN_L) {
        double value = wcstod(item, &converted_end);
        success = converted_end == item + (item_end - start);
        if (success && !suppress) *va_arg(*arguments, double *) = value;
    } else {
        float value = wcstof(item, &converted_end);
        success = converted_end == item + (item_end - start);
        if (success && !suppress) *va_arg(*arguments, float *) = value;
    }
    free(item);
    return success ? (suppress ? 2 : 1) : 0;
}

static int wcrt_wscan_in_set(wchar_t character, const wchar_t *set,
    const wchar_t *end)
{
    while (set < end) {
        if (set + 2 < end && set[1] == L'-') {
            if (character >= set[0] && character <= set[2]) return 1;
            set += 3;
        } else if (character == *set++) return 1;
    }
    return 0;
}

static int wcrt_wscan_characters(const wchar_t **source, int suppress,
    int width, enum wcrt_wscan_length length, wchar_t conversion,
    const wchar_t *set, const wchar_t *set_end, int invert,
    va_list *arguments)
{
    int maximum = width == 0 ? (conversion == L'c' ? 1 : INT_MAX) : width;
    wchar_t *wide = (wchar_t *)0;
    char *narrow = (char *)0;
    int count = 0;
    if (!suppress) {
        if (length == WCRT_WSCAN_L) narrow = va_arg(*arguments, char *);
        else wide = va_arg(*arguments, wchar_t *);
    }
    while (count < maximum && **source != 0) {
        wchar_t character = **source;
        int accepted = conversion == L'c' ? 1 :
            (conversion == L's' ? !iswspace((wint_t)character) :
            (wcrt_wscan_in_set(character, set, set_end) != invert));
        if (!accepted) break;
        if (!suppress) {
            if (length == WCRT_WSCAN_L) {
                int byte = wctob((wint_t)character);
                if (byte == EOF) {
                    errno = EILSEQ;
                    return -1;
                }
                narrow[count] = (char)byte;
            } else wide[count] = character;
        }
        ++*source;
        ++count;
    }
    if (count == 0 || (conversion == L'c' && count < maximum)) {
        return **source == 0 ? -2 : 0;
    }
    if (!suppress && conversion != L'c') {
        if (length == WCRT_WSCAN_L) narrow[count] = '\0';
        else wide[count] = 0;
    }
    return suppress ? 2 : 1;
}

static int wcrt_wstring_scan(const wchar_t *source, const wchar_t *format,
    va_list arguments)
{
    const wchar_t *input = source;
    size_t consumed = 0;
    int assignments = 0;
    int completed = 0;

    while (*format != 0) {
        int suppress = 0;
        int width;
        int result;
        int invert = 0;
        enum wcrt_wscan_length length = WCRT_WSCAN_NONE;
        wchar_t conversion;
        const wchar_t *set = (const wchar_t *)0;
        const wchar_t *set_end = (const wchar_t *)0;
        const wchar_t *before;
        if (iswspace((wint_t)*format)) {
            while (iswspace((wint_t)*format)) ++format;
            while (iswspace((wint_t)*input)) ++input;
            consumed = (size_t)(input - source);
            continue;
        }
        if (*format != L'%') {
            if (*input == 0 || *input != *format) {
                return *input == 0 && completed == 0 ? EOF : assignments;
            }
            ++input;
            ++format;
            consumed = (size_t)(input - source);
            continue;
        }
        ++format;
        if (*format == L'%') {
            if (*input != L'%') {
                return *input == 0 && completed == 0 ? EOF : assignments;
            }
            ++input;
            ++format;
            consumed = (size_t)(input - source);
            continue;
        }
        if (*format == L'*') { suppress = 1; ++format; }
        width = wcrt_wscan_width(&format);
        if (format[0] == L'h' && format[1] == L'h') {
            length = WCRT_WSCAN_HH; format += 2;
        } else if (*format == L'h') {
            length = WCRT_WSCAN_H; ++format;
        } else if (format[0] == L'l' && format[1] == L'l') {
            length = WCRT_WSCAN_LL; format += 2;
        } else if (*format == L'l') {
            length = WCRT_WSCAN_L; ++format;
        } else if (*format == L'j') {
            length = WCRT_WSCAN_J; ++format;
        } else if (*format == L'z') {
            length = WCRT_WSCAN_Z; ++format;
        } else if (*format == L't') {
            length = WCRT_WSCAN_T; ++format;
        } else if (*format == L'L') {
            length = WCRT_WSCAN_CAPITAL_L; ++format;
        }
        conversion = *format;
        if (conversion == 0) return assignments;
        ++format;
        if (conversion == L'[') {
            if (*format == L'^') { invert = 1; ++format; }
            set = format;
            if (*format == L']') ++format;
            while (*format != 0 && *format != L']') ++format;
            set_end = format;
            if (*format == L']') ++format;
        }
        if (conversion != L'c' && conversion != L'[' && conversion != L'n') {
            while (iswspace((wint_t)*input)) ++input;
        }
        consumed = (size_t)(input - source);
        if (conversion == L'n') {
            if (!suppress) wcrt_wscan_store_count(&arguments, length, consumed);
            completed = 1;
            continue;
        }
        before = input;
        if (conversion == L'd' || conversion == L'i' || conversion == L'o' ||
            conversion == L'u' || conversion == L'x' || conversion == L'X' ||
            conversion == L'p') {
            result = wcrt_wscan_integer(&input, suppress, width, length,
                conversion, &arguments);
        } else if (conversion == L'a' || conversion == L'A' ||
            conversion == L'e' || conversion == L'E' || conversion == L'f' ||
            conversion == L'F' || conversion == L'g' || conversion == L'G') {
            result = wcrt_wscan_float(&input, suppress, width, length,
                &arguments);
        } else if (conversion == L'c' || conversion == L's' ||
            conversion == L'[') {
            result = wcrt_wscan_characters(&input, suppress, width, length,
                conversion, set, set_end, invert, &arguments);
        } else return assignments;
        consumed += (size_t)(input - before);
        if (result < 0) return completed == 0 ? EOF : assignments;
        if (result == 0) return assignments;
        if (result == 1) ++assignments;
        completed = 1;
    }
    return assignments;
}

int vswscanf(const wchar_t *source, const wchar_t *format, va_list arguments)
{
    if (source == (const wchar_t *)0 || format == (const wchar_t *)0) {
        return EOF;
    }
    return wcrt_wstring_scan(source, format, arguments);
}

int vfwscanf(FILE *stream, const wchar_t *format, va_list arguments)
{
    char *narrow_format;
    int result;

    if (stream == (FILE *)0 || format == (const wchar_t *)0) return EOF;
    __wcrt_prepare_stream(stream);
    if (__wcrt_require_orientation(stream, WCRT_ORIENTATION_WIDE) != 0) {
        return EOF;
    }
    if (wcrt_wscan_format(format, &narrow_format) != 0) return EOF;
    result = __wcrt_vfwscanf_c_locale(stream, narrow_format, arguments);
    free(narrow_format);
    return result;
}

int vwscanf(const wchar_t *format, va_list arguments)
{
    return vfwscanf(stdin, format, arguments);
}

int swscanf(const wchar_t *source, const wchar_t *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vswscanf(source, format, arguments);
    va_end(arguments);
    return result;
}

int fwscanf(FILE *stream, const wchar_t *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vfwscanf(stream, format, arguments);
    va_end(arguments);
    return result;
}

int wscanf(const wchar_t *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vwscanf(format, arguments);
    va_end(arguments);
    return result;
}
