/**
 * @file inttypes.c
 * @brief Exercises C99 greatest-width arithmetic, formats, and conversions.
 */
#include <errno.h>
#include <inttypes.h>
#include <string.h>

#define WCRT_FORMAT_SET(prefix) \
    prefix##8, prefix##16, prefix##32, prefix##64, \
    prefix##LEAST8, prefix##LEAST16, prefix##LEAST32, prefix##LEAST64, \
    prefix##FAST8, prefix##FAST16, prefix##FAST32, prefix##FAST64, \
    prefix##MAX, prefix##PTR

#if defined(_WIN64)
#define WCRT_POINTER_FORMAT(letter) "ll" letter
#else
#define WCRT_POINTER_FORMAT(letter) letter
#endif

#define WCRT_PRINT_EXPECTED(letter) \
    letter, letter, letter, "ll" letter, \
    letter, letter, letter, "ll" letter, \
    letter, letter, letter, "ll" letter, \
    "ll" letter, WCRT_POINTER_FORMAT(letter)

#define WCRT_SCAN_EXPECTED(letter) \
    "hh" letter, "h" letter, letter, "ll" letter, \
    "hh" letter, "h" letter, letter, "ll" letter, \
    letter, letter, letter, "ll" letter, \
    "ll" letter, WCRT_POINTER_FORMAT(letter)

/** @brief Actual spellings of every controlled integer format macro. */
static const char *const wcrt_formats[] = {
    WCRT_FORMAT_SET(PRId), WCRT_FORMAT_SET(PRIi),
    WCRT_FORMAT_SET(PRIo), WCRT_FORMAT_SET(PRIu),
    WCRT_FORMAT_SET(PRIx), WCRT_FORMAT_SET(PRIX),
    WCRT_FORMAT_SET(SCNd), WCRT_FORMAT_SET(SCNi),
    WCRT_FORMAT_SET(SCNo), WCRT_FORMAT_SET(SCNu),
    WCRT_FORMAT_SET(SCNx)
};

/** @brief ABI-specific expected spellings matching wcrt_formats. */
static const char *const wcrt_expected_formats[] = {
    WCRT_PRINT_EXPECTED("d"), WCRT_PRINT_EXPECTED("i"),
    WCRT_PRINT_EXPECTED("o"), WCRT_PRINT_EXPECTED("u"),
    WCRT_PRINT_EXPECTED("x"), WCRT_PRINT_EXPECTED("X"),
    WCRT_SCAN_EXPECTED("d"), WCRT_SCAN_EXPECTED("i"),
    WCRT_SCAN_EXPECTED("o"), WCRT_SCAN_EXPECTED("u"),
    WCRT_SCAN_EXPECTED("x")
};

/** @brief Checks every controlled format macro against the selected ABI. */
static int wcrt_check_formats(void)
{
    size_t index;
    size_t count = sizeof(wcrt_formats) / sizeof(wcrt_formats[0]);

    if (count != sizeof(wcrt_expected_formats) /
        sizeof(wcrt_expected_formats[0])) {
        return 1;
    }
    for (index = 0; index < count; ++index) {
        if (strcmp(wcrt_formats[index], wcrt_expected_formats[index]) != 0) {
            return 1;
        }
    }
    return 0;
}

/** @brief Runs greatest-width integer checks. */
int main(void)
{
    char *end;
    wchar_t *wide_end;
    imaxdiv_t divided;
    int base;

    if (wcrt_check_formats() != 0) return 1;
    if (imaxabs(-42) != 42) return 15;
    divided = imaxdiv(-20, 6);
    if (divided.quot != -3 || divided.rem != -2) return 16;

    errno = EDOM;
    if (strtoimax(" \t-0x7fZ", &end, 0) != -127 || *end != 'Z' ||
        errno != EDOM) return 17;
    if (strtoimax("10102", &end, 2) != 10 || *end != '2') return 18;
    if (strtoimax("z!", &end, 36) != 35 || *end != '!') return 19;
    for (base = 2; base <= 36; ++base) {
        if (strtoimax("10!", &end, base) != base || *end != '!') return 28;
    }
    if (strtoimax("9223372036854775807", &end, 10) != INTMAX_MAX ||
        *end != '\0') return 20;
    if (strtoimax("-9223372036854775808", &end, 10) != INTMAX_MIN ||
        *end != '\0') return 21;
    errno = 0;
    if (strtoimax("9223372036854775808x", &end, 10) != INTMAX_MAX ||
        errno != ERANGE || *end != 'x') return 22;
    errno = 0;
    if (strtoimax("-9223372036854775809", &end, 10) != INTMAX_MIN ||
        errno != ERANGE) return 23;
    errno = EDOM;
    if (strtoumax("18446744073709551615", &end, 10) != UINTMAX_MAX ||
        errno != EDOM) return 24;
    if (strtoumax("-1", &end, 10) != UINTMAX_MAX) return 25;
    errno = 0;
    if (strtoumax("18446744073709551616", &end, 10) != UINTMAX_MAX ||
        errno != ERANGE) return 26;
    errno = EDOM;
    if (strtoimax("  +x", &end, 10) != 0 || end[0] != ' ' ||
        errno != EDOM) return 27;
    if (wcstoimax(L"-42!", &wide_end, 10) != (intmax_t)-42 ||
        *wide_end != L'!') return 29;
    if (wcstoumax(L"ff!", &wide_end, 16) != (uintmax_t)255 ||
        *wide_end != L'!') return 30;
    return 0;
}
