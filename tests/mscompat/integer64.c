/**
 * @file integer64.c
 * @brief Verifies Microsoft 64-bit integer conversion interfaces.
 */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    /** @brief Exact unsigned maximum in bases 2 through 36. */
    static const char *maximum_digits[] = {
        "1111111111111111111111111111111111111111111111111111111111111111",
        "11112220022122120101211020120210210211220",
        "33333333333333333333333333333333",
        "2214220303114400424121122430",
        "3520522010102100444244423",
        "45012021522523134134601",
        "1777777777777777777777",
        "145808576354216723756",
        "18446744073709551615",
        "335500516a429071284",
        "839365134a2a240713",
        "219505a9511a867b72",
        "8681049adb03db171",
        "2c1d56b648c6cd110",
        "ffffffffffffffff",
        "67979g60f5428010",
        "2d3fgb0b9cg4bd2f",
        "141c8786h1ccaagg",
        "b53bjh07be4dj0f",
        "5e8g4ggg7g56dif",
        "2l4lf104353j8kf",
        "1ddh88h2782i515",
        "l12ee5fn0ji1if",
        "c9c336o0mlb7ef",
        "7b7n2pcniokcgf",
        "4eo8hfam6fllmo",
        "2nc6j26l66rhof",
        "1n3rsh11f098rn",
        "14l9lkmo30o40f",
        "nd075ib45k86f",
        "fvvvvvvvvvvvv",
        "b1w8p7j5q9r6f",
        "7orp63sh4dphh",
        "5g24a25twkwff",
        "3w5e11264sgsf"
    };
    char buffer[66];
    char overflow[68];
    char *end;
    const char *text;
    int base;

    for (base = 2; base <= 36; ++base) {
        memset(buffer, 'Z', sizeof(buffer));
        errno = EDOM;
        if (_ui64toa(_UI64_MAX, buffer, base) != buffer ||
            strcmp(buffer, maximum_digits[base - 2]) != 0 ||
            errno != EDOM || buffer[65] != 'Z') return 1;
        if (_strtoui64(buffer, &end, base) != _UI64_MAX ||
            *end != '\0' || errno != EDOM) return 2;
        if (base != 10 && (_i64toa(-1, buffer, base) != buffer ||
            strcmp(buffer, maximum_digits[base - 2]) != 0)) return 3;
        strcpy(overflow, maximum_digits[base - 2]);
        strcat(overflow, "0!");
        errno = 0;
        if (_strtoui64(overflow, &end, base) != _UI64_MAX ||
            errno != ERANGE || *end != '!') return 4;
    }
    if (_i64toa(_I64_MIN, buffer, 10) != buffer ||
        strcmp(buffer, "-9223372036854775808") != 0) return 5;
    if (_i64toa(_I64_MAX, buffer, 10) != buffer ||
        strcmp(buffer, "9223372036854775807") != 0) return 6;
    if (_i64toa(-1, buffer, 10) != buffer || strcmp(buffer, "-1") != 0)
        return 7;
    if (_ui64toa(0, buffer, 36) != buffer || strcmp(buffer, "0") != 0)
        return 8;
    errno = EDOM;
    if (_atoi64("  -9223372036854775808tail") != _I64_MIN ||
        errno != EDOM || _atoi64("314.5") != 314 || _atoi64("0x10") != 0)
        return 9;
    text = " +9223372036854775807!";
    if (_strtoi64(text, &end, 10) != _I64_MAX || *end != '!' ||
        errno != EDOM) return 10;
    text = "9223372036854775808!";
    errno = 0;
    if (_strtoi64(text, &end, 10) != _I64_MAX ||
        errno != ERANGE || *end != '!') return 11;
    errno = 0;
    if (_strtoi64("-9223372036854775809", NULL, 10) != _I64_MIN ||
        errno != ERANGE) return 12;
    errno = 0;
    if (_atoi64("999999999999999999999999999") != _I64_MAX ||
        errno != ERANGE) return 13;
    errno = EDOM;
    if (_strtoui64("-1", NULL, 10) != _UI64_MAX || errno != EDOM)
        return 14;
    if (_strtoi64("-0x2A!", &end, 0) != -42 || *end != '!' ||
        _strtoi64("0779", &end, 0) != 63 || *end != '9') return 15;
    text = "  +!";
    if (_strtoi64(text, &end, 10) != 0 || end != text || errno != EDOM)
        return 16;
    text = "0x";
    if (_strtoi64(text, &end, 0) != 0 || end != text + 1) return 17;
    text = "123";
    errno = 0;
    if (_strtoi64(text, &end, 1) != 0 || end != text || errno != EINVAL)
        return 18;
    errno = 0;
    if (_strtoui64(text, &end, 37) != 0 || end != text || errno != EINVAL)
        return 19;
    errno = 0;
    if (_strtoi64(NULL, &end, 10) != 0 || end != NULL || errno != EINVAL)
        return 20;
    errno = 0;
    if (_strtoui64(NULL, &end, 10) != 0 || end != NULL || errno != EINVAL)
        return 21;
    errno = 0;
    if (_atoi64(NULL) != 0 || errno != EINVAL) return 22;
    errno = 0;
    if (_i64toa(1, NULL, 10) != NULL || errno != EINVAL) return 23;
    errno = 0;
    if (_ui64toa(1, buffer, 0) != NULL || errno != EINVAL) return 24;
    return 0;
}
