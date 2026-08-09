/**
 * @file stdlib.c
 * @brief Implements C89 conversions and general utilities.
 */

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "internal/stdlib.h"

#define WCRT_ATEXIT_MAX 32

/** @brief Registered normal-termination callbacks. */
static void (*wcrt_exit_functions[WCRT_ATEXIT_MAX])(void);
/** @brief Number of registered callbacks. */
static unsigned int wcrt_exit_function_count;
/** @brief Pseudo-random sequence state. */
static unsigned long wcrt_random_state = 1;

/**
 * @brief Converts one ASCII digit for a candidate base.
 * @param character Candidate input byte.
 * @return Digit value, or -1 when not alphanumeric.
 */
static int wcrt_digit(int character)
{
    if (character >= '0' && character <= '9') {
        return character - '0';
    }
    if (character >= 'a' && character <= 'z') {
        return character - 'a' + 10;
    }
    if (character >= 'A' && character <= 'Z') {
        return character - 'A' + 10;
    }
    return -1;
}

/** @brief Converts a subject sequence to an unsigned long-long magnitude. */
static unsigned long long wcrt_strtoull(const char *string,
    char **end_pointer, int base, int *was_negative, int *did_overflow)
{
    const char *cursor = string;
    unsigned long long value = 0;
    int negative = 0;
    int converted = 0;
    int overflow = 0;

    if (did_overflow != NULL) {
        *did_overflow = 0;
    }

    while (isspace((unsigned char)*cursor)) {
        ++cursor;
    }
    if (*cursor == '+' || *cursor == '-') {
        negative = *cursor++ == '-';
    }
    if ((base == 0 || base == 16) && cursor[0] == '0' &&
        (cursor[1] == 'x' || cursor[1] == 'X') &&
        wcrt_digit((unsigned char)cursor[2]) >= 0 &&
        wcrt_digit((unsigned char)cursor[2]) < 16) {
        base = 16;
        cursor += 2;
    } else if (base == 0) {
        base = *cursor == '0' ? 8 : 10;
    }
    if (base < 2 || base > 36) {
        if (end_pointer != NULL) {
            *end_pointer = (char *)string;
        }
        if (was_negative != NULL) {
            *was_negative = negative;
        }
        return 0;
    }
    while (wcrt_digit((unsigned char)*cursor) >= 0 &&
        wcrt_digit((unsigned char)*cursor) < base) {
        int digit = wcrt_digit((unsigned char)*cursor++);
        converted = 1;
        if (value > (18446744073709551615ULL -
            (unsigned long long)digit) / (unsigned long long)base) {
            overflow = 1;
        } else if (!overflow) {
            value = value * (unsigned long long)base +
                (unsigned long long)digit;
        }
    }
    if (end_pointer != NULL) {
        *end_pointer = (char *)(converted ? cursor : string);
    }
    if (was_negative != NULL) {
        *was_negative = negative;
    }
    if (did_overflow != NULL) {
        *did_overflow = overflow;
    }
    if (overflow) {
        errno = ERANGE;
        return 18446744073709551615ULL;
    }
    return negative ? 0ULL - value : value;
}

unsigned long strtoul(const char *string, char **end_pointer, int base)
{
    const char *cursor = string;
    const char *digits;
    unsigned long value = 0;
    int negative = 0;
    int converted = 0;
    int overflow = 0;

    while (isspace((unsigned char)*cursor)) {
        ++cursor;
    }
    if (*cursor == '+' || *cursor == '-') {
        negative = *cursor++ == '-';
    }
    if ((base == 0 || base == 16) && cursor[0] == '0' &&
        (cursor[1] == 'x' || cursor[1] == 'X') &&
        wcrt_digit((unsigned char)cursor[2]) >= 0 &&
        wcrt_digit((unsigned char)cursor[2]) < 16) {
        base = 16;
        cursor += 2;
    } else if (base == 0) {
        base = *cursor == '0' ? 8 : 10;
    }
    if (base < 2 || base > 36) {
        if (end_pointer != NULL) {
            *end_pointer = (char *)string;
        }
        return 0;
    }
    digits = cursor;
    while (wcrt_digit((unsigned char)*cursor) >= 0 &&
        wcrt_digit((unsigned char)*cursor) < base) {
        int digit = wcrt_digit((unsigned char)*cursor++);
        converted = 1;
        if (value > (ULONG_MAX - (unsigned long)digit) /
            (unsigned long)base) {
            overflow = 1;
        } else if (!overflow) {
            value = value * (unsigned long)base + (unsigned long)digit;
        }
    }
    if (end_pointer != NULL) {
        *end_pointer = (char *)(converted ? cursor : string);
    }
    if (overflow) {
        errno = ERANGE;
        return ULONG_MAX;
    }
    (void)digits;
    return negative ? 0UL - value : value;
}

long strtol(const char *string, char **end_pointer, int base)
{
    const char *cursor = string;
    char *end;
    int negative;
    unsigned long magnitude;

    while (isspace((unsigned char)*cursor)) {
        ++cursor;
    }
    negative = *cursor == '-';
    magnitude = strtoul(string, &end, base);
    if (end_pointer != NULL) {
        *end_pointer = end;
    }
    if (end == string) {
        return 0;
    }
    if (negative) {
        magnitude = 0UL - magnitude;
        if (magnitude > (unsigned long)LONG_MAX + 1UL) {
            errno = ERANGE;
            return LONG_MIN;
        }
        if (magnitude == (unsigned long)LONG_MAX + 1UL) {
            return LONG_MIN;
        }
        return -(long)magnitude;
    }
    if (magnitude > (unsigned long)LONG_MAX) {
        errno = ERANGE;
        return LONG_MAX;
    }
    return (long)magnitude;
}

/** @brief Scales a decimal significand while limiting intermediate rounding. */
static double wcrt_scale_decimal(double value, int scale, int *did_overflow)
{
    static const double powers[] = {
        1.0e1, 1.0e2, 1.0e4, 1.0e8, 1.0e16,
        1.0e32, 1.0e64, 1.0e128, 1.0e256
    };
    double factor;
    double limit;
    unsigned int magnitude;
    unsigned int bit = 0;

    *did_overflow = 0;
    if (value == 0.0) {
        return 0.0;
    }
    magnitude = (unsigned int)(scale < 0 ? -scale : scale);
    while (magnitude != 0) {
        if ((magnitude & 1U) != 0) {
            if (bit >= sizeof(powers) / sizeof(powers[0])) {
                if (scale > 0) {
                    *did_overflow = 1;
                    return DBL_MAX;
                }
                return 0.0;
            }
            factor = powers[bit];
            if (scale > 0) {
                limit = DBL_MAX / factor;
                if (value > limit) {
                    if (value - limit <= limit * DBL_EPSILON * 2.0) {
                        return DBL_MAX;
                    }
                    *did_overflow = 1;
                    return DBL_MAX;
                }
                value *= factor;
            } else {
                value /= factor;
            }
        }
        magnitude >>= 1;
        ++bit;
    }
    return value;
}

/** @brief Binary64 representation used by numeric-text conversion. */
union wcrt_double_representation {
    unsigned long long bits;
    double value;
};

/** @brief Binary32 representation used by numeric-text conversion. */
union wcrt_float_representation {
    unsigned int bits;
    float value;
};

/** @brief Tests an ASCII character without locale-dependent case folding. */
static int wcrt_ascii_equal(int character, int lower)
{
    if (character >= 'A' && character <= 'Z') {
        character += 'a' - 'A';
    }
    return character == lower;
}

/** @brief Matches one ASCII word without changing the supplied cursor. */
static int wcrt_ascii_word(const char *cursor, const char *word)
{
    while (*word != '\0') {
        if (!wcrt_ascii_equal((unsigned char)*cursor++, *word++)) {
            return 0;
        }
    }
    return 1;
}

/** @brief Returns the most significant set-bit index in a hexadecimal digit. */
static int wcrt_hexadecimal_msb(int digit)
{
    if (digit >= 8) return 3;
    if (digit >= 4) return 2;
    if (digit >= 2) return 1;
    return 0;
}

/** @brief Packs a correctly rounded hexadecimal subject into an IEEE format. */
static int wcrt_hexadecimal_bits(const char *string, char **end_pointer,
    int precision, int minimum_exponent, int maximum_exponent, int bias,
    unsigned long long sign_mask, unsigned long long maximum_bits,
    unsigned long long *result_bits)
{
    const char *cursor = string;
    const char *digits;
    const char *significand_end;
    int negative = 0;
    int saw_point = 0;
    int saw_digit = 0;
    int first_digit = -1;
    int first_index = 0;
    int digit_index = 0;
    int digits_before_point = 0;
    int exponent_negative = 0;
    int exponent_value = 0;
    int exponent;
    int subnormal_exponent = minimum_exponent - (precision - 1);
    int keep;
    int stored = 0;
    int guard = 0;
    int sticky = 0;
    int bit_index = 0;
    unsigned long long significand = 0;
    unsigned long long fraction_mask =
        (1ULL << (precision - 1)) - 1ULL;

    while (isspace((unsigned char)*cursor)) ++cursor;
    if (*cursor == '+' || *cursor == '-') {
        negative = *cursor++ == '-';
    }
    if (cursor[0] != '0' || (cursor[1] != 'x' && cursor[1] != 'X')) {
        return 0;
    }
    cursor += 2;
    digits = cursor;
    while (*cursor != '\0') {
        int digit = wcrt_digit((unsigned char)*cursor);
        if (digit >= 0 && digit < 16) {
            saw_digit = 1;
            if (!saw_point && digits_before_point < 100000) {
                ++digits_before_point;
            }
            if (first_digit < 0 && digit != 0) {
                first_digit = digit;
                first_index = digit_index;
            }
            if (digit_index < 100000) ++digit_index;
            ++cursor;
        } else if (*cursor == '.' && !saw_point) {
            saw_point = 1;
            ++cursor;
        } else {
            break;
        }
    }
    if (!saw_digit) return 0;
    significand_end = cursor;
    if (*cursor == 'p' || *cursor == 'P') {
        const char *exponent_start = cursor++;
        if (*cursor == '+' || *cursor == '-') {
            exponent_negative = *cursor++ == '-';
        }
        if (!isdigit((unsigned char)*cursor)) {
            cursor = exponent_start;
        } else {
            while (isdigit((unsigned char)*cursor)) {
                if (exponent_value < 100000) {
                    exponent_value = exponent_value * 10 + *cursor - '0';
                }
                ++cursor;
            }
        }
    }
    if (end_pointer != NULL) *end_pointer = (char *)cursor;
    if (first_digit < 0) {
        *result_bits = negative ? sign_mask : 0;
        return 1;
    }

    exponent = (exponent_negative ? -exponent_value : exponent_value) +
        4 * (digits_before_point - first_index - 1) +
        wcrt_hexadecimal_msb(first_digit);
    keep = exponent >= minimum_exponent ? precision :
        exponent - subnormal_exponent + 1;
    if (keep < 0) {
        errno = ERANGE;
        *result_bits = negative ? sign_mask : 0;
        return 1;
    }

    cursor = digits;
    while (cursor < significand_end) {
        int digit;
        int bit;
        if (*cursor == '.') {
            ++cursor;
            continue;
        }
        digit = wcrt_digit((unsigned char)*cursor++);
        for (bit = 3; bit >= 0; --bit) {
            int value = (digit >> bit) & 1;
            if (bit_index == 0 && value == 0) continue;
            if (stored < keep) {
                significand = (significand << 1) | (unsigned int)value;
                ++stored;
            } else if (stored == keep && bit_index == keep) {
                guard = value;
            } else if (value != 0) {
                sticky = 1;
            }
            ++bit_index;
        }
    }
    while (stored < keep) {
        significand <<= 1;
        ++stored;
    }
    if (guard && (sticky || (significand & 1ULL) != 0)) {
        ++significand;
    }

    if (exponent >= minimum_exponent) {
        if (significand == (1ULL << precision)) {
            significand >>= 1;
            ++exponent;
        }
        if (exponent > maximum_exponent) {
            errno = ERANGE;
            *result_bits = maximum_bits | (negative ? sign_mask : 0);
            return 1;
        }
        *result_bits = ((unsigned long long)(exponent + bias) <<
            (precision - 1)) | (significand & fraction_mask) |
            (negative ? sign_mask : 0);
    } else {
        if (significand >= (1ULL << (precision - 1))) {
            *result_bits = 1ULL << (precision - 1);
        } else {
            *result_bits = significand;
        }
        if (negative) *result_bits |= sign_mask;
        errno = ERANGE;
    }
    return 1;
}

/** @brief Parses C99 infinity or NaN subjects into binary64 bits. */
static int wcrt_special_double(const char *string, char **end_pointer,
    double *result)
{
    const char *cursor = string;
    int negative = 0;
    union wcrt_double_representation converted;

    while (isspace((unsigned char)*cursor)) ++cursor;
    if (*cursor == '+' || *cursor == '-') {
        negative = *cursor++ == '-';
    }
    if (wcrt_ascii_word(cursor, "inf")) {
        cursor += 3;
        if (wcrt_ascii_word(cursor, "inity")) cursor += 5;
        converted.bits = 0x7ff0000000000000ULL;
    } else if (wcrt_ascii_word(cursor, "nan")) {
        cursor += 3;
        if (*cursor == '(') {
            const char *payload = cursor + 1;
            const char *end = payload;
            while (isalnum((unsigned char)*end) || *end == '_') ++end;
            if (*end == ')') cursor = end + 1;
        }
        converted.bits = 0x7ff8000000000000ULL;
    } else {
        return 0;
    }
    if (negative) converted.bits |= 0x8000000000000000ULL;
    if (end_pointer != NULL) *end_pointer = (char *)cursor;
    *result = converted.value;
    return 1;
}

/** @brief Parses C99 infinity or NaN subjects into binary32 bits. */
static int wcrt_special_float(const char *string, char **end_pointer,
    float *result)
{
    double wide;
    union wcrt_double_representation source;
    union wcrt_float_representation converted;
    if (!wcrt_special_double(string, end_pointer, &wide)) return 0;
    source.value = wide;
    converted.bits = (source.bits & 0x8000000000000000ULL) != 0 ?
        0xffc00000U : 0x7fc00000U;
    if ((source.bits & 0x000fffffffffffffULL) == 0) {
        converted.bits &= 0xff800000U;
    }
    *result = converted.value;
    return 1;
}

/** @brief Parses a C99 hexadecimal subject directly into binary64. */
static int wcrt_hexadecimal_double(const char *string, char **end_pointer,
    double *result)
{
    union wcrt_double_representation converted;
    if (!wcrt_hexadecimal_bits(string, end_pointer, 53, -1022, 1023,
        1023, 0x8000000000000000ULL, 0x7fefffffffffffffULL,
        &converted.bits)) {
        return 0;
    }
    *result = converted.value;
    return 1;
}

/** @brief Parses a C99 hexadecimal subject directly into binary32. */
static int wcrt_hexadecimal_float(const char *string, char **end_pointer,
    float *result)
{
    union wcrt_float_representation converted;
    unsigned long long bits;
    if (!wcrt_hexadecimal_bits(string, end_pointer, 24, -126, 127, 127,
        0x80000000ULL, 0x7f7fffffULL, &bits)) {
        return 0;
    }
    converted.bits = (unsigned int)bits;
    *result = converted.value;
    return 1;
}

/** @brief Parses the decimal floating subject sequence shared by C99 APIs. */
static double wcrt_strtod_decimal(const char *string, char **end_pointer)
{
    const char *cursor = string;
    unsigned long long significand = 0;
    double value;
    int negative = 0;
    int converted = 0;
    int exponent = 0;
    int exponent_negative = 0;
    int significant_digits = 0;
    int decimal_adjustment = 0;
    int nonzero = 0;
    int overflow = 0;

    while (isspace((unsigned char)*cursor)) {
        ++cursor;
    }
    if (*cursor == '+' || *cursor == '-') {
        negative = *cursor++ == '-';
    }
    while (isdigit((unsigned char)*cursor)) {
        converted = 1;
        if (*cursor != '0') {
            nonzero = 1;
        }
        if (significant_digits == 0 && *cursor == '0') {
            /* Leading integer zero does not consume stored precision. */
        } else if (significant_digits < 19) {
            significand = significand * 10ULL +
                (unsigned long long)(*cursor - '0');
            ++significant_digits;
        } else {
            ++decimal_adjustment;
        }
        ++cursor;
    }
    if (*cursor == '.') {
        ++cursor;
        while (isdigit((unsigned char)*cursor)) {
            converted = 1;
            if (*cursor != '0') {
                nonzero = 1;
            }
            if (significant_digits == 0 && *cursor == '0') {
                --decimal_adjustment;
            } else if (significant_digits < 19) {
                significand = significand * 10ULL +
                    (unsigned long long)(*cursor - '0');
                ++significant_digits;
                --decimal_adjustment;
            }
            ++cursor;
        }
    }
    if (converted && (*cursor == 'e' || *cursor == 'E')) {
        const char *exponent_start = cursor++;
        if (*cursor == '+' || *cursor == '-') {
            exponent_negative = *cursor++ == '-';
        }
        if (!isdigit((unsigned char)*cursor)) {
            cursor = exponent_start;
        } else {
            while (isdigit((unsigned char)*cursor)) {
                if (exponent < 1000) {
                    exponent = exponent * 10 + (*cursor - '0');
                }
                ++cursor;
            }
        }
    }
    if (!converted) {
        cursor = string;
        value = 0.0;
    } else {
        int scale = decimal_adjustment +
            (exponent_negative ? -exponent : exponent);
        value = wcrt_scale_decimal((double)significand, scale, &overflow);
        if (overflow) {
            errno = ERANGE;
        }
        if (nonzero && value < DBL_MIN) {
            errno = ERANGE;
        }
    }
    if (end_pointer != NULL) {
        *end_pointer = (char *)cursor;
    }
    return negative ? -value : value;
}

double strtod(const char *string, char **end_pointer)
{
    double result;
    if (wcrt_special_double(string, end_pointer, &result)) return result;
    if (wcrt_hexadecimal_double(string, end_pointer, &result)) return result;
    return wcrt_strtod_decimal(string, end_pointer);
}

double atof(const char *string)
{
    return strtod(string, NULL);
}

int atoi(const char *string)
{
    return (int)strtol(string, NULL, 10);
}

long atol(const char *string)
{
    return strtol(string, NULL, 10);
}

int rand(void)
{
    wcrt_random_state = wcrt_random_state * 1103515245UL + 12345UL;
    return (int)((wcrt_random_state >> 16) & RAND_MAX);
}

void srand(unsigned int seed)
{
    wcrt_random_state = seed;
}

void *malloc(size_t size)
{
    return __wcrt_heap_allocate(size, 0);
}

void *calloc(size_t count, size_t size)
{
    if (size != 0 && count > (size_t)-1 / size) {
        return NULL;
    }
    return __wcrt_heap_allocate(count * size, 1);
}

void *realloc(void *memory, size_t size)
{
    return __wcrt_heap_reallocate(memory, size);
}

void free(void *memory)
{
    __wcrt_heap_free(memory);
}

int atexit(void (*function)(void))
{
    if (function == NULL || wcrt_exit_function_count == WCRT_ATEXIT_MAX) {
        return -1;
    }
    wcrt_exit_functions[wcrt_exit_function_count++] = function;
    return 0;
}

void exit(int status)
{
    while (wcrt_exit_function_count != 0) {
        wcrt_exit_functions[--wcrt_exit_function_count]();
    }
    __wcrt_process_exit((unsigned int)status);
}

void abort(void)
{
    __wcrt_process_exit(3);
}

char *getenv(const char *name)
{
    return __wcrt_process_getenv(name);
}

int system(const char *command)
{
    return __wcrt_process_system(command);
}

void *bsearch(const void *key, const void *base, size_t count, size_t size,
    int (*compare)(const void *, const void *))
{
    const unsigned char *bytes = (const unsigned char *)base;
    size_t first = 0;
    while (first < count) {
        size_t middle = first + (count - first) / 2;
        int order = compare(key, bytes + middle * size);
        if (order == 0) {
            return (void *)(bytes + middle * size);
        }
        if (order < 0) {
            count = middle;
        } else {
            first = middle + 1;
        }
    }
    return NULL;
}

void qsort(void *base, size_t count, size_t size,
    int (*compare)(const void *, const void *))
{
    unsigned char *bytes = (unsigned char *)base;
    size_t index;
    for (index = 1; index < count; ++index) {
        size_t current = index;
        while (current != 0 && compare(bytes + (current - 1) * size,
            bytes + current * size) > 0) {
            size_t byte;
            for (byte = 0; byte < size; ++byte) {
                unsigned char temporary = bytes[(current - 1) * size + byte];
                bytes[(current - 1) * size + byte] =
                    bytes[current * size + byte];
                bytes[current * size + byte] = temporary;
            }
            --current;
        }
    }
}

int abs(int value)
{
    return value < 0 ? -value : value;
}

long labs(long value)
{
    return value < 0 ? -value : value;
}

div_t div(int numerator, int denominator)
{
    div_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}

ldiv_t ldiv(long numerator, long denominator)
{
    ldiv_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}

#if !defined(WCRT_C89)

void _Exit(int status)
{
    __wcrt_process_exit((unsigned int)status);
}

long long atoll(const char *string)
{
    return strtoll(string, NULL, 10);
}

long long llabs(long long value)
{
    return value < 0 ? -value : value;
}

lldiv_t lldiv(long long numerator, long long denominator)
{
    lldiv_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}

unsigned long long strtoull(const char *string, char **end_pointer, int base)
{
    return wcrt_strtoull(string, end_pointer, base, NULL, NULL);
}

long long strtoll(const char *string, char **end_pointer, int base)
{
    char *end;
    int negative;
    int overflow;
    unsigned long long magnitude = wcrt_strtoull(string, &end, base,
        &negative, &overflow);

    if (end_pointer != NULL) {
        *end_pointer = end;
    }
    if (end == string) {
        return 0;
    }
    if (overflow) {
        return negative ? (-9223372036854775807LL - 1LL) :
            9223372036854775807LL;
    }
    if (negative) {
        magnitude = 0ULL - magnitude;
        if (magnitude > 9223372036854775808ULL) {
            errno = ERANGE;
            return -9223372036854775807LL - 1LL;
        }
        if (magnitude == 9223372036854775808ULL) {
            return -9223372036854775807LL - 1LL;
        }
        return -(long long)magnitude;
    }
    if (magnitude > 9223372036854775807ULL) {
        errno = ERANGE;
        return 9223372036854775807LL;
    }
    return (long long)magnitude;
}

float strtof(const char *string, char **end_pointer)
{
    char *end;
    double value;
    double magnitude;
    float result;

    if (wcrt_special_float(string, end_pointer, &result)) return result;
    if (wcrt_hexadecimal_float(string, end_pointer, &result)) return result;
    value = wcrt_strtod_decimal(string, &end);
    magnitude = value < 0.0 ? -value : value;

    if (end_pointer != NULL) {
        *end_pointer = end;
    }
    if (end == string) {
        return 0.0F;
    }
    if (magnitude > FLT_MAX) {
        errno = ERANGE;
        return value < 0.0 ? -FLT_MAX : FLT_MAX;
    }
    result = (float)value;
    if (magnitude != 0.0 && magnitude < FLT_MIN) {
        errno = ERANGE;
    }
    return result;
}

long double strtold(const char *string, char **end_pointer)
{
    return (long double)strtod(string, end_pointer);
}

intmax_t imaxabs(intmax_t value)
{
    return value < 0 ? -value : value;
}

imaxdiv_t imaxdiv(intmax_t numerator, intmax_t denominator)
{
    imaxdiv_t result;
    result.quot = numerator / denominator;
    result.rem = numerator % denominator;
    return result;
}

intmax_t strtoimax(const char *string, char **end_pointer, int base)
{
    return (intmax_t)strtoll(string, end_pointer, base);
}

uintmax_t strtoumax(const char *string, char **end_pointer, int base)
{
    return (uintmax_t)strtoull(string, end_pointer, base);
}

#endif

int mblen(const char *string, size_t count)
{
    if (string == NULL) {
        return 0;
    }
    if (count == 0) {
        return -1;
    }
    return *string == '\0' ? 0 : 1;
}

int mbtowc(wchar_t *wide, const char *string, size_t count)
{
    int length = mblen(string, count);
    if (length >= 0 && string != NULL && wide != NULL) {
        *wide = (wchar_t)(unsigned char)*string;
    }
    return length;
}

int wctomb(char *string, wchar_t wide)
{
    if (string == NULL) {
        return 0;
    }
    if ((unsigned long)wide > 255UL) {
        errno = EILSEQ;
        return -1;
    }
    *string = (char)wide;
    return 1;
}

size_t mbstowcs(wchar_t *destination, const char *source, size_t count)
{
    size_t length = 0;
    if (destination == NULL) {
        while (source[length] != '\0') ++length;
        return length;
    }
    while (length < count && source[length] != '\0') {
        destination[length] = (wchar_t)(unsigned char)source[length];
        ++length;
    }
    if (length < count) {
        destination[length] = 0;
    }
    return length;
}

size_t wcstombs(char *destination, const wchar_t *source, size_t count)
{
    size_t length = 0;
    if (destination == NULL) {
        while (source[length] != 0) {
            if ((unsigned long)source[length] > 255UL) {
                errno = EILSEQ;
                return (size_t)-1;
            }
            ++length;
        }
        return length;
    }
    while (length < count && source[length] != 0) {
        if ((unsigned long)source[length] > 255UL) {
            errno = EILSEQ;
            return (size_t)-1;
        }
        destination[length] = (char)source[length];
        ++length;
    }
    if (length < count) {
        destination[length] = '\0';
    }
    return length;
}
