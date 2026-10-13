/**
 * @file i64-format.c
 * @brief Exercises Microsoft I64 integer formatting and scanning.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/** @brief Calls the legacy va-list output entry point. */
static int format_list(char *buffer, size_t size, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = _vsnprintf(buffer, size, format, arguments);
    va_end(arguments);
    return result;
}

int main(void)
{
    char buffer[160];
    long long minimum = -9223372036854775807LL - 1;
    long long maximum = 9223372036854775807LL;
    unsigned long long umax = 18446744073709551615ULL;
    long long signed_value = 0;
    unsigned long long unsigned_value = 0;
    long long count = -1;
    int trailing = 0;
    FILE *stream;

    if (sprintf(buffer, "%I64d|%I64i|%d", minimum, maximum, 42) != 43 ||
        strcmp(buffer, "-9223372036854775808|9223372036854775807|42") != 0)
        return 1;
    if (sprintf(buffer, "%I64u|%I64x|%I64X", umax, umax, umax) != 54 ||
        strcmp(buffer, "18446744073709551615|ffffffffffffffff|FFFFFFFFFFFFFFFF")
        != 0) return 2;
    sprintf(buffer, "%I64o", umax);
    if (strcmp(buffer, "1777777777777777777777") != 0) return 3;
    sprintf(buffer, "%+#024I64X|%+024I64d", umax, -42LL);
    if (strcmp(buffer,
        "0X000000FFFFFFFFFFFFFFFF|-00000000000000000000042") != 0) return 4;
    sprintf(buffer, "%*.*I64d|%-6I64u", 8, 5, 42LL, 7ULL);
    if (strcmp(buffer, "   00042|7     ") != 0) return 5;
    if (format_list(buffer, sizeof(buffer), "%I64d/%d", maximum, 9) != 21 ||
        strcmp(buffer, "9223372036854775807/9") != 0) return 6;
    memset(buffer, 'Z', sizeof(buffer));
    if (_snprintf(buffer, 4, "%I64u", umax) != -1 ||
        memcmp(buffer, "1844Z", 5) != 0) return 7;
    if (_snprintf(NULL, 0, "%I64d", minimum) != 20) return 8;
    if (sprintf(buffer, "abc%I64n%d", &count, 7) != 4 || count != 3 ||
        strcmp(buffer, "abc7") != 0) return 9;
    if (sscanf("-9223372036854775808 42", "%I64d %d", &signed_value,
        &trailing) != 2 || signed_value != minimum || trailing != 42) return 10;
    if (sscanf("18446744073709551615", "%I64u", &unsigned_value) != 1 ||
        unsigned_value != umax) return 11;
    if (sscanf("0xffffffffffffffff", "%I64x", &unsigned_value) != 1 ||
        unsigned_value != umax) return 12;
    if (sscanf("1777777777777777777777", "%I64o", &unsigned_value) != 1 ||
        unsigned_value != umax) return 13;
    if (sscanf("-0x2a", "%I64i", &signed_value) != 1 || signed_value != -42)
        return 14;
    count = -1;
    if (sscanf("12345x", "%3I64d%I64n%d", &signed_value, &count,
        &trailing) != 2 || signed_value != 123 || count != 3 || trailing != 45)
        return 15;
    if (sscanf("4294967296 17", "%*I64u%d", &trailing) != 1 || trailing != 17)
        return 16;
    signed_value = 123;
    if (sscanf("x", "%I64d", &signed_value) != 0 || signed_value != 123)
        return 17;
    stream = tmpfile();
    if (stream == NULL) return 18;
    if (fprintf(stream, "%I64d!", minimum) != 21) return 19;
    rewind(stream);
    if (fscanf(stream, "%I64d", &signed_value) != 1 ||
        signed_value != minimum || fgetc(stream) != '!') return 20;
    if (fclose(stream) != 0) return 21;
    if (sprintf(buffer, "%I") != -1 || sprintf(buffer, "%I6") != -1 ||
        sprintf(buffer, "%I64") != -1) return 22;
    return 0;
}
