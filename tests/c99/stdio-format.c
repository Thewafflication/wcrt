/**
 * @file stdio-format.c
 * @brief Exercises C99 narrow formatted output.
 */

#include <inttypes.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/** @brief Provides exact binary64 test values. */
union wcrt_double_bits {
    unsigned long long bits;
    double value;
};

/** @brief Calls vsnprintf for direct va_list coverage. */
static int wcrt_call_vsnprintf(char *output, size_t size,
    const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vsnprintf(output, size, format, arguments);
    va_end(arguments);
    return result;
}

/** @brief Verifies integer modifiers and n destination types. */
static int wcrt_test_integer_modifiers(void)
{
    char output[256];
    signed char count_hh = -1;
    long long count_ll = -1;
    intmax_t count_j = -1;
    ptrdiff_t count_z = -1;
    ptrdiff_t count_t = -1;
    int result;

    result = snprintf(output, sizeof(output),
        "%hhd %hhu %lld %llu %jd %ju %zd %zu %td %tu",
        -12, 250, -1234567890123LL, 1234567890123ULL,
        (intmax_t)-77, (uintmax_t)88, (ptrdiff_t)-9,
        (size_t)10, (ptrdiff_t)-11, (size_t)12);
    if (result < 0 || strcmp(output,
        "-12 250 -1234567890123 1234567890123 -77 88 -9 10 -11 12")
        != 0) {
        return 1;
    }

    result = snprintf(output, sizeof(output),
        "%hhd %hhi %hho %hhu %hhx %hhX",
        -12, -12, 10, 250, 250, 250);
    if (result != 20 || strcmp(output, "-12 -12 12 250 fa FA") != 0) {
        return 2;
    }
    result = snprintf(output, sizeof(output),
        "%lld %lli %llo %llu %llx %llX",
        -12LL, -12LL, 10ULL, 250ULL, 250ULL, 250ULL);
    if (result != 20 || strcmp(output, "-12 -12 12 250 fa FA") != 0) {
        return 3;
    }
    result = snprintf(output, sizeof(output), "%jd %ji %jo %ju %jx %jX",
        (intmax_t)-12, (intmax_t)-12, (uintmax_t)10,
        (uintmax_t)250, (uintmax_t)250, (uintmax_t)250);
    if (result != 20 || strcmp(output, "-12 -12 12 250 fa FA") != 0) {
        return 4;
    }
    result = snprintf(output, sizeof(output), "%zd %zi %zo %zu %zx %zX",
        (ptrdiff_t)-12, (ptrdiff_t)-12, (size_t)10,
        (size_t)250, (size_t)250, (size_t)250);
    if (result != 20 || strcmp(output, "-12 -12 12 250 fa FA") != 0) {
        return 5;
    }
    result = snprintf(output, sizeof(output), "%td %ti %to %tu %tx %tX",
        (ptrdiff_t)-12, (ptrdiff_t)-12, (size_t)10,
        (size_t)250, (size_t)250, (size_t)250);
    if (result != 20 || strcmp(output, "-12 -12 12 250 fa FA") != 0) {
        return 6;
    }
    result = snprintf(output, sizeof(output), "%" PRIdMAX,
        (intmax_t)-1234);
    if (result != 5 || strcmp(output, "-1234") != 0) return 7;

    result = snprintf(output, sizeof(output),
        "A%hhnB%llnC%jnD%znE%tn",
        &count_hh, &count_ll, &count_j, &count_z, &count_t);
    if (result != 5 || strcmp(output, "ABCDE") != 0 ||
        count_hh != 1 || count_ll != 2 || count_j != 3 ||
        count_z != 4 || count_t != 5) {
        return 8;
    }

    result = snprintf(output, sizeof(output), "%#.0o %.0u %#08x",
        0U, 0U, 42U);
    if (result != 11 || strcmp(output, "0  0x00002a") != 0) {
        return 9;
    }
    return 0;
}

/** @brief Verifies hexadecimal, special, and wide-character output. */
static int wcrt_test_floating_and_characters(void)
{
    union wcrt_double_bits infinity;
    union wcrt_double_bits nan_value;
    union wcrt_double_bits negative_zero;
    wchar_t wide[] = { (wchar_t)'O', (wchar_t)'K', 0 };
    char output[256];
    int result;

    result = snprintf(output, sizeof(output), "%a %A %.1a %La",
        3.0, 3.0, 1.09375, (long double)1.25L);
    if (result < 0 || strcmp(output,
        "0x1.8p+1 0X1.8P+1 0x1.2p+0 0x1.4p+0") != 0) {
        return 1;
    }

    result = snprintf(output, sizeof(output), "%.1a %.1a",
        1.03125, 1.09375);
    if (result != 17 || strcmp(output,
        "0x1.0p+0 0x1.2p+0") != 0) {
        return 2;
    }
    result = snprintf(output, sizeof(output), "%+#012.2a",
        3.0);
    if (result != 12 || strcmp(output, "+0x001.80p+1") != 0) {
        return 3;
    }

    infinity.bits = 0x7ff0000000000000ULL;
    nan_value.bits = 0x7ff8000000000001ULL;
    result = snprintf(output, sizeof(output), "%F %F",
        infinity.value, nan_value.value);
    if (result != 7 || strcmp(output, "INF NAN") != 0) {
        return 4;
    }

    negative_zero.bits = 0x8000000000000000ULL;
    result = snprintf(output, sizeof(output), "%a", negative_zero.value);
    if (result != 7 || strcmp(output, "-0x0p+0") != 0) {
        return 5;
    }

    result = snprintf(output, sizeof(output), "%.2f %.0f %.0f %g",
        -1.25, 2.5, 3.5, 100.0);
    if (result != 13 || strcmp(output, "-1.25 2 4 100") != 0) {
        return 6;
    }

    result = snprintf(output, sizeof(output), "%lc %ls",
        (int)(wchar_t)'A', wide);
    if (result != 4 || strcmp(output, "A OK") != 0) {
        return 7;
    }
    return 0;
}

/** @brief Verifies bounded, length-only, and va_list paths. */
static int wcrt_test_sinks(void)
{
    char output[32];
    char small[5];
    int result;

    result = snprintf(NULL, 0, "%.300F", 1.0);
    if (result != 302) return 1;
    memset(small, 'X', sizeof(small));
    result = snprintf(small, 4, "%.300F", 1.0);
    if (result != 302 || memcmp(small, "1.0\0X", 5) != 0) return 2;

    result = wcrt_call_vsnprintf(output, sizeof(output), "%jd:%a",
        (intmax_t)-9, 2.0);
    if (result != 9 || strcmp(output, "-9:0x1p+1") != 0) return 3;
    if (snprintf(NULL, 0, "%.300d", 1) != 300) return 4;
    return 0;
}

/** @brief Runs C99 narrow formatted-output tests. */
int main(void)
{
    int result = wcrt_test_integer_modifiers();
    if (result != 0) return result;
    result = wcrt_test_floating_and_characters();
    if (result != 0) return 10 + result;
    result = wcrt_test_sinks();
    if (result != 0) return 20 + result;
    return 0;
}
