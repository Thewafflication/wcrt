/**
 * @file stdlib.c
 * @brief Implements C89 conversions and general utilities.
 */

#include <ctype.h>
#include <errno.h>
#include <float.h>
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

double strtod(const char *string, char **end_pointer)
{
    const char *cursor = string;
    const char *start;
    double value = 0.0;
    double fraction = 0.1;
    int negative = 0;
    int converted = 0;
    int exponent = 0;
    int exponent_negative = 0;

    while (isspace((unsigned char)*cursor)) {
        ++cursor;
    }
    if (*cursor == '+' || *cursor == '-') {
        negative = *cursor++ == '-';
    }
    start = cursor;
    while (isdigit((unsigned char)*cursor)) {
        converted = 1;
        value = value * 10.0 + (double)(*cursor++ - '0');
    }
    if (*cursor == '.') {
        ++cursor;
        while (isdigit((unsigned char)*cursor)) {
            converted = 1;
            value += (double)(*cursor++ - '0') * fraction;
            fraction *= 0.1;
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
                if (exponent < 10000) {
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
        double factor = exponent_negative ? 0.1 : 10.0;
        while (exponent-- > 0) {
            if (!exponent_negative && value > DBL_MAX / 10.0) {
                errno = ERANGE;
                value = DBL_MAX;
                break;
            }
            value *= factor;
        }
    }
    if (end_pointer != NULL) {
        *end_pointer = (char *)cursor;
    }
    (void)start;
    return negative ? -value : value;
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
        return -1;
    }
    *string = (char)wide;
    return 1;
}

size_t mbstowcs(wchar_t *destination, const char *source, size_t count)
{
    size_t length = 0;
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
    while (length < count && source[length] != 0) {
        if ((unsigned long)source[length] > 255UL) {
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
