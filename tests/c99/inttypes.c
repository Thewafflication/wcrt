/**
 * @file inttypes.c
 * @brief Exercises C99 greatest-width arithmetic, formats, and conversions.
 */
#include <errno.h>
#include <inttypes.h>
#include <string.h>

#define CHECK_FORMAT(name, expected, code) \
    do { if (strcmp(name, expected) != 0) return code; } while (0)

/** @brief Runs greatest-width integer checks. */
int main(void)
{
    char *end;
    imaxdiv_t divided;
    int base;

    CHECK_FORMAT(PRId8, "d", 1);
    CHECK_FORMAT(PRId16, "d", 2);
    CHECK_FORMAT(PRId32, "d", 3);
    CHECK_FORMAT(PRId64, "lld", 4);
    CHECK_FORMAT(PRIuLEAST8, "u", 5);
    CHECK_FORMAT(PRIxFAST16, "x", 6);
    CHECK_FORMAT(PRIXMAX, "llX", 7);
    CHECK_FORMAT(SCNd8, "hhd", 8);
    CHECK_FORMAT(SCNi16, "hi", 9);
    CHECK_FORMAT(SCNo32, "o", 10);
    CHECK_FORMAT(SCNu64, "llu", 11);
    CHECK_FORMAT(SCNxMAX, "llx", 12);
#if defined(_WIN64)
    CHECK_FORMAT(PRIdPTR, "lld", 13);
    CHECK_FORMAT(SCNxPTR, "llx", 14);
#else
    CHECK_FORMAT(PRIdPTR, "d", 13);
    CHECK_FORMAT(SCNxPTR, "x", 14);
#endif
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
    return 0;
}
