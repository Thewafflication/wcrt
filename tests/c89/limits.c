/**
 * @file limits.c
 * @brief Validates C89 integer limits against target representations.
 */

#include <limits.h>

/**
 * @brief Runs integer range, signedness, and storage checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    signed char signed_character = SCHAR_MIN;
    unsigned char unsigned_character = UCHAR_MAX;
    short signed_short = SHRT_MIN;
    unsigned short unsigned_short = USHRT_MAX;
    int signed_integer = INT_MIN;
    unsigned int unsigned_integer = UINT_MAX;
    long signed_long = LONG_MIN;
    unsigned long unsigned_long = ULONG_MAX;

    if (CHAR_BIT != 8 || MB_LEN_MAX < 1) return 1;
    if (sizeof(char) != 1 || sizeof(signed char) != 1) return 2;
    if (sizeof(unsigned char) != 1) return 3;
    if (sizeof(short) * CHAR_BIT != 16) return 4;
    if (sizeof(int) * CHAR_BIT != 32) return 5;
    if (sizeof(long) * CHAR_BIT != 32) return 6;
    if (SCHAR_MIN != -SCHAR_MAX - 1) return 7;
    if (SHRT_MIN != -SHRT_MAX - 1) return 8;
    if (INT_MIN != -INT_MAX - 1) return 9;
    if (LONG_MIN != -LONG_MAX - 1L) return 10;
    if (UCHAR_MAX != (unsigned char)-1) return 11;
    if (USHRT_MAX != (unsigned short)-1) return 12;
    if (UINT_MAX != (unsigned int)-1) return 13;
    if (ULONG_MAX != (unsigned long)-1) return 14;
    if (((char)-1 < 0) != (CHAR_MIN < 0)) return 15;
    if (signed_character != SCHAR_MIN) return 16;
    if (unsigned_character != UCHAR_MAX) return 17;
    if (signed_short != SHRT_MIN || unsigned_short != USHRT_MAX) return 18;
    if (signed_integer != INT_MIN || unsigned_integer != UINT_MAX) return 19;
    if (signed_long != LONG_MIN || unsigned_long != ULONG_MAX) return 20;
    return 0;
}
