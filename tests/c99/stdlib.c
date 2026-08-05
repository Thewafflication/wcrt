/**
 * @file stdlib.c
 * @brief Exercises C99 long-long and decimal conversion utilities.
 */
#include <errno.h>
#include <float.h>
#include <stdlib.h>

/** @brief Runs C99 general-utility checks. */
int main(void)
{
    char *end;
    lldiv_t divided;
    float single;
    long double extended;

    if (atoll(" -42x") != -42 || llabs(-17) != 17) return 1;
    divided = lldiv(-20, 6);
    if (divided.quot != -3 || divided.rem != -2) return 2;
    errno = EDOM;
    if (strtoll("0X7fffffffffffffff!", &end, 0) !=
        9223372036854775807LL || *end != '!' || errno != EDOM) return 3;
    if (strtoll("-9223372036854775808", &end, 10) !=
        (-9223372036854775807LL - 1LL)) return 4;
    errno = 0;
    if (strtoll("9223372036854775808", &end, 10) !=
        9223372036854775807LL || errno != ERANGE) return 5;
    errno = 0;
    if (strtoull("18446744073709551616", &end, 10) !=
        18446744073709551615ULL || errno != ERANGE) return 6;
    errno = EDOM;
    if (strtoull("0777z", &end, 0) != 511 || *end != 'z' ||
        errno != EDOM) return 7;

    errno = EDOM;
    single = strtof(" \t-12.5e2x", &end);
    if (single != -1250.0F || *end != 'x' || errno != EDOM) return 8;
    extended = strtold(".125E+2!", &end);
    if (extended != (long double)12.5 || *end != '!') return 9;
    extended = strtold("0.00000000000000000001", &end);
    if (extended <= 0.0L || *end != '\0') return 18;
    errno = EDOM;
    extended = strtold("1.7976931348623157e308", &end);
    if (extended != LDBL_MAX) return 19;
    if (*end != '\0') return 20;
    if (errno != EDOM) return 21;
    errno = EDOM;
    extended = strtold("2.2250738585072014e-308", &end);
    if (extended != LDBL_MIN) return 22;
    if (*end != '\0') return 23;
    if (errno != EDOM) return 24;
    if (strtof("1e+x", &end) != 1.0F || *end != 'e') return 10;
    if (strtof("-0", &end) != 0.0F ||
        1.0F / strtof("-0", &end) > 0.0F) return 11;
    errno = EDOM;
    if (strtof("xyz", &end) != 0.0F || end[0] != 'x' ||
        errno != EDOM) return 12;
    errno = 0;
    single = strtof("1e100", &end);
    if (single != FLT_MAX || errno != ERANGE) return 13;
    errno = 0;
    single = strtof("1e-100", &end);
    if (single != 0.0F || errno != ERANGE) return 14;
    errno = 0;
    extended = strtold("1e1000", &end);
    if (extended != (long double)DBL_MAX || errno != ERANGE) return 15;
    errno = 0;
    extended = strtold("1e-1000", &end);
    if (extended != 0.0L || errno != ERANGE) return 16;
    if (sizeof(long double) != sizeof(double)) return 17;
    return 0;
}
