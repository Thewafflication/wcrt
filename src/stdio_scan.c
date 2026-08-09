/**
 * @file stdio_scan.c
 * @brief Implements C89 and C99 narrow formatted input.
 */

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#include "internal/file.h"

/** @brief Parsed length modifiers used by the scanning engine. */
enum wcrt_scan_length {
    WCRT_SCAN_NONE,
    WCRT_SCAN_HH,
    WCRT_SCAN_H,
    WCRT_SCAN_L,
    WCRT_SCAN_LL,
    WCRT_SCAN_J,
    WCRT_SCAN_Z,
    WCRT_SCAN_T,
    WCRT_SCAN_CAPITAL_L
};

/** @brief String or stream input with one retained lookahead byte. */
struct wcrt_input {
    const char *string;
    size_t position;
    FILE *stream;
    int wide;
    size_t consumed;
    int lookahead;
    int has_lookahead;
    int at_end;
};

/** @brief Growable numeric input item. */
struct wcrt_scan_item {
    char *text;
    size_t length;
    size_t capacity;
    int failed;
};

/** @brief Returns the next byte without consuming it. */
static int wcrt_input_peek(struct wcrt_input *input)
{
    if (input->has_lookahead) return input->lookahead;
    if (input->stream != NULL) {
        if (input->wide) {
            wint_t character = fgetwc(input->stream);
            input->lookahead = character == WEOF ? EOF : (int)character;
        } else input->lookahead = fgetc(input->stream);
    } else {
        input->lookahead = (unsigned char)input->string[input->position];
        if (input->lookahead == 0) input->lookahead = EOF;
    }
    if (input->lookahead == EOF) {
        input->at_end = 1;
        return EOF;
    }
    input->has_lookahead = 1;
    return input->lookahead;
}

/** @brief Consumes and counts the next byte. */
static int wcrt_input_get(struct wcrt_input *input)
{
    int character = wcrt_input_peek(input);
    if (character == EOF) return EOF;
    input->has_lookahead = 0;
    if (input->stream == NULL) ++input->position;
    ++input->consumed;
    return character;
}

/** @brief Restores retained stream lookahead before returning to the caller. */
static void wcrt_input_finish(struct wcrt_input *input)
{
    if (input->stream != NULL && input->has_lookahead) {
        int failed = input->wide ?
            ungetwc((wint_t)input->lookahead, input->stream) == WEOF :
            ungetc(input->lookahead, input->stream) == EOF;
        if (failed) {
            input->at_end = 1;
        }
        input->has_lookahead = 0;
    }
}

/** @brief Consumes input white space. */
static void wcrt_input_space(struct wcrt_input *input)
{
    int character;
    while ((character = wcrt_input_peek(input)) != EOF &&
        (input->wide ? iswspace((wint_t)character) :
        isspace((unsigned char)character))) {
        (void)wcrt_input_get(input);
    }
}

/** @brief Appends one byte to a growable numeric item. */
static void wcrt_item_append(struct wcrt_scan_item *item, int character)
{
    if (item->failed) return;
    if (item->length + 1 >= item->capacity) {
        size_t capacity = item->capacity == 0 ? 32U : item->capacity * 2U;
        char *expanded;
        if (capacity <= item->capacity) {
            item->failed = 1;
            return;
        }
        expanded = (char *)realloc(item->text, capacity);
        if (expanded == NULL) {
            item->failed = 1;
            return;
        }
        item->text = expanded;
        item->capacity = capacity;
    }
    item->text[item->length++] = (char)character;
    item->text[item->length] = '\0';
}

/** @brief Consumes and appends a byte while a field width remains. */
static void wcrt_item_get(struct wcrt_input *input,
    struct wcrt_scan_item *item, int *remaining)
{
    if (*remaining == 0) return;
    wcrt_item_append(item, wcrt_input_get(input));
    if (*remaining != INT_MAX) --*remaining;
}

/** @brief Tests one case-insensitive ASCII byte. */
static int wcrt_scan_ascii(int character, int lower)
{
    if (character >= 'A' && character <= 'Z') {
        character += 'a' - 'A';
    }
    return character == lower;
}

/** @brief Collects an optional sign into a numeric item. */
static void wcrt_scan_sign(struct wcrt_input *input,
    struct wcrt_scan_item *item, int *remaining)
{
    int character = wcrt_input_peek(input);
    if (*remaining != 0 && (character == '+' || character == '-')) {
        wcrt_item_get(input, item, remaining);
    }
}

/** @brief Collects an integer input item with conversion-specific radix. */
static void wcrt_integer_item(struct wcrt_input *input,
    struct wcrt_scan_item *item, int width, char conversion)
{
    int remaining = width == 0 ? INT_MAX : width;
    int character;
    int base = conversion == 'o' ? 8 :
        (conversion == 'x' || conversion == 'X' || conversion == 'p' ?
        16 : 10);
    wcrt_scan_sign(input, item, &remaining);
    character = wcrt_input_peek(input);
    if (remaining != 0 && character == '0' &&
        (conversion == 'i' || base == 16)) {
        wcrt_item_get(input, item, &remaining);
        character = wcrt_input_peek(input);
        if (remaining != 0 && (character == 'x' || character == 'X')) {
            wcrt_item_get(input, item, &remaining);
            base = 16;
        } else if (conversion == 'i') {
            base = 8;
        }
    } else if (conversion == 'i') {
        base = 10;
    }
    while (remaining != 0) {
        int digit;
        character = wcrt_input_peek(input);
        if (character >= '0' && character <= '9') digit = character - '0';
        else if (character >= 'a' && character <= 'f') {
            digit = character - 'a' + 10;
        } else if (character >= 'A' && character <= 'F') {
            digit = character - 'A' + 10;
        } else {
            break;
        }
        if (digit >= base) break;
        wcrt_item_get(input, item, &remaining);
    }
}

/** @brief Collects a case-insensitive expected word as far as it matches. */
static void wcrt_scan_word(struct wcrt_input *input,
    struct wcrt_scan_item *item, int *remaining, const char *word)
{
    while (*word != '\0' && *remaining != 0 &&
        wcrt_scan_ascii(wcrt_input_peek(input), *word)) {
        wcrt_item_get(input, item, remaining);
        ++word;
    }
}

/** @brief Collects a decimal or hexadecimal floating input item. */
static void wcrt_float_item(struct wcrt_input *input,
    struct wcrt_scan_item *item, int width)
{
    int remaining = width == 0 ? INT_MAX : width;
    int character;
    int hexadecimal = 0;
    int digits = 0;
    wcrt_scan_sign(input, item, &remaining);
    character = wcrt_input_peek(input);
    if (remaining != 0 && wcrt_scan_ascii(character, 'i')) {
        wcrt_scan_word(input, item, &remaining, "infinity");
        return;
    }
    if (remaining != 0 && wcrt_scan_ascii(character, 'n')) {
        wcrt_scan_word(input, item, &remaining, "nan");
        if (remaining != 0 && wcrt_input_peek(input) == '(') {
            wcrt_item_get(input, item, &remaining);
            while (remaining != 0) {
                character = wcrt_input_peek(input);
                if (!isalnum((unsigned char)character) && character != '_') {
                    break;
                }
                wcrt_item_get(input, item, &remaining);
            }
            if (remaining != 0 && wcrt_input_peek(input) == ')') {
                wcrt_item_get(input, item, &remaining);
            }
        }
        return;
    }
    if (remaining != 0 && character == '0') {
        wcrt_item_get(input, item, &remaining);
        ++digits;
        if (remaining != 0 &&
            (wcrt_input_peek(input) == 'x' ||
            wcrt_input_peek(input) == 'X')) {
            wcrt_item_get(input, item, &remaining);
            hexadecimal = 1;
            digits = 0;
        }
    }
    while (remaining != 0) {
        character = wcrt_input_peek(input);
        if (isdigit((unsigned char)character) || (hexadecimal &&
            ((character >= 'a' && character <= 'f') ||
            (character >= 'A' && character <= 'F')))) {
            wcrt_item_get(input, item, &remaining);
            ++digits;
        } else {
            break;
        }
    }
    if (remaining != 0 && wcrt_input_peek(input) == '.') {
        wcrt_item_get(input, item, &remaining);
        while (remaining != 0) {
            character = wcrt_input_peek(input);
            if (isdigit((unsigned char)character) || (hexadecimal &&
                ((character >= 'a' && character <= 'f') ||
                (character >= 'A' && character <= 'F')))) {
                wcrt_item_get(input, item, &remaining);
                ++digits;
            } else {
                break;
            }
        }
    }
    character = wcrt_input_peek(input);
    if (remaining != 0 && ((hexadecimal &&
        (character == 'p' || character == 'P')) || (!hexadecimal &&
        (character == 'e' || character == 'E')))) {
        wcrt_item_get(input, item, &remaining);
        wcrt_scan_sign(input, item, &remaining);
        while (remaining != 0 &&
            isdigit((unsigned char)wcrt_input_peek(input))) {
            wcrt_item_get(input, item, &remaining);
        }
    }
    (void)digits;
}

/** @brief Stores a signed integer through the selected destination type. */
static void wcrt_store_signed(va_list *arguments,
    enum wcrt_scan_length length, long long value)
{
    if (length == WCRT_SCAN_HH) {
        *va_arg(*arguments, signed char *) = (signed char)value;
    } else if (length == WCRT_SCAN_H) {
        *va_arg(*arguments, short *) = (short)value;
    } else if (length == WCRT_SCAN_L) {
        *va_arg(*arguments, long *) = (long)value;
    } else if (length == WCRT_SCAN_LL) {
        *va_arg(*arguments, long long *) = value;
    } else if (length == WCRT_SCAN_J) {
        *va_arg(*arguments, intmax_t *) = (intmax_t)value;
    } else if (length == WCRT_SCAN_Z || length == WCRT_SCAN_T) {
        *va_arg(*arguments, ptrdiff_t *) = (ptrdiff_t)value;
    } else {
        *va_arg(*arguments, int *) = (int)value;
    }
}

/** @brief Stores an unsigned integer through the selected destination type. */
static void wcrt_store_unsigned(va_list *arguments,
    enum wcrt_scan_length length, unsigned long long value)
{
    if (length == WCRT_SCAN_HH) {
        *va_arg(*arguments, unsigned char *) = (unsigned char)value;
    } else if (length == WCRT_SCAN_H) {
        *va_arg(*arguments, unsigned short *) = (unsigned short)value;
    } else if (length == WCRT_SCAN_L) {
        *va_arg(*arguments, unsigned long *) = (unsigned long)value;
    } else if (length == WCRT_SCAN_LL) {
        *va_arg(*arguments, unsigned long long *) = value;
    } else if (length == WCRT_SCAN_J) {
        *va_arg(*arguments, uintmax_t *) = (uintmax_t)value;
    } else if (length == WCRT_SCAN_Z || length == WCRT_SCAN_T) {
        *va_arg(*arguments, size_t *) = (size_t)value;
    } else {
        *va_arg(*arguments, unsigned int *) = (unsigned int)value;
    }
}

/** @brief Stores a consumed-character count for a percent-n directive. */
static void wcrt_store_scan_count(va_list *arguments,
    enum wcrt_scan_length length, size_t count)
{
    if (length == WCRT_SCAN_HH) {
        *va_arg(*arguments, signed char *) = (signed char)count;
    } else if (length == WCRT_SCAN_H) {
        *va_arg(*arguments, short *) = (short)count;
    } else if (length == WCRT_SCAN_L) {
        *va_arg(*arguments, long *) = (long)count;
    } else if (length == WCRT_SCAN_LL) {
        *va_arg(*arguments, long long *) = (long long)count;
    } else if (length == WCRT_SCAN_J) {
        *va_arg(*arguments, intmax_t *) = (intmax_t)count;
    } else if (length == WCRT_SCAN_Z || length == WCRT_SCAN_T) {
        *va_arg(*arguments, ptrdiff_t *) = (ptrdiff_t)count;
    } else {
        *va_arg(*arguments, int *) = (int)count;
    }
}

/** @brief Tests membership in a scanf bracket conversion set. */
static int wcrt_in_set(int character, const char *set, const char *end)
{
    while (set < end) {
        if (set + 2 < end && set[1] == '-') {
            if (character >= (unsigned char)set[0] &&
                character <= (unsigned char)set[2]) {
                return 1;
            }
            set += 3;
        } else if (character == (unsigned char)*set++) {
            return 1;
        }
    }
    return 0;
}

/** @brief Parses a checked decimal field width. */
static int wcrt_scan_width(const char **format)
{
    int result = 0;
    while (isdigit((unsigned char)**format)) {
        int digit = *(*format)++ - '0';
        if (result > (INT_MAX - digit) / 10) result = INT_MAX;
        else result = result * 10 + digit;
    }
    return result;
}

/** @brief Returns either matching failure or initial input failure. */
static int wcrt_scan_failure(struct wcrt_input *input, int assignments,
    int completed)
{
    return input->at_end && completed == 0 ? EOF : assignments;
}

/** @brief Scans one integer directive. */
static int wcrt_scan_integer(struct wcrt_input *input, int suppress,
    int width, enum wcrt_scan_length length, char conversion,
    va_list *arguments)
{
    struct wcrt_scan_item item = {NULL, 0, 0, 0};
    char *end;
    int base = conversion == 'o' ? 8 :
        (conversion == 'x' || conversion == 'X' || conversion == 'p' ?
        16 : (conversion == 'i' ? 0 : 10));
    int success = 0;
    wcrt_integer_item(input, &item, width, conversion);
    if (item.failed) {
        free(item.text);
        return -1;
    }
    if (item.length != 0) {
        if (conversion == 'd' || conversion == 'i') {
            long long value = strtoll(item.text, &end, base);
            success = end == item.text + item.length;
            if (success && !suppress) {
                wcrt_store_signed(arguments, length, value);
            }
        } else {
            unsigned long long value = strtoull(item.text, &end, base);
            success = end == item.text + item.length;
            if (success && !suppress) {
                if (conversion == 'p') {
                    *va_arg(*arguments, void **) = (void *)(size_t)value;
                } else {
                    wcrt_store_unsigned(arguments, length, value);
                }
            }
        }
    }
    free(item.text);
    if (!success && item.length == 0 && input->at_end) return -2;
    return success ? (suppress ? 2 : 1) : 0;
}

/** @brief Scans one floating directive. */
static int wcrt_scan_float(struct wcrt_input *input, int suppress,
    int width, enum wcrt_scan_length length, va_list *arguments)
{
    struct wcrt_scan_item item = {NULL, 0, 0, 0};
    char *end;
    int success = 0;
    wcrt_float_item(input, &item, width);
    if (item.failed) {
        free(item.text);
        return -1;
    }
    if (item.length != 0) {
        if (length == WCRT_SCAN_CAPITAL_L) {
            long double value = strtold(item.text, &end);
            success = end == item.text + item.length;
            if (success && !suppress) {
                *va_arg(*arguments, long double *) = value;
            }
        } else if (length == WCRT_SCAN_L) {
            double value = strtod(item.text, &end);
            success = end == item.text + item.length;
            if (success && !suppress) *va_arg(*arguments, double *) = value;
        } else {
            float value = strtof(item.text, &end);
            success = end == item.text + item.length;
            if (success && !suppress) *va_arg(*arguments, float *) = value;
        }
    }
    free(item.text);
    if (!success && item.length == 0 && input->at_end) return -2;
    return success ? (suppress ? 2 : 1) : 0;
}

/** @brief Scans c, s, or bracket character conversions. */
static int wcrt_scan_characters(struct wcrt_input *input, int suppress,
    int width, enum wcrt_scan_length length, char conversion,
    const char *set, const char *set_end, int invert, va_list *arguments,
    int secure)
{
    int maximum = width == 0 ?
        (conversion == 'c' ? 1 : INT_MAX) : width;
    char *narrow = NULL;
    wchar_t *wide = NULL;
    unsigned int output_size = (unsigned int)-1;
    int count = 0;
    int wide_output = input->wide ? length != WCRT_SCAN_L :
        length == WCRT_SCAN_L;
    if (!suppress) {
        if (wide_output) wide = va_arg(*arguments, wchar_t *);
        else narrow = va_arg(*arguments, char *);
        if (secure) output_size = va_arg(*arguments, unsigned int);
        if (conversion == 'c' && output_size < (unsigned int)maximum) {
            errno = EINVAL;
            return -1;
        }
    }
    while (count < maximum) {
        int character = wcrt_input_peek(input);
        int accepted;
        if (character == EOF) break;
        if (conversion == 'c') accepted = 1;
        else if (conversion == 's') {
            accepted = !isspace((unsigned char)character);
        } else {
            accepted = wcrt_in_set(character, set, set_end) != invert;
        }
        if (!accepted) break;
        if (!suppress && conversion != 'c' &&
            (unsigned int)(count + 1) >= output_size) {
            if (wide_output) wide[0] = 0;
            else narrow[0] = '\0';
            errno = EINVAL;
            return -1;
        }
        character = wcrt_input_get(input);
        if (!suppress) {
            if (wide_output) wide[count] = (wchar_t)character;
            else if (input->wide) {
                int converted = wctob((wint_t)character);
                if (converted == EOF) {
                    errno = EILSEQ;
                    return -1;
                }
                narrow[count] = (char)converted;
            } else narrow[count] = (char)character;
        }
        ++count;
    }
    if (count == 0 || (conversion == 'c' && count < maximum)) {
        return input->at_end ? -2 : 0;
    }
    if (!suppress && conversion != 'c') {
        if (wide_output) wide[count] = 0;
        else narrow[count] = '\0';
    }
    return suppress ? 2 : 1;
}

/** @brief Implements the shared formatted-input directive parser. */
static int wcrt_scan(struct wcrt_input *input, const char *format,
    va_list arguments, int secure)
{
    int assignments = 0;
    int completed = 0;
    while (*format != '\0') {
        int suppress = 0;
        int width = 0;
        int result;
        enum wcrt_scan_length length = WCRT_SCAN_NONE;
        char conversion;
        const char *set = NULL;
        const char *set_end = NULL;
        int invert = 0;

        if (isspace((unsigned char)*format)) {
            while (isspace((unsigned char)*format)) ++format;
            wcrt_input_space(input);
            continue;
        }
        if (*format != '%') {
            int character = wcrt_input_peek(input);
            if (character == EOF || character != (unsigned char)*format) {
                return wcrt_scan_failure(input, assignments, completed);
            }
            (void)wcrt_input_get(input);
            ++format;
            continue;
        }
        ++format;
        if (*format == '%') {
            int character = wcrt_input_peek(input);
            if (character == EOF || character != '%') {
                return wcrt_scan_failure(input, assignments, completed);
            }
            (void)wcrt_input_get(input);
            ++format;
            continue;
        }
        if (*format == '*') {
            suppress = 1;
            ++format;
        }
        width = wcrt_scan_width(&format);
        if (format[0] == 'h' && format[1] == 'h') {
            length = WCRT_SCAN_HH;
            format += 2;
        } else if (*format == 'h') {
            length = WCRT_SCAN_H;
            ++format;
        } else if (format[0] == 'l' && format[1] == 'l') {
            length = WCRT_SCAN_LL;
            format += 2;
        } else if (*format == 'l') {
            length = WCRT_SCAN_L;
            ++format;
        } else if (*format == 'j') {
            length = WCRT_SCAN_J;
            ++format;
        } else if (*format == 'z') {
            length = WCRT_SCAN_Z;
            ++format;
        } else if (*format == 't') {
            length = WCRT_SCAN_T;
            ++format;
        } else if (*format == 'L') {
            length = WCRT_SCAN_CAPITAL_L;
            ++format;
        }
        conversion = *format;
        if (conversion == '\0') return assignments;
        ++format;
        if (conversion == '[') {
            if (*format == '^') {
                invert = 1;
                ++format;
            }
            set = format;
            if (*format == ']') ++format;
            while (*format != '\0' && *format != ']') ++format;
            set_end = format;
            if (*format == ']') ++format;
        }
        if (conversion != 'c' && conversion != '[' && conversion != 'n') {
            wcrt_input_space(input);
        }
        if (conversion == 'n') {
            if (!suppress) {
                wcrt_store_scan_count(&arguments, length, input->consumed);
            }
            completed = 1;
            continue;
        }
        if (conversion == 'd' || conversion == 'i' || conversion == 'o' ||
            conversion == 'u' || conversion == 'x' || conversion == 'X' ||
            conversion == 'p') {
            result = wcrt_scan_integer(input, suppress, width, length,
                conversion, &arguments);
        } else if (conversion == 'a' || conversion == 'A' ||
            conversion == 'e' || conversion == 'E' || conversion == 'f' ||
            conversion == 'F' || conversion == 'g' || conversion == 'G') {
            result = wcrt_scan_float(input, suppress, width, length,
                &arguments);
        } else if (conversion == 'c' || conversion == 's' ||
            conversion == '[') {
            result = wcrt_scan_characters(input, suppress, width, length,
                conversion, set, set_end, invert, &arguments, secure);
        } else {
            return assignments;
        }
        if (result == -2) {
            return completed == 0 ? EOF : assignments;
        }
        if (result < 0) return completed == 0 ? EOF : assignments;
        if (result == 0) return assignments;
        if (result == 1) ++assignments;
        completed = 1;
    }
    return assignments;
}

/** @brief Initializes and runs a string scanner. */
static int wcrt_string_scan(const char *source, const char *format,
    va_list arguments, int secure)
{
    struct wcrt_input input = {source, 0, NULL, 0, 0, 0, 0, 0};
    return wcrt_scan(&input, format, arguments, secure);
}

int vsscanf(const char *source, const char *format, va_list arguments)
{
    return wcrt_string_scan(source, format, arguments, 0);
}

int sscanf(const char *source, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vsscanf(source, format, arguments);
    va_end(arguments);
    return result;
}

int sscanf_s(const char *source, const char *format, ...)
{
    va_list arguments;
    int result;
    if (source == NULL || format == NULL) {
        errno = EINVAL;
        return EOF;
    }
    va_start(arguments, format);
    result = wcrt_string_scan(source, format, arguments, 1);
    va_end(arguments);
    return result;
}

int vfscanf(FILE *stream, const char *format, va_list arguments)
{
    struct wcrt_input input = {NULL, 0, stream, 0, 0, 0, 0, 0};
    int result = wcrt_scan(&input, format, arguments, 0);
    wcrt_input_finish(&input);
    return result;
}

int __wcrt_vfwscanf_c_locale(FILE *stream, const char *format,
    va_list arguments)
{
    struct wcrt_input input = {NULL, 0, stream, 1, 0, 0, 0, 0};
    int result = wcrt_scan(&input, format, arguments, 0);
    wcrt_input_finish(&input);
    return result;
}

int vscanf(const char *format, va_list arguments)
{
    return vfscanf(stdin, format, arguments);
}

int fscanf(FILE *stream, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vfscanf(stream, format, arguments);
    va_end(arguments);
    return result;
}

int scanf(const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vscanf(format, arguments);
    va_end(arguments);
    return result;
}
