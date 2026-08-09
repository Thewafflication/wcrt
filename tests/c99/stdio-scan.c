/**
 * @file stdio-scan.c
 * @brief Exercises C99 narrow formatted input.
 */

#include <float.h>
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/** @brief Provides exact binary64 test values. */
union wcrt_scan_double_bits {
    unsigned long long bits;
    double value;
};

/** @brief Calls vsscanf for direct va_list coverage. */
static int wcrt_call_vsscanf(const char *source, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vsscanf(source, format, arguments);
    va_end(arguments);
    return result;
}

/** @brief Calls vfscanf for direct va_list coverage. */
static int wcrt_call_vfscanf(FILE *stream, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vfscanf(stream, format, arguments);
    va_end(arguments);
    return result;
}

/** @brief Verifies integer modifiers and n destination types. */
static int wcrt_test_integer_modifiers(void)
{
    signed char signed_byte = 0;
    unsigned char unsigned_byte = 0;
    long long signed_wide = 0;
    unsigned long long unsigned_wide = 0;
    intmax_t signed_maximum = 0;
    uintmax_t unsigned_maximum = 0;
    ptrdiff_t signed_size = 0;
    size_t unsigned_size = 0;
    ptrdiff_t difference = 0;
    size_t unsigned_difference = 0;
    signed char count_hh = -1;
    long long count_ll = -1;
    intmax_t count_j = -1;
    ptrdiff_t count_z = -1;
    ptrdiff_t count_t = -1;
    int result;

    result = wcrt_call_vsscanf(
        "-12 250 -1234567890123 1234567890123 -77 88 -9 10 -11 12",
        "%hhd %hhu %lld %llu %jd %ju %zd %zu %td %tu",
        &signed_byte, &unsigned_byte, &signed_wide, &unsigned_wide,
        &signed_maximum, &unsigned_maximum, &signed_size,
        &unsigned_size, &difference, &unsigned_difference);
    if (result != 10 || signed_byte != -12 || unsigned_byte != 250 ||
        signed_wide != -1234567890123LL ||
        unsigned_wide != 1234567890123ULL || signed_maximum != -77 ||
        unsigned_maximum != 88 || signed_size != -9 ||
        unsigned_size != 10 || difference != -11 ||
        unsigned_difference != 12) {
        return 1;
    }

    result = wcrt_call_vsscanf("ABCDE", "A%hhnB%llnC%jnD%znE%tn",
        &count_hh, &count_ll, &count_j, &count_z, &count_t);
    if (result != 0 || count_hh != 1 || count_ll != 2 ||
        count_j != 3 || count_z != 4 || count_t != 5) {
        return 2;
    }
    return 0;
}

/** @brief Verifies each new modifier across the integer conversion radix. */
static int wcrt_test_integer_radices(void)
{
    const char *source = "-12 -12 12 250 fa FA";
    signed char hh_d = 0, hh_i = 0;
    unsigned char hh_o = 0, hh_u = 0, hh_x = 0, hh_X = 0;
    long long ll_d = 0, ll_i = 0;
    unsigned long long ll_o = 0, ll_u = 0, ll_x = 0, ll_X = 0;
    intmax_t j_d = 0, j_i = 0;
    uintmax_t j_o = 0, j_u = 0, j_x = 0, j_X = 0;
    ptrdiff_t z_d = 0, z_i = 0, t_d = 0, t_i = 0;
    size_t z_o = 0, z_u = 0, z_x = 0, z_X = 0;
    size_t t_o = 0, t_u = 0, t_x = 0, t_X = 0;
    intmax_t macro_value = 0;
    int result;

    result = sscanf(source, "%hhd %hhi %hho %hhu %hhx %hhX",
        &hh_d, &hh_i, &hh_o, &hh_u, &hh_x, &hh_X);
    if (result != 6 || hh_d != -12 || hh_i != -12 || hh_o != 10 ||
        hh_u != 250 || hh_x != 250 || hh_X != 250) {
        return 1;
    }
    result = sscanf(source, "%lld %lli %llo %llu %llx %llX",
        &ll_d, &ll_i, &ll_o, &ll_u, &ll_x, &ll_X);
    if (result != 6 || ll_d != -12 || ll_i != -12 || ll_o != 10 ||
        ll_u != 250 || ll_x != 250 || ll_X != 250) {
        return 2;
    }
    result = sscanf(source, "%jd %ji %jo %ju %jx %jX",
        &j_d, &j_i, &j_o, &j_u, &j_x, &j_X);
    if (result != 6 || j_d != -12 || j_i != -12 || j_o != 10 ||
        j_u != 250 || j_x != 250 || j_X != 250) {
        return 3;
    }
    result = sscanf(source, "%zd %zi %zo %zu %zx %zX",
        &z_d, &z_i, &z_o, &z_u, &z_x, &z_X);
    if (result != 6 || z_d != -12 || z_i != -12 || z_o != 10 ||
        z_u != 250 || z_x != 250 || z_X != 250) {
        return 4;
    }
    result = sscanf(source, "%td %ti %to %tu %tx %tX",
        &t_d, &t_i, &t_o, &t_u, &t_x, &t_X);
    if (result != 6 || t_d != -12 || t_i != -12 || t_o != 10 ||
        t_u != 250 || t_x != 250 || t_X != 250) {
        return 5;
    }
    result = sscanf("-1234", "%" SCNdMAX, &macro_value);
    if (result != 1 || macro_value != -1234) return 6;
    return 0;
}

/** @brief Verifies numeric, character, and failure partitions. */
static int wcrt_test_conversions(void)
{
    float first = 0.0F;
    double second = 0.0;
    long double third = 0.0L;
    wchar_t wide_character = 0;
    wchar_t wide_string[4] = { 0, 0, 0, 0 };
    wchar_t wide_set[4] = { 0, 0, 0, 0 };
    char character = 'X';
    int integer = 0;
    int untouched = 99;
    float spellings[7] = { 0.0F, 0.0F, 0.0F, 0.0F,
        0.0F, 0.0F, 0.0F };
    union wcrt_scan_double_bits overflow;
    union wcrt_scan_double_bits underflow;
    int result;

    result = wcrt_call_vsscanf("0x1.8p+1 INF nan",
        "%a %lF %LA", &first, &second, &third);
    if (result != 3 || first != 3.0F || second <= DBL_MAX ||
        third == third) {
        return 1;
    }

    result = wcrt_call_vsscanf("1 2 3 4 5 6 0x1p+3",
        "%e %E %f %F %g %G %A", &spellings[0], &spellings[1],
        &spellings[2], &spellings[3], &spellings[4], &spellings[5],
        &spellings[6]);
    if (result != 7 || spellings[0] != 1.0F ||
        spellings[5] != 6.0F || spellings[6] != 8.0F) {
        return 2;
    }

    result = wcrt_call_vsscanf("A BC abc1", "%lc %2ls %3l[a-z]",
        &wide_character, wide_string, wide_set);
    if (result != 3 || wide_character != (wchar_t)'A' ||
        wide_string[0] != (wchar_t)'B' ||
        wide_string[1] != (wchar_t)'C' || wide_string[2] != 0 ||
        wide_set[0] != (wchar_t)'a' || wide_set[2] != (wchar_t)'c' ||
        wide_set[3] != 0) {
        return 3;
    }

    result = sscanf("123", "%2d%c", &integer, &character);
    if (result != 2 || integer != 12 || character != '3') return 4;
    result = sscanf("12.34x", "%5f%c", &first, &character);
    if (result != 2 || first != 12.34F || character != 'x') return 5;
    if (sscanf("x", "%d", &untouched) != 0 || untouched != 99) {
        return 6;
    }
    if (sscanf("", "%d", &untouched) != EOF || untouched != 99) {
        return 7;
    }
    if (sscanf("12 x", "%d %*c%d", &integer, &untouched) != 1 ||
        integer != 12 || untouched != 99) {
        return 8;
    }
    character = 'X';
    if (sscanf("100er", "%f%c", &first, &character) != 0 ||
        character != 'X') {
        return 9;
    }
    if (sscanf("x1", "%*d%d", &untouched) != 0 || untouched != 99) {
        return 10;
    }
    if (sscanf("1", "%*d%d", &untouched) != 0 || untouched != 99) {
        return 11;
    }
    errno = 0;
    result = sscanf("0x1p+1024 0x1p-2000", "%la %la",
        &overflow.value, &underflow.value);
    if (result != 2 || overflow.value != DBL_MAX ||
        underflow.bits != 0 || errno != ERANGE) {
        return 12;
    }
    return 0;
}

/** @brief Verifies first-unread and repeated stream behavior. */
static int wcrt_test_stream_state(void)
{
    FILE *stream = tmpfile();
    int first = 0;
    int second = 0;
    float floating = 0.0F;
    int result;

    if (stream == NULL || fputs("12 34x", stream) == EOF) return 1;
    rewind(stream);
    result = wcrt_call_vfscanf(stream, "%d", &first);
    if (result != 1 || first != 12) return 2;
    result = wcrt_call_vfscanf(stream, "%d", &second);
    if (result != 1 || second != 34 || fgetc(stream) != 'x') return 3;
    fclose(stream);

    stream = tmpfile();
    if (stream == NULL || fputs("100er", stream) == EOF) return 4;
    rewind(stream);
    if (fscanf(stream, "%f", &floating) != 0 || fgetc(stream) != 'r') {
        return 5;
    }
    fclose(stream);

    stream = tmpfile();
    if (stream == NULL) return 6;
    if (fscanf(stream, "%d", &first) != EOF) return 7;
    fclose(stream);
    return 0;
}

/** @brief Runs C99 narrow formatted-input tests. */
int main(void)
{
    int result = wcrt_test_integer_modifiers();
    if (result != 0) return result;
    result = wcrt_test_integer_radices();
    if (result != 0) return 10 + result;
    result = wcrt_test_conversions();
    if (result != 0) return 20 + result;
    result = wcrt_test_stream_state();
    if (result != 0) return 40 + result;
    return 0;
}
