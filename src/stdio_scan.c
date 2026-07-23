/**
 * @file stdio_scan.c
 * @brief Implements C89 formatted input for strings and streams.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief Tests membership in a scanf bracket conversion set. */
static int wcrt_in_set(int character, const char *set, const char *end)
{
    while (set < end) {
        if (set + 2 < end && set[1] == '-') {
            if (character >= set[0] && character <= set[2]) return 1;
            set += 3;
        } else if (character == *set++) return 1;
    }
    return 0;
}

/** @brief Scans a terminated source using C89 conversion rules. */
static int wcrt_scan(const char *source, const char *format, va_list arguments)
{
    const char *cursor = source;
    int assignments = 0;
    while (*format != '\0') {
        int suppress = 0, width = 0, length = 0;
        char conversion;
        if (isspace((unsigned char)*format)) {
            while (isspace((unsigned char)*format)) ++format;
            while (isspace((unsigned char)*cursor)) ++cursor;
            continue;
        }
        if (*format != '%') {
            if (*cursor++ != *format++) return assignments;
            continue;
        }
        ++format;
        if (*format == '%') {
            if (*cursor++ != *format++) return assignments;
            continue;
        }
        if (*format == '*') { suppress = 1; ++format; }
        while (isdigit((unsigned char)*format)) {
            width = width * 10 + *format++ - '0';
        }
        if (*format == 'h' || *format == 'l' || *format == 'L') {
            length = *format++;
        }
        conversion = *format++;
        if (conversion != 'c' && conversion != '[' && conversion != 'n') {
            while (isspace((unsigned char)*cursor)) ++cursor;
        }
        if (conversion == 'n') {
            if (!suppress) {
                int consumed = (int)(cursor - source);
                if (length == 'h') *va_arg(arguments, short *) =
                    (short)consumed;
                else if (length == 'l') *va_arg(arguments, long *) = consumed;
                else *va_arg(arguments, int *) = consumed;
            }
            continue;
        }
        if (*cursor == '\0') return assignments == 0 ? EOF : assignments;
        if (conversion == 'd' || conversion == 'i' || conversion == 'o' ||
            conversion == 'u' || conversion == 'x' || conversion == 'p') {
            char *end;
            int base = conversion == 'o' ? 8 :
                (conversion == 'x' || conversion == 'p' ? 16 :
                (conversion == 'i' ? 0 : 10));
            const char *start = cursor;
            unsigned long number = strtoul(cursor, &end, base);
            if (end == cursor || (width > 0 && end - start > width)) {
                return assignments;
            }
            cursor = end;
            if (!suppress) {
                if (conversion == 'p') *va_arg(arguments, void **) =
                    (void *)(size_t)number;
                else if (length == 'h') *va_arg(arguments, unsigned short *) =
                    (unsigned short)number;
                else if (length == 'l') *va_arg(arguments, unsigned long *) =
                    number;
                else *va_arg(arguments, unsigned int *) = (unsigned int)number;
                ++assignments;
            }
        } else if (conversion == 'e' || conversion == 'E' ||
            conversion == 'f' || conversion == 'g' || conversion == 'G') {
            char *end;
            double number = strtod(cursor, &end);
            if (end == cursor || (width > 0 && end - cursor > width)) {
                return assignments;
            }
            cursor = end;
            if (!suppress) {
                if (length == 'l' || length == 'L')
                    *va_arg(arguments, double *) = number;
                else *va_arg(arguments, float *) = (float)number;
                ++assignments;
            }
        } else if (conversion == 'c') {
            int count = width == 0 ? 1 : width;
            char *output = suppress ? NULL : va_arg(arguments, char *);
            int index;
            for (index = 0; index < count; ++index) {
                if (*cursor == '\0') return assignments;
                if (!suppress) output[index] = *cursor;
                ++cursor;
            }
            if (!suppress) ++assignments;
        } else if (conversion == 's' || conversion == '[') {
            char *output = suppress ? NULL : va_arg(arguments, char *);
            int count = 0, invert = 0;
            const char *set = NULL, *set_end = NULL;
            if (conversion == '[') {
                if (*format == '^') { invert = 1; ++format; }
                set = format;
                if (*format == ']') ++format;
                while (*format != '\0' && *format != ']') ++format;
                set_end = format;
                if (*format == ']') ++format;
            }
            while (*cursor != '\0' && (width == 0 || count < width)) {
                int accepted = conversion == 's' ?
                    !isspace((unsigned char)*cursor) :
                    (wcrt_in_set(*cursor, set, set_end) != invert);
                if (!accepted) break;
                if (!suppress) output[count] = *cursor;
                ++cursor; ++count;
            }
            if (count == 0) return assignments;
            if (!suppress) { output[count] = '\0'; ++assignments; }
        } else return assignments;
    }
    return assignments;
}

int sscanf(const char *source, const char *format, ...)
{
    va_list arguments; int result;
    va_start(arguments, format);
    result = wcrt_scan(source, format, arguments);
    va_end(arguments);
    return result;
}

/** @brief Reads a bounded stream segment for the shared scanning engine. */
static int wcrt_stream_scan(FILE *stream, const char *format,
    va_list arguments)
{
    char input[4096];
    int length = 0;
    int character;
    while (length + 1 < (int)sizeof(input) &&
        (character = fgetc(stream)) != EOF) {
        input[length++] = (char)character;
        if (character == '\n') break;
    }
    input[length] = '\0';
    return length == 0 ? EOF : wcrt_scan(input, format, arguments);
}

int fscanf(FILE *stream, const char *format, ...)
{
    va_list arguments; int result;
    va_start(arguments, format);
    result = wcrt_stream_scan(stream, format, arguments);
    va_end(arguments);
    return result;
}

int scanf(const char *format, ...)
{
    va_list arguments; int result;
    va_start(arguments, format);
    result = wcrt_stream_scan(stdin, format, arguments);
    va_end(arguments);
    return result;
}
