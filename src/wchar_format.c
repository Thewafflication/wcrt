/**
 * @file wchar_format.c
 * @brief Implements C99 wide formatted output.
 */

#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "internal/file.h"

enum wcrt_wformat_length {
    WCRT_WFORMAT_NONE,
    WCRT_WFORMAT_HH,
    WCRT_WFORMAT_H,
    WCRT_WFORMAT_L,
    WCRT_WFORMAT_LL,
    WCRT_WFORMAT_J,
    WCRT_WFORMAT_Z,
    WCRT_WFORMAT_T,
    WCRT_WFORMAT_CAPITAL_L
};

struct wcrt_woutput {
    FILE *stream;
    wchar_t *string;
    size_t count;
    size_t capacity;
    int failed;
    int overflow;
};

struct wcrt_wformat_spec {
    int alternate;
    int zero;
    int left;
    int space;
    int plus;
    int width;
    int precision;
    enum wcrt_wformat_length length;
    wchar_t conversion;
};

static void wcrt_wemit(struct wcrt_woutput *output, wchar_t character)
{
    if (output->failed) return;
    if (output->stream != (FILE *)0) {
        if (fputwc(character, output->stream) == WEOF) output->failed = 1;
    } else if (output->count + 1 < output->capacity) {
        output->string[output->count] = character;
    } else {
        output->overflow = 1;
    }
    if (output->count == (size_t)INT_MAX) output->failed = 1;
    else ++output->count;
}

static void wcrt_wpadding(struct wcrt_woutput *output, wchar_t character,
    int count)
{
    while (count-- > 0 && !output->failed) wcrt_wemit(output, character);
}

static void wcrt_wwide_field(struct wcrt_woutput *output,
    const wchar_t *text, int length, const struct wcrt_wformat_spec *spec)
{
    int padding = spec->width > length ? spec->width - length : 0;
    if (!spec->left) wcrt_wpadding(output, L' ', padding);
    while (length-- > 0 && !output->failed) wcrt_wemit(output, *text++);
    if (spec->left) wcrt_wpadding(output, L' ', padding);
}

static int wcrt_wformat_number(const wchar_t **format)
{
    int value = 0;
    while (**format >= L'0' && **format <= L'9') {
        int digit = **format - L'0';
        if (value > (INT_MAX - digit) / 10) return -1;
        value = value * 10 + digit;
        ++*format;
    }
    return value;
}

static int wcrt_append_decimal(char *text, int position, int value)
{
    char reverse[16];
    int count = 0;
    int index;
    do {
        reverse[count++] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    for (index = count - 1; index >= 0; --index) {
        text[position++] = reverse[index];
    }
    return position;
}

static int wcrt_narrow_spec(char *text,
    const struct wcrt_wformat_spec *spec, const char *length,
    char conversion)
{
    int position = 0;
    text[position++] = '%';
    if (spec->alternate) text[position++] = '#';
    if (spec->zero && !spec->left) text[position++] = '0';
    if (spec->left) text[position++] = '-';
    if (spec->space) text[position++] = ' ';
    if (spec->plus) text[position++] = '+';
    if (spec->width > 0) {
        position = wcrt_append_decimal(text, position, spec->width);
    }
    if (spec->precision >= 0) {
        text[position++] = '.';
        position = wcrt_append_decimal(text, position, spec->precision);
    }
    while (*length != '\0') text[position++] = *length++;
    text[position++] = conversion;
    text[position] = '\0';
    return position;
}

static int wcrt_emit_narrow_result(struct wcrt_woutput *output,
    const char *format, int kind, unsigned long long unsigned_value,
    long long signed_value, long double real_value, void *pointer)
{
    int length;
    int written;
    char *text;

    if (kind == 1) length = snprintf((char *)0, 0, format, signed_value);
    else if (kind == 2) {
        length = snprintf((char *)0, 0, format, unsigned_value);
    } else if (kind == 3) {
        length = snprintf((char *)0, 0, format, pointer);
    } else if (kind == 4) {
        length = snprintf((char *)0, 0, format, (double)real_value);
    } else {
        length = snprintf((char *)0, 0, format, real_value);
    }
    if (length < 0 || (size_t)length == (size_t)-1) return -1;
    text = (char *)malloc((size_t)length + 1U);
    if (text == (char *)0) return -1;
    if (kind == 1) written = snprintf(text, (size_t)length + 1U,
        format, signed_value);
    else if (kind == 2) written = snprintf(text, (size_t)length + 1U,
        format, unsigned_value);
    else if (kind == 3) written = snprintf(text, (size_t)length + 1U,
        format, pointer);
    else if (kind == 4) written = snprintf(text, (size_t)length + 1U,
        format, (double)real_value);
    else written = snprintf(text, (size_t)length + 1U, format, real_value);
    if (written == length) {
        int index;
        for (index = 0; index < length; ++index) {
            wcrt_wemit(output, (wchar_t)(unsigned char)text[index]);
        }
    }
    free(text);
    return written == length ? 0 : -1;
}

static long long wcrt_wsigned_argument(va_list *arguments,
    enum wcrt_wformat_length length)
{
    if (length == WCRT_WFORMAT_HH) {
        return (signed char)va_arg(*arguments, int);
    }
    if (length == WCRT_WFORMAT_H) return (short)va_arg(*arguments, int);
    if (length == WCRT_WFORMAT_L) return va_arg(*arguments, long);
    if (length == WCRT_WFORMAT_LL) return va_arg(*arguments, long long);
    if (length == WCRT_WFORMAT_J) return va_arg(*arguments, intmax_t);
    if (length == WCRT_WFORMAT_Z || length == WCRT_WFORMAT_T) {
        return va_arg(*arguments, ptrdiff_t);
    }
    return va_arg(*arguments, int);
}

static unsigned long long wcrt_wunsigned_argument(va_list *arguments,
    enum wcrt_wformat_length length)
{
    if (length == WCRT_WFORMAT_HH) {
        return (unsigned char)va_arg(*arguments, unsigned int);
    }
    if (length == WCRT_WFORMAT_H) {
        return (unsigned short)va_arg(*arguments, unsigned int);
    }
    if (length == WCRT_WFORMAT_L) return va_arg(*arguments, unsigned long);
    if (length == WCRT_WFORMAT_LL) {
        return va_arg(*arguments, unsigned long long);
    }
    if (length == WCRT_WFORMAT_J) return va_arg(*arguments, uintmax_t);
    if (length == WCRT_WFORMAT_Z) return va_arg(*arguments, size_t);
    if (length == WCRT_WFORMAT_T) return va_arg(*arguments, size_t);
    return va_arg(*arguments, unsigned int);
}

static void wcrt_wstore_count(va_list *arguments,
    enum wcrt_wformat_length length, size_t count)
{
    if (length == WCRT_WFORMAT_HH) *va_arg(*arguments, signed char *) =
        (signed char)count;
    else if (length == WCRT_WFORMAT_H) *va_arg(*arguments, short *) =
        (short)count;
    else if (length == WCRT_WFORMAT_L) *va_arg(*arguments, long *) =
        (long)count;
    else if (length == WCRT_WFORMAT_LL) *va_arg(*arguments, long long *) =
        (long long)count;
    else if (length == WCRT_WFORMAT_J) *va_arg(*arguments, intmax_t *) =
        (intmax_t)count;
    else if (length == WCRT_WFORMAT_Z || length == WCRT_WFORMAT_T) {
        *va_arg(*arguments, ptrdiff_t *) = (ptrdiff_t)count;
    }
    else *va_arg(*arguments, int *) = (int)count;
}

static int wcrt_wformat_character(struct wcrt_woutput *output,
    const struct wcrt_wformat_spec *spec, va_list *arguments)
{
    wchar_t character;
    if (spec->length == WCRT_WFORMAT_L) {
        character = (wchar_t)va_arg(*arguments, wint_t);
    } else {
        wint_t converted = btowc(va_arg(*arguments, int));
        if (converted == WEOF) {
            errno = EILSEQ;
            return -1;
        }
        character = (wchar_t)converted;
    }
    wcrt_wwide_field(output, &character, 1, spec);
    return 0;
}

static int wcrt_wformat_string(struct wcrt_woutput *output,
    const struct wcrt_wformat_spec *spec, va_list *arguments)
{
    if (spec->length != WCRT_WFORMAT_L) {
        const char *text = va_arg(*arguments, const char *);
        int length = 0;
        int padding;
        if (text == (const char *)0) text = "(null)";
        while (text[length] != '\0' &&
            (spec->precision < 0 || length < spec->precision)) ++length;
        padding = spec->width > length ? spec->width - length : 0;
        if (!spec->left) wcrt_wpadding(output, L' ', padding);
        while (length-- > 0) {
            wcrt_wemit(output, (wchar_t)(unsigned char)*text++);
        }
        if (spec->left) wcrt_wpadding(output, L' ', padding);
    } else {
        const wchar_t *text = va_arg(*arguments, const wchar_t *);
        int length = 0;
        if (text == (const wchar_t *)0) text = L"(null)";
        while (text[length] != 0 &&
            (spec->precision < 0 || length < spec->precision)) ++length;
        wcrt_wwide_field(output, text, length, spec);
    }
    return 0;
}

static int wcrt_wformat_conversion(struct wcrt_woutput *output,
    const struct wcrt_wformat_spec *spec, va_list *arguments)
{
    char format[64];
    char conversion = (char)spec->conversion;

    if (spec->conversion == L'c') {
        return wcrt_wformat_character(output, spec, arguments);
    }
    if (spec->conversion == L's') {
        return wcrt_wformat_string(output, spec, arguments);
    }
    if (spec->conversion == L'n') {
        wcrt_wstore_count(arguments, spec->length, output->count);
        return 0;
    }
    if (spec->conversion == L'%') {
        wcrt_wemit(output, L'%');
        return 0;
    }
    if (spec->conversion == L'd' || spec->conversion == L'i') {
        long long value = wcrt_wsigned_argument(arguments, spec->length);
        wcrt_narrow_spec(format, spec, "ll", conversion);
        return wcrt_emit_narrow_result(output, format, 1, 0, value, 0, 0);
    }
    if (spec->conversion == L'u' || spec->conversion == L'o' ||
        spec->conversion == L'x' || spec->conversion == L'X') {
        unsigned long long value = wcrt_wunsigned_argument(arguments,
            spec->length);
        wcrt_narrow_spec(format, spec, "ll", conversion);
        return wcrt_emit_narrow_result(output, format, 2, value, 0, 0, 0);
    }
    if (spec->conversion == L'p') {
        void *value = va_arg(*arguments, void *);
        wcrt_narrow_spec(format, spec, "", 'p');
        return wcrt_emit_narrow_result(output, format, 3, 0, 0, 0, value);
    }
    if (spec->conversion == L'f' || spec->conversion == L'F' ||
        spec->conversion == L'e' || spec->conversion == L'E' ||
        spec->conversion == L'g' || spec->conversion == L'G' ||
        spec->conversion == L'a' || spec->conversion == L'A') {
        long double value;
        int kind;
        if (spec->length == WCRT_WFORMAT_CAPITAL_L) {
            value = va_arg(*arguments, long double);
            kind = 5;
            wcrt_narrow_spec(format, spec, "L", conversion);
        } else {
            value = (long double)va_arg(*arguments, double);
            kind = 4;
            wcrt_narrow_spec(format, spec, "", conversion);
        }
        return wcrt_emit_narrow_result(output, format, kind, 0, 0, value, 0);
    }
    return -1;
}

static int wcrt_wformat(struct wcrt_woutput *output, const wchar_t *format,
    va_list arguments)
{
    while (*format != 0 && !output->failed) {
        struct wcrt_wformat_spec spec;
        if (*format != L'%') {
            wcrt_wemit(output, *format++);
            continue;
        }
        ++format;
        spec.alternate = spec.zero = spec.left = 0;
        spec.space = spec.plus = 0;
        spec.width = 0;
        spec.precision = -1;
        spec.length = WCRT_WFORMAT_NONE;
        for (;;) {
            if (*format == L'#') spec.alternate = 1;
            else if (*format == L'0') spec.zero = 1;
            else if (*format == L'-') spec.left = 1;
            else if (*format == L' ') spec.space = 1;
            else if (*format == L'+') spec.plus = 1;
            else break;
            ++format;
        }
        if (*format == L'*') {
            spec.width = va_arg(arguments, int);
            ++format;
            if (spec.width < 0) {
                if (spec.width == INT_MIN) return -1;
                spec.left = 1;
                spec.width = -spec.width;
            }
        } else {
            spec.width = wcrt_wformat_number(&format);
            if (spec.width < 0) return -1;
        }
        if (*format == L'.') {
            ++format;
            if (*format == L'*') {
                spec.precision = va_arg(arguments, int);
                ++format;
                if (spec.precision < 0) spec.precision = -1;
            } else {
                spec.precision = wcrt_wformat_number(&format);
                if (spec.precision < 0) return -1;
            }
        }
        if (*format == L'h') {
            spec.length = *++format == L'h' ? WCRT_WFORMAT_HH :
                WCRT_WFORMAT_H;
            if (spec.length == WCRT_WFORMAT_HH) ++format;
        } else if (*format == L'l') {
            spec.length = *++format == L'l' ? WCRT_WFORMAT_LL :
                WCRT_WFORMAT_L;
            if (spec.length == WCRT_WFORMAT_LL) ++format;
        } else if (*format == L'j') {
            spec.length = WCRT_WFORMAT_J; ++format;
        } else if (*format == L'z') {
            spec.length = WCRT_WFORMAT_Z; ++format;
        } else if (*format == L't') {
            spec.length = WCRT_WFORMAT_T; ++format;
        } else if (*format == L'L') {
            spec.length = WCRT_WFORMAT_CAPITAL_L; ++format;
        }
        spec.conversion = *format;
        if (*format == 0) return -1;
        ++format;
        if (wcrt_wformat_conversion(output, &spec, &arguments) != 0) {
            output->failed = 1;
        }
    }
    if (output->stream == (FILE *)0 && output->capacity != 0 &&
        !output->failed && !output->overflow) {
        output->string[output->count] = 0;
    }
    return output->failed || output->overflow ? -1 : (int)output->count;
}

int vfwprintf(FILE *stream, const wchar_t *format, va_list arguments)
{
    struct wcrt_woutput output;
    if (stream == (FILE *)0 || format == (const wchar_t *)0) return -1;
    if (__wcrt_require_orientation(stream, WCRT_ORIENTATION_WIDE) != 0) {
        return -1;
    }
    output.stream = stream;
    output.string = (wchar_t *)0;
    output.count = 0;
    output.capacity = 0;
    output.failed = 0;
    output.overflow = 0;
    return wcrt_wformat(&output, format, arguments);
}

int vwprintf(const wchar_t *format, va_list arguments)
{
    return vfwprintf(stdout, format, arguments);
}

int vswprintf(wchar_t *destination, size_t capacity,
    const wchar_t *format, va_list arguments)
{
    struct wcrt_woutput output;
    if (destination == (wchar_t *)0 || format == (const wchar_t *)0) return -1;
    output.stream = (FILE *)0;
    output.string = destination;
    output.count = 0;
    output.capacity = capacity;
    output.failed = 0;
    output.overflow = capacity == 0;
    return wcrt_wformat(&output, format, arguments);
}

int fwprintf(FILE *stream, const wchar_t *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vfwprintf(stream, format, arguments);
    va_end(arguments);
    return result;
}

int wprintf(const wchar_t *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vwprintf(format, arguments);
    va_end(arguments);
    return result;
}

int swprintf(wchar_t *destination, size_t capacity,
    const wchar_t *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vswprintf(destination, capacity, format, arguments);
    va_end(arguments);
    return result;
}
