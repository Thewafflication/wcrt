/**
 * @file stdio_format.c
 * @brief Implements C89 formatted output for streams and strings.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/** @brief Destination state shared by the formatting engine. */
struct wcrt_output {
    FILE *stream;
    char *string;
    int count;
    int failed;
};

/** @brief Emits one formatter byte. */
static void wcrt_emit(struct wcrt_output *output, int character)
{
    if (output->stream != NULL) {
        if (fputc(character, output->stream) == EOF) output->failed = 1;
    } else {
        output->string[output->count] = (char)character;
    }
    ++output->count;
}

/** @brief Emits padding followed or preceded by supplied text. */
static void wcrt_field(struct wcrt_output *output, const char *text,
    int length, int width, int left, int zero)
{
    int padding = width > length ? width - length : 0;
    if (!left && zero && length > 0 &&
        (text[0] == '+' || text[0] == '-' || text[0] == ' ')) {
        wcrt_emit(output, *text++);
        --length;
    } else if (!left && zero && length > 1 && text[0] == '0' &&
        (text[1] == 'x' || text[1] == 'X')) {
        wcrt_emit(output, *text++);
        wcrt_emit(output, *text++);
        length -= 2;
    }
    if (!left) while (padding-- > 0) wcrt_emit(output, zero ? '0' : ' ');
    while (length-- > 0) wcrt_emit(output, *text++);
    if (left) while (padding-- > 0) wcrt_emit(output, ' ');
}

/** @brief Converts an unsigned integer into reversed radix digits. */
static int wcrt_unsigned(char *result, unsigned long long value, int base,
    int upper)
{
    const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    char reversed[32];
    int count = 0;
    int index;
    do {
        reversed[count++] = digits[value % (unsigned int)base];
        value /= (unsigned int)base;
    } while (value != 0);
    for (index = 0; index < count; ++index) {
        result[index] = reversed[count - index - 1];
    }
    result[count] = '\0';
    return count;
}

/** @brief Formats a finite floating value in fixed notation. */
static int wcrt_fixed(char *result, double value, int precision)
{
    unsigned long long whole;
    double fraction;
    char integer[32];
    int length = 0;
    int index;
    int negative = value < 0.0;
    if (negative) {
        result[length++] = '-';
        value = -value;
    }
    whole = (unsigned long long)value;
    fraction = value - (double)whole;
    wcrt_unsigned(integer, whole, 10, 0);
    strcpy(result + (negative ? 1 : 0), integer);
    length = (negative ? 1 : 0) + (int)strlen(integer);
    if (precision > 0) result[length++] = '.';
    for (index = 0; index < precision; ++index) {
        fraction *= 10.0;
        result[length++] = (char)('0' + (int)fraction % 10);
    }
    result[length] = '\0';
    return length;
}

/** @brief Formats using a shared C89 conversion parser. */
static int wcrt_format(struct wcrt_output *output, const char *format,
    va_list arguments)
{
    while (*format != '\0') {
        int left = 0, plus = 0, space = 0, alternate = 0, zero = 0;
        int width = 0, precision = -1, length_modifier = 0;
        char conversion;
        char value[128];
        int value_length = 0;
        if (*format != '%') {
            wcrt_emit(output, *format++);
            continue;
        }
        ++format;
        for (;;) {
            if (*format == '-') left = 1;
            else if (*format == '+') plus = 1;
            else if (*format == ' ') space = 1;
            else if (*format == '#') alternate = 1;
            else if (*format == '0') zero = 1;
            else break;
            ++format;
        }
        if (*format == '*') {
            width = va_arg(arguments, int);
            if (width < 0) { left = 1; width = -width; }
            ++format;
        } else while (*format >= '0' && *format <= '9') {
            width = width * 10 + *format++ - '0';
        }
        if (*format == '.') {
            ++format;
            precision = 0;
            if (*format == '*') {
                precision = va_arg(arguments, int);
                ++format;
            } else while (*format >= '0' && *format <= '9') {
                precision = precision * 10 + *format++ - '0';
            }
        }
        if (*format == 'h' || *format == 'l' || *format == 'L') {
            length_modifier = *format++;
        }
        conversion = *format++;
        if (conversion == 'd' || conversion == 'i') {
            long long signed_value = length_modifier == 'l' ?
                va_arg(arguments, long) : va_arg(arguments, int);
            unsigned long long magnitude;
            char sign = 0;
            if (signed_value < 0) {
                sign = '-';
                magnitude = 0ULL - (unsigned long long)signed_value;
            } else {
                sign = plus ? '+' : (space ? ' ' : 0);
                magnitude = (unsigned long long)signed_value;
            }
            if (sign) value[value_length++] = sign;
            value_length += wcrt_unsigned(value + value_length, magnitude,
                10, 0);
        } else if (conversion == 'u' || conversion == 'o' ||
            conversion == 'x' || conversion == 'X' || conversion == 'p') {
            unsigned long long number;
            int base = conversion == 'o' ? 8 :
                (conversion == 'u' ? 10 : 16);
            if (conversion == 'p') number =
                (unsigned long long)(size_t)va_arg(arguments, void *);
            else number = length_modifier == 'l' ?
                va_arg(arguments, unsigned long) :
                va_arg(arguments, unsigned int);
            if ((alternate || conversion == 'p') && base == 16) {
                value[value_length++] = '0';
                value[value_length++] = conversion == 'X' ? 'X' : 'x';
            } else if (alternate && base == 8) value[value_length++] = '0';
            value_length += wcrt_unsigned(value + value_length, number, base,
                conversion == 'X');
        } else if (conversion == 'c') {
            value[0] = (char)va_arg(arguments, int);
            value[1] = '\0';
            value_length = 1;
        } else if (conversion == 's') {
            const char *string = va_arg(arguments, const char *);
            value_length = (int)strlen(string == NULL ? "(null)" : string);
            if (precision >= 0 && value_length > precision) {
                value_length = precision;
            }
            wcrt_field(output, string == NULL ? "(null)" : string,
                value_length, width, left, 0);
            continue;
        } else if (conversion == 'f' || conversion == 'e' ||
            conversion == 'E' || conversion == 'g' || conversion == 'G') {
            double number = va_arg(arguments, double);
            int exponent = 0;
            if (precision < 0) precision = 6;
            if (conversion != 'f' && number != 0.0) {
                double absolute = number < 0.0 ? -number : number;
                while (absolute >= 10.0) { number /= 10.0; absolute /= 10.0;
                    ++exponent; }
                while (absolute < 1.0) { number *= 10.0; absolute *= 10.0;
                    --exponent; }
            }
            value_length = wcrt_fixed(value, number, precision);
            if (conversion == 'e' || conversion == 'E') {
                value[value_length++] = conversion;
                value[value_length++] = exponent < 0 ? '-' : '+';
                if (exponent < 0) exponent = -exponent;
                value[value_length++] = (char)('0' + exponent / 10 % 10);
                value[value_length++] = (char)('0' + exponent % 10);
                value[value_length] = '\0';
            }
        } else if (conversion == 'n') {
            if (length_modifier == 'h') *va_arg(arguments, short *) =
                (short)output->count;
            else if (length_modifier == 'l') *va_arg(arguments, long *) =
                output->count;
            else *va_arg(arguments, int *) = output->count;
            continue;
        } else if (conversion == '%') {
            value[0] = '%'; value[1] = '\0'; value_length = 1;
        } else return -1;
        wcrt_field(output, value, value_length, width, left, zero);
    }
    if (output->string != NULL) output->string[output->count] = '\0';
    return output->failed ? -1 : output->count;
}

int vfprintf(FILE *stream, const char *format, va_list arguments)
{
    struct wcrt_output output = {stream, NULL, 0, 0};
    return wcrt_format(&output, format, arguments);
}

int vprintf(const char *format, va_list arguments)
{
    return vfprintf(stdout, format, arguments);
}

int vsprintf(char *destination, const char *format, va_list arguments)
{
    struct wcrt_output output = {NULL, destination, 0, 0};
    return wcrt_format(&output, format, arguments);
}

int fprintf(FILE *stream, const char *format, ...)
{
    va_list arguments; int result;
    va_start(arguments, format);
    result = vfprintf(stream, format, arguments);
    va_end(arguments);
    return result;
}

int printf(const char *format, ...)
{
    va_list arguments; int result;
    va_start(arguments, format);
    result = vprintf(format, arguments);
    va_end(arguments);
    return result;
}

int sprintf(char *destination, const char *format, ...)
{
    va_list arguments; int result;
    va_start(arguments, format);
    result = vsprintf(destination, format, arguments);
    va_end(arguments);
    return result;
}
