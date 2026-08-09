/**
 * @file stdio_format.c
 * @brief Implements C89 and C99 narrow formatted output.
 */

#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief Parsed length modifiers used by the formatting engine. */
enum wcrt_format_length {
    WCRT_LENGTH_NONE,
    WCRT_LENGTH_HH,
    WCRT_LENGTH_H,
    WCRT_LENGTH_L,
    WCRT_LENGTH_LL,
    WCRT_LENGTH_J,
    WCRT_LENGTH_Z,
    WCRT_LENGTH_T,
    WCRT_LENGTH_CAPITAL_L
};

/** @brief Destination state shared by all formatted-output contracts. */
struct wcrt_output {
    FILE *stream;
    char *string;
    size_t count;
    size_t limit;
    int bounded;
    int failed;
};

/** @brief Binary64 representation used for exact classification. */
union wcrt_format_double {
    unsigned long long bits;
    double value;
};

/** @brief Emits one logical formatter byte without crossing a bound. */
static void wcrt_emit(struct wcrt_output *output, int character)
{
    if (output->failed) return;
    if (output->stream != NULL) {
        if (fputc(character, output->stream) == EOF) output->failed = 1;
    } else if (!output->bounded || output->count < output->limit) {
        if (output->string == NULL) output->failed = 1;
        else output->string[output->count] = (char)character;
    }
    if (output->count == (size_t)INT_MAX) output->failed = 1;
    else ++output->count;
}

/** @brief Emits repeated padding bytes. */
static void wcrt_padding(struct wcrt_output *output, int character,
    int count)
{
    while (count-- > 0 && !output->failed) wcrt_emit(output, character);
}

/** @brief Emits a field, preserving a sign or radix prefix before zeroes. */
static void wcrt_field(struct wcrt_output *output, const char *text,
    int length, int width, int left, int zero)
{
    int padding = width > length ? width - length : 0;
    int prefix = 0;
    if (!left && zero && length > 0 &&
        (text[0] == '+' || text[0] == '-' || text[0] == ' ')) {
        prefix = 1;
    }
    if (!left && zero && length > prefix + 1 && text[prefix] == '0' &&
        (text[prefix + 1] == 'x' || text[prefix + 1] == 'X')) {
        prefix += 2;
    }
    if (!left && zero) {
        while (prefix-- > 0) {
            wcrt_emit(output, *text++);
            --length;
        }
    }
    if (!left) wcrt_padding(output, zero ? '0' : ' ', padding);
    while (length-- > 0 && !output->failed) wcrt_emit(output, *text++);
    if (left) wcrt_padding(output, ' ', padding);
}

/** @brief Converts an unsigned integer into forward radix digits. */
static int wcrt_unsigned(char *result, unsigned long long value, int base,
    int upper)
{
    const char *digits = upper ? "0123456789ABCDEF" :
        "0123456789abcdef";
    char reversed[65];
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

/** @brief Appends a signed decimal exponent with at least two digits. */
static int wcrt_exponent(char *result, int exponent, int upper)
{
    char digits[16];
    int count;
    int length = 0;
    result[length++] = upper ? 'E' : 'e';
    result[length++] = exponent < 0 ? '-' : '+';
    if (exponent < 0) exponent = -exponent;
    count = wcrt_unsigned(digits, (unsigned int)exponent, 10, 0);
    if (count < 2) result[length++] = '0';
    memcpy(result + length, digits, (size_t)count);
    return length + count;
}

/** @brief Normalizes a positive finite value to the interval [1, 10). */
static double wcrt_decimal_normalize(double value, int *exponent)
{
    *exponent = 0;
    while (value >= 10.0) {
        value /= 10.0;
        ++*exponent;
    }
    while (value < 1.0) {
        value *= 10.0;
        --*exponent;
    }
    return value;
}

/** @brief Produces and rounds a requested number of decimal digits. */
static int wcrt_decimal_digits(char *digits, int count, double normalized,
    int *exponent)
{
    int index;
    int guard;
    int sticky;
    for (index = 0; index < count; ++index) {
        int digit = (int)normalized;
        if (digit > 9) digit = 9;
        digits[index] = (char)('0' + digit);
        normalized = (normalized - (double)digit) * 10.0;
    }
    guard = (int)normalized;
    normalized = (normalized - (double)guard) * 10.0;
    sticky = normalized != 0.0;
    if (guard > 5 || (guard == 5 &&
        (sticky || (count > 0 && ((digits[count - 1] - '0') & 1))))) {
        index = count - 1;
        while (index >= 0 && digits[index] == '9') {
            digits[index--] = '0';
        }
        if (index >= 0) {
            ++digits[index];
        } else {
            memmove(digits + 1, digits, (size_t)count);
            digits[0] = '1';
            ++*exponent;
            return count + 1;
        }
    }
    return count;
}

/** @brief Formats finite absolute binary64 in fixed decimal notation. */
static int wcrt_decimal_fixed(char *result, double value, int precision,
    int alternate, int trim)
{
    char *digits;
    double normalized = 0.0;
    int exponent = 0;
    int count;
    int position = 0;
    int decimal_position = -1;
    int power;
    int rounded_up = 0;

    if (value != 0.0) normalized = wcrt_decimal_normalize(value, &exponent);
    count = value == 0.0 ? 0 : exponent + precision + 1;
    if (count < 0) count = 0;
    digits = (char *)malloc((size_t)count + 2U);
    if (digits == NULL) return -1;
    if (count > 0) {
        count = wcrt_decimal_digits(digits, count, normalized, &exponent);
    } else if (value != 0.0 && exponent + precision == -1 &&
        normalized > 5.0) {
        rounded_up = 1;
    }

    if (exponent < 0) {
        result[position++] = '0';
    } else {
        for (power = exponent; power >= 0; --power) {
            int index = exponent - power;
            result[position++] = index < count ? digits[index] : '0';
        }
    }
    if (precision > 0 || alternate) {
        decimal_position = position;
        result[position++] = '.';
    }
    for (power = -1; power >= -precision; --power) {
        int index = exponent - power;
        char digit = index >= 0 && index < count ? digits[index] : '0';
        if (rounded_up && power == -precision) digit = '1';
        result[position++] = digit;
    }
    if (trim && !alternate && decimal_position >= 0) {
        while (position > decimal_position + 1 &&
            result[position - 1] == '0') {
            --position;
        }
        if (position == decimal_position + 1) --position;
    }
    result[position] = '\0';
    free(digits);
    return position;
}

/** @brief Formats finite absolute binary64 in exponent notation. */
static int wcrt_decimal_scientific(char *result, double value,
    int precision, int alternate, int upper, int trim)
{
    char *digits;
    double normalized = 0.0;
    int exponent = 0;
    int count = precision + 1;
    int position = 0;
    int index;

    if (value != 0.0) normalized = wcrt_decimal_normalize(value, &exponent);
    digits = (char *)malloc((size_t)count + 2U);
    if (digits == NULL) return -1;
    if (value == 0.0) {
        memset(digits, '0', (size_t)count);
    } else {
        count = wcrt_decimal_digits(digits, count, normalized, &exponent);
    }
    result[position++] = digits[0];
    if (precision > 0 || alternate) result[position++] = '.';
    for (index = 1; index <= precision; ++index) {
        result[position++] = index < count ? digits[index] : '0';
    }
    if (trim && !alternate) {
        while (position > 0 && result[position - 1] == '0') --position;
        if (position > 0 && result[position - 1] == '.') --position;
    }
    position += wcrt_exponent(result + position, exponent, upper);
    result[position] = '\0';
    free(digits);
    return position;
}

/** @brief Formats finite or special decimal floating output. */
static int wcrt_decimal_float(char *result, double value, int precision,
    int alternate, int upper, char conversion)
{
    union wcrt_format_double represented;
    unsigned long long magnitude;
    int exponent = 0;
    int position = 0;
    int trim = 0;
    represented.value = value;
    magnitude = represented.bits & 0x7fffffffffffffffULL;
    if ((magnitude & 0x7ff0000000000000ULL) ==
        0x7ff0000000000000ULL) {
        const char *word = (magnitude & 0xfffffffffffffULL) != 0 ?
            (upper ? "NAN" : "nan") : (upper ? "INF" : "inf");
        strcpy(result, word);
        return 3;
    }
    if (precision < 0) precision = 6;
    if (conversion == 'g' || conversion == 'G') {
        if (precision == 0) precision = 1;
        if (magnitude != 0) {
            (void)wcrt_decimal_normalize(value, &exponent);
        }
        trim = 1;
        if (exponent < -4 || exponent >= precision) {
            position = wcrt_decimal_scientific(result, value,
                precision - 1, alternate, upper, trim);
        } else {
            position = wcrt_decimal_fixed(result, value,
                precision - exponent - 1, alternate, trim);
        }
    } else if (conversion == 'e' || conversion == 'E') {
        position = wcrt_decimal_scientific(result, value, precision,
            alternate, upper, 0);
    } else {
        position = wcrt_decimal_fixed(result, value, precision, alternate, 0);
    }
    return position;
}

/** @brief Formats exact hexadecimal floating output. */
static int wcrt_hexadecimal_float(char *result, double value, int precision,
    int alternate, int upper)
{
    const char *hex = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    union wcrt_format_double represented;
    unsigned long long magnitude;
    unsigned long long significand;
    unsigned int exponent_field;
    int exponent;
    int digits;
    int requested = precision;
    int position = 0;
    int index;

    represented.value = value;
    magnitude = represented.bits & 0x7fffffffffffffffULL;
    if ((magnitude & 0x7ff0000000000000ULL) ==
        0x7ff0000000000000ULL) {
        const char *word = (magnitude & 0xfffffffffffffULL) != 0 ?
            (upper ? "NAN" : "nan") : (upper ? "INF" : "inf");
        strcpy(result, word);
        return 3;
    }
    exponent_field = (unsigned int)(magnitude >> 52);
    significand = magnitude & 0xfffffffffffffULL;
    exponent = exponent_field == 0 ? -1022 : (int)exponent_field - 1023;
    if (exponent_field != 0) significand |= 1ULL << 52;
    if (magnitude == 0) exponent = 0;

    if (requested < 0) {
        digits = 13;
        while (digits > 0 &&
            ((significand >> (52 - digits * 4)) & 15ULL) == 0) {
            --digits;
        }
    } else {
        digits = requested;
        if (digits < 13) {
            int shift = 52 - digits * 4;
            unsigned long long discarded = significand &
                ((1ULL << shift) - 1ULL);
            unsigned long long halfway = 1ULL << (shift - 1);
            unsigned long long retained = significand >> shift;
            if (discarded > halfway ||
                (discarded == halfway && (retained & 1ULL) != 0)) {
                ++retained;
            }
            significand = retained << shift;
            if (significand == (1ULL << 53)) {
                significand >>= 1;
                ++exponent;
            }
        }
    }

    result[position++] = '0';
    result[position++] = upper ? 'X' : 'x';
    result[position++] = hex[(significand >> 52) & 15ULL];
    if (digits > 0 || alternate) result[position++] = '.';
    for (index = 0; index < digits; ++index) {
        unsigned int digit = index < 13 ?
            (unsigned int)((significand >> (48 - index * 4)) & 15ULL) :
            0U;
        result[position++] = hex[digit];
    }
    result[position++] = upper ? 'P' : 'p';
    result[position++] = exponent < 0 ? '-' : '+';
    if (exponent < 0) exponent = -exponent;
    position += wcrt_unsigned(result + position,
        (unsigned int)exponent, 10, 0);
    return position;
}

/** @brief Returns a signed argument using its exact promoted type. */
static long long wcrt_signed_argument(va_list *arguments,
    enum wcrt_format_length length)
{
    if (length == WCRT_LENGTH_L) return va_arg(*arguments, long);
    if (length == WCRT_LENGTH_LL || length == WCRT_LENGTH_J) {
        return va_arg(*arguments, long long);
    }
    if (length == WCRT_LENGTH_Z || length == WCRT_LENGTH_T) {
        return va_arg(*arguments, ptrdiff_t);
    }
    if (length == WCRT_LENGTH_HH) {
        return (signed char)va_arg(*arguments, int);
    }
    if (length == WCRT_LENGTH_H) return (short)va_arg(*arguments, int);
    return va_arg(*arguments, int);
}

/** @brief Returns an unsigned argument using its exact promoted type. */
static unsigned long long wcrt_unsigned_argument(va_list *arguments,
    enum wcrt_format_length length)
{
    if (length == WCRT_LENGTH_L) return va_arg(*arguments, unsigned long);
    if (length == WCRT_LENGTH_LL || length == WCRT_LENGTH_J) {
        return va_arg(*arguments, unsigned long long);
    }
    if (length == WCRT_LENGTH_Z || length == WCRT_LENGTH_T) {
        return va_arg(*arguments, size_t);
    }
    if (length == WCRT_LENGTH_HH) {
        return (unsigned char)va_arg(*arguments, int);
    }
    if (length == WCRT_LENGTH_H) {
        return (unsigned short)va_arg(*arguments, int);
    }
    return va_arg(*arguments, unsigned int);
}

/** @brief Stores the logical output count for a percent-n directive. */
static void wcrt_store_count(va_list *arguments,
    enum wcrt_format_length length, size_t count)
{
    if (length == WCRT_LENGTH_HH) {
        *va_arg(*arguments, signed char *) = (signed char)count;
    } else if (length == WCRT_LENGTH_H) {
        *va_arg(*arguments, short *) = (short)count;
    } else if (length == WCRT_LENGTH_L) {
        *va_arg(*arguments, long *) = (long)count;
    } else if (length == WCRT_LENGTH_LL) {
        *va_arg(*arguments, long long *) = (long long)count;
    } else if (length == WCRT_LENGTH_J) {
        *va_arg(*arguments, intmax_t *) = (intmax_t)count;
    } else if (length == WCRT_LENGTH_Z || length == WCRT_LENGTH_T) {
        *va_arg(*arguments, ptrdiff_t *) = (ptrdiff_t)count;
    } else {
        *va_arg(*arguments, int *) = (int)count;
    }
}

/** @brief Parses a decimal width or precision with checked saturation. */
static int wcrt_format_number(const char **format)
{
    int result = 0;
    while (**format >= '0' && **format <= '9') {
        int digit = *(*format)++ - '0';
        if (result > (INT_MAX - digit) / 10) result = INT_MAX;
        else result = result * 10 + digit;
    }
    return result;
}

/** @brief Implements the shared formatted-output directive parser. */
static int wcrt_format(struct wcrt_output *output, const char *format,
    va_list arguments)
{
    while (*format != '\0' && !output->failed) {
        int left = 0, plus = 0, space = 0, alternate = 0, zero = 0;
        int width = 0, precision = -1;
        enum wcrt_format_length length = WCRT_LENGTH_NONE;
        char conversion;
        char value[96];
        char *field_value = value;
        char *allocated = NULL;
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
            if (width < 0) {
                left = 1;
                width = width == INT_MIN ? INT_MAX : -width;
            }
            ++format;
        } else {
            width = wcrt_format_number(&format);
        }
        if (*format == '.') {
            ++format;
            if (*format == '*') {
                precision = va_arg(arguments, int);
                if (precision < 0) precision = -1;
                ++format;
            } else {
                precision = wcrt_format_number(&format);
            }
        }
        if (format[0] == 'h' && format[1] == 'h') {
            length = WCRT_LENGTH_HH;
            format += 2;
        } else if (*format == 'h') {
            length = WCRT_LENGTH_H;
            ++format;
        } else if (format[0] == 'l' && format[1] == 'l') {
            length = WCRT_LENGTH_LL;
            format += 2;
        } else if (*format == 'l') {
            length = WCRT_LENGTH_L;
            ++format;
        } else if (*format == 'j') {
            length = WCRT_LENGTH_J;
            ++format;
        } else if (*format == 'z') {
            length = WCRT_LENGTH_Z;
            ++format;
        } else if (*format == 't') {
            length = WCRT_LENGTH_T;
            ++format;
        } else if (*format == 'L') {
            length = WCRT_LENGTH_CAPITAL_L;
            ++format;
        }
        conversion = *format;
        if (conversion == '\0') return -1;
        ++format;

        if ((conversion == 'd' || conversion == 'i' ||
            conversion == 'u' || conversion == 'o' ||
            conversion == 'x' || conversion == 'X' ||
            conversion == 'p') && precision > 64) {
            if (precision > INT_MAX - 70) return -1;
            allocated = (char *)malloc((size_t)precision + 70U);
            if (allocated == NULL) return -1;
            field_value = allocated;
        }

        if (conversion == 'd' || conversion == 'i') {
            long long signed_value = wcrt_signed_argument(&arguments, length);
            unsigned long long magnitude;
            char sign = 0;
            char digits[65];
            int digit_count;
            if (signed_value < 0) {
                sign = '-';
                magnitude = 0ULL - (unsigned long long)signed_value;
            } else {
                sign = plus ? '+' : (space ? ' ' : 0);
                magnitude = (unsigned long long)signed_value;
            }
            digit_count = magnitude == 0 && precision == 0 ? 0 :
                wcrt_unsigned(digits, magnitude, 10, 0);
            if (sign != 0) field_value[value_length++] = sign;
            while (precision > digit_count) {
                field_value[value_length++] = '0';
                --precision;
            }
            memcpy(field_value + value_length, digits, (size_t)digit_count);
            value_length += digit_count;
            if (precision >= 0) zero = 0;
        } else if (conversion == 'u' || conversion == 'o' ||
            conversion == 'x' || conversion == 'X' || conversion == 'p') {
            unsigned long long number;
            char digits[65];
            int base = conversion == 'o' ? 8 :
                (conversion == 'u' ? 10 : 16);
            int digit_count;
            if (conversion == 'p') {
                number = (unsigned long long)(size_t)
                    va_arg(arguments, void *);
            } else {
                number = wcrt_unsigned_argument(&arguments, length);
            }
            digit_count = number == 0 && precision == 0 ? 0 :
                wcrt_unsigned(digits, number, base, conversion == 'X');
            if ((conversion == 'p' || (alternate && number != 0)) &&
                base == 16) {
                field_value[value_length++] = '0';
                field_value[value_length++] = conversion == 'X' ? 'X' : 'x';
            }
            if (alternate && base == 8 &&
                (digit_count == 0 || digits[0] != '0')) {
                field_value[value_length++] = '0';
            }
            while (precision > digit_count) {
                field_value[value_length++] = '0';
                --precision;
            }
            memcpy(field_value + value_length, digits,
                (size_t)digit_count);
            value_length += digit_count;
            if (precision >= 0) zero = 0;
        } else if (conversion == 'c') {
            wint_t character = length == WCRT_LENGTH_L ?
                va_arg(arguments, wint_t) : (wint_t)va_arg(arguments, int);
            if (length == WCRT_LENGTH_L && character > 0xffU) {
                errno = EILSEQ;
                output->failed = 1;
            } else {
                value[0] = (char)(unsigned char)character;
                value_length = 1;
            }
        } else if (conversion == 's') {
            if (length == WCRT_LENGTH_L) {
                const wchar_t *wide = va_arg(arguments, const wchar_t *);
                int wide_length = 0;
                if (wide == NULL) {
                    wcrt_field(output, "(null)", 6, width, left, 0);
                    continue;
                }
                while (wide[wide_length] != 0 &&
                    (precision < 0 || wide_length < precision)) {
                    if ((unsigned int)wide[wide_length] > 0xffU) {
                        errno = EILSEQ;
                        output->failed = 1;
                        break;
                    }
                    ++wide_length;
                }
                if (!output->failed) {
                    int padding = width > wide_length ?
                        width - wide_length : 0;
                    int index;
                    if (!left) wcrt_padding(output, ' ', padding);
                    for (index = 0; index < wide_length; ++index) {
                        wcrt_emit(output, (char)wide[index]);
                    }
                    if (left) wcrt_padding(output, ' ', padding);
                }
                continue;
            } else {
                const char *string = va_arg(arguments, const char *);
                size_t string_length;
                if (string == NULL) string = "(null)";
                string_length = strlen(string);
                if (string_length > (size_t)INT_MAX) return -1;
                value_length = (int)string_length;
                if (precision >= 0 && value_length > precision) {
                    value_length = precision;
                }
                wcrt_field(output, string, value_length, width, left, 0);
                continue;
            }
        } else if (conversion == 'f' || conversion == 'F' ||
            conversion == 'e' || conversion == 'E' ||
            conversion == 'g' || conversion == 'G' ||
            conversion == 'a' || conversion == 'A') {
            union wcrt_format_double represented;
            double number = length == WCRT_LENGTH_CAPITAL_L ?
                (double)va_arg(arguments, long double) :
                va_arg(arguments, double);
            unsigned long long magnitude;
            int upper = conversion == 'F' || conversion == 'E' ||
                conversion == 'G' || conversion == 'A';
            size_t capacity;
            represented.value = number;
            magnitude = represented.bits & 0x7fffffffffffffffULL;
            if (precision > INT_MAX - 400) return -1;
            capacity = (size_t)(precision < 0 ? 16 : precision) + 400U;
            allocated = (char *)malloc(capacity);
            if (allocated == NULL) return -1;
            if ((represented.bits & 0x8000000000000000ULL) != 0) {
                allocated[value_length++] = '-';
            } else if (plus) {
                allocated[value_length++] = '+';
            } else if (space) {
                allocated[value_length++] = ' ';
            }
            represented.bits = magnitude;
            number = represented.value;
            if ((magnitude & 0x7ff0000000000000ULL) ==
                0x7ff0000000000000ULL) {
                zero = 0;
            }
            if (conversion == 'a' || conversion == 'A') {
                value_length += wcrt_hexadecimal_float(
                    allocated + value_length, number, precision,
                    alternate, upper);
            } else {
                value_length += wcrt_decimal_float(
                    allocated + value_length, number, precision,
                    alternate, upper, conversion);
            }
            wcrt_field(output, allocated, value_length, width, left, zero);
            free(allocated);
            continue;
        } else if (conversion == 'n') {
            wcrt_store_count(&arguments, length, output->count);
            continue;
        } else if (conversion == '%') {
            value[0] = '%';
            value_length = 1;
        } else {
            return -1;
        }
        wcrt_field(output, field_value, value_length, width, left,
            zero && !left);
        if (allocated != NULL) free(allocated);
    }
    if (output->string != NULL && !output->bounded && !output->failed) {
        output->string[output->count] = '\0';
    }
    return output->failed ? -1 : (int)output->count;
}

int vfprintf(FILE *stream, const char *format, va_list arguments)
{
    struct wcrt_output output = {stream, NULL, 0, 0, 0, 0};
    return wcrt_format(&output, format, arguments);
}

int vprintf(const char *format, va_list arguments)
{
    return vfprintf(stdout, format, arguments);
}

int vsprintf(char *destination, const char *format, va_list arguments)
{
    struct wcrt_output output = {NULL, destination, 0, 0, 0, 0};
    return wcrt_format(&output, format, arguments);
}

int vsnprintf(char *destination, size_t size, const char *format,
    va_list arguments)
{
    struct wcrt_output output = {
        NULL, destination, 0, size > 0 ? size - 1 : 0, 1, 0
    };
    int result = wcrt_format(&output, format, arguments);
    if (size > 0 && destination != NULL) {
        size_t end = output.count < size ? output.count : size - 1;
        destination[end] = '\0';
    }
    return result;
}

int _vsnprintf(char *destination, size_t size, const char *format,
    va_list arguments)
{
    struct wcrt_output output;
    int result;
    if (destination != NULL && size == 0) return -1;
    output.stream = NULL;
    output.string = destination;
    output.count = 0;
    output.limit = size;
    output.bounded = 1;
    output.failed = 0;
    result = wcrt_format(&output, format, arguments);
    if (destination == NULL && size == 0) return result;
    if (result >= 0 && (size_t)result < size) destination[result] = '\0';
    if (result >= 0 && (size_t)result > size) return -1;
    return result;
}

int fprintf(FILE *stream, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vfprintf(stream, format, arguments);
    va_end(arguments);
    return result;
}

int printf(const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vprintf(format, arguments);
    va_end(arguments);
    return result;
}

int sprintf(char *destination, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vsprintf(destination, format, arguments);
    va_end(arguments);
    return result;
}

int snprintf(char *destination, size_t size, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vsnprintf(destination, size, format, arguments);
    va_end(arguments);
    return result;
}

int _snprintf(char *destination, size_t size, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = _vsnprintf(destination, size, format, arguments);
    va_end(arguments);
    return result;
}
