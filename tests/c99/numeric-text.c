/**
 * @file numeric-text.c
 * @brief Exercises C99 narrow numeric text interchange.
 */

#include <errno.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief Provides exact binary64 test values. */
union wcrt_double_bits {
    unsigned long long bits;
    double value;
};

/** @brief Provides exact binary32 test values. */
union wcrt_float_bits {
    unsigned int bits;
    float value;
};

/** @brief Returns the binary64 representation of a value. */
static unsigned long long wcrt_double_representation(double value)
{
    union wcrt_double_bits converted;
    converted.value = value;
    return converted.bits;
}

/** @brief Returns the binary32 representation of a value. */
static unsigned int wcrt_float_representation(float value)
{
    union wcrt_float_bits converted;
    converted.value = value;
    return converted.bits;
}

/** @brief Verifies hexadecimal syntax, end pointers, and exact values. */
static int wcrt_test_hexadecimal(void)
{
    const char *malformed = "0x.p1";
    char *end;
    double value;

    errno = EDOM;
    value = strtod("0x1.8p+1!", &end);
    if (value != 3.0 || *end != '!' || errno != EDOM) return 1;
    value = strtod("0X.8P+2?", &end);
    if (value != 2.0 || *end != '?') return 2;
    value = strtod("0x1.8z", &end);
    if (value != 1.5 || *end != 'z') return 3;
    value = strtod(malformed, &end);
    if (value != 0.0 || end != malformed + 1) return 4;
    value = strtod("0x1.00000000000008p+0", &end);
    if (wcrt_double_representation(value) != 0x3ff0000000000000ULL ||
        *end != '\0') {
        return 5;
    }
    value = strtod("0x1.00000000000018p+0", &end);
    if (wcrt_double_representation(value) != 0x3ff0000000000002ULL ||
        *end != '\0') {
        return 6;
    }
    errno = 0;
    value = strtod("0x0.00000000000008p-1022", &end);
    if (wcrt_double_representation(value) != 0 || errno != ERANGE ||
        *end != '\0') {
        return 7;
    }
    return 0;
}

/** @brief Verifies special values, signs, and range behavior. */
static int wcrt_test_special_and_range(void)
{
    char *end;
    double value;
    unsigned long long bits;

    value = strtod("-INFINITY!", &end);
    bits = wcrt_double_representation(value);
    if (bits != 0xfff0000000000000ULL || *end != '!') return 1;
    value = strtod("NaN(payload)?", &end);
    bits = wcrt_double_representation(value);
    if ((bits & 0x7ff0000000000000ULL) !=
        0x7ff0000000000000ULL ||
        (bits & 0x000fffffffffffffULL) == 0 || *end != '?') {
        return 2;
    }
    value = strtod("-0x0p+0", &end);
    if (wcrt_double_representation(value) !=
        0x8000000000000000ULL || *end != '\0') {
        return 3;
    }

    errno = 0;
    value = strtod("0x1p+1024", &end);
    if (value != DBL_MAX || errno != ERANGE || *end != '\0') return 4;
    errno = 0;
    value = strtod("0x1p-2000", &end);
    if (wcrt_double_representation(value) != 0 ||
        errno != ERANGE || *end != '\0') {
        return 5;
    }
    errno = 0;
    value = strtod("0x0.0000000000001p-1022", &end);
    if (wcrt_double_representation(value) != 1ULL ||
        errno != ERANGE || *end != '\0') {
        return 6;
    }
    return 0;
}

/** @brief Verifies direct binary32 rounding without double rounding. */
static int wcrt_test_float_rounding(void)
{
    char *end;
    float value;

    errno = EDOM;
    value = strtof("0x1.000001p+0", &end);
    if (wcrt_float_representation(value) != 0x3f800000U ||
        *end != '\0' || errno != EDOM) {
        return 1;
    }
    value = strtof("0x1.000003p+0", &end);
    if (wcrt_float_representation(value) != 0x3f800002U ||
        *end != '\0') {
        return 2;
    }
    errno = 0;
    value = strtof("0x1p+128", &end);
    if (value != FLT_MAX || errno != ERANGE || *end != '\0') return 3;
    errno = 0;
    value = strtof("0x1p-200", &end);
    if (wcrt_float_representation(value) != 0 || errno != ERANGE ||
        *end != '\0') {
        return 4;
    }
    if (strtold("0x1.4p+0", &end) != 1.25L || *end != '\0') return 5;
    return 0;
}

/** @brief Verifies cross-interface hexadecimal round trips. */
static int wcrt_test_round_trip(void)
{
    static const unsigned long long boundaries[] = {
        0x0000000000000001ULL,
        0x000fffffffffffffULL,
        0x0010000000000000ULL,
        0x3ff0000000000000ULL,
        0x7fefffffffffffffULL,
        0x8000000000000000ULL,
        0xbff0000000000000ULL
    };
    union wcrt_double_bits original;
    union wcrt_double_bits converted;
    const char *no_conversion = "xyz";
    char buffer[64];
    char *end;
    unsigned long long state = 0x243f6a8885a308d3ULL;
    size_t index;

    original.bits = 0x400921fb54442d18ULL;
    if (snprintf(buffer, sizeof(buffer), "%a", original.value) <= 0) {
        return 1;
    }
    if (sscanf(buffer, "%la", &converted.value) != 1 ||
        converted.bits != original.bits) {
        return 2;
    }
    errno = EDOM;
    if (strtod(no_conversion, &end) != 0.0 || end != no_conversion ||
        errno != EDOM) {
        return 3;
    }
    original.bits = boundaries[0];
    if (snprintf(buffer, sizeof(buffer), "%a", original.value) != 23 ||
        strcmp(buffer, "0x0.0000000000001p-1022") != 0) {
        return 4;
    }
    original.bits = boundaries[4];
    if (snprintf(buffer, sizeof(buffer), "%a", original.value) != 23 ||
        strcmp(buffer, "0x1.fffffffffffffp+1023") != 0) {
        return 5;
    }
    for (index = 0; index < sizeof(boundaries) / sizeof(boundaries[0]);
        ++index) {
        original.bits = boundaries[index];
        if (snprintf(buffer, sizeof(buffer), "%a", original.value) <= 0 ||
            sscanf(buffer, "%la", &converted.value) != 1 ||
            converted.bits != original.bits) {
            return 6;
        }
    }
    for (index = 0; index < 512; ++index) {
        state = state * 6364136223846793005ULL + 1442695040888963407ULL;
        original.bits = state;
        if ((original.bits & 0x7ff0000000000000ULL) ==
            0x7ff0000000000000ULL) {
            original.bits ^= 0x0010000000000000ULL;
        }
        if (snprintf(buffer, sizeof(buffer), "%a", original.value) <= 0 ||
            sscanf(buffer, "%la", &converted.value) != 1 ||
            converted.bits != original.bits) {
            return 7;
        }
    }
    return 0;
}

/** @brief Runs C99 narrow numeric-text tests. */
int main(void)
{
    int result = wcrt_test_hexadecimal();
    if (result != 0) return result;
    result = wcrt_test_special_and_range();
    if (result != 0) return 10 + result;
    result = wcrt_test_float_rounding();
    if (result != 0) return 20 + result;
    result = wcrt_test_round_trip();
    if (result != 0) return 30 + result;
    return 0;
}
