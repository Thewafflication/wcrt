/**
 * @file integer-string.c
 * @brief Verifies Microsoft integer-string conversion behavior.
 */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char buffer[40];

    if (_itoa(0, buffer, 10) != buffer || strcmp(buffer, "0") != 0)
        return 1;
    if (_itoa(-42, buffer, 10) != buffer || strcmp(buffer, "-42") != 0)
        return 2;
    if (_itoa(-1, buffer, 16) != buffer ||
        strcmp(buffer, "ffffffff") != 0) return 3;
    if (_itoa(INT_MIN, buffer, 10) != buffer ||
        strcmp(buffer, "-2147483648") != 0) return 4;
    if (_ltoa(LONG_MIN, buffer, 10) != buffer ||
        strcmp(buffer, "-2147483648") != 0) return 5;
    if (_ultoa(ULONG_MAX, buffer, 10) != buffer ||
        strcmp(buffer, "4294967295") != 0) return 6;
    if (_ultoa(35UL, buffer, 36) != buffer || strcmp(buffer, "z") != 0)
        return 7;
    if (_ultoa(10UL, buffer, 2) != buffer || strcmp(buffer, "1010") != 0)
        return 8;
    errno = 0;
    if (_itoa(1, buffer, 1) != 0 || errno != EINVAL) return 9;
    errno = 0;
    if (_ltoa(1L, buffer, 37) != 0 || errno != EINVAL) return 10;
    errno = 0;
    if (_ultoa(1UL, 0, 10) != 0 || errno != EINVAL) return 11;
    return 0;
}
