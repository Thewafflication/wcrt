/**
 * @file iso646.c
 * @brief Exercises C99 alternative operator spellings.
 */
#include <iso646.h>

/** @brief Runs alternative-token equivalence and evaluation checks. */
int main(void)
{
    unsigned int value = 3U;
    int left = 0;
    int right = 0;

    if ((1 and 0) != (1 && 0)) return 1;
    if ((1 or 0) != (1 || 0)) return 2;
    if ((not 0) != (!0) || (1 not_eq 0) != (1 != 0)) return 3;
    if ((6 bitand 3) != (6 & 3)) return 4;
    if ((4 bitor 1) != (4 | 1)) return 5;
    if ((6 xor 3) != (6 ^ 3) || (compl 0U) != (~0U)) return 6;
    value and_eq 6U;
    if (value != 2U) return 7;
    value or_eq 1U;
    if (value != 3U) return 8;
    value xor_eq 7U;
    if (value != 4U) return 9;
    if ((left++ and right++) != 0 || left != 1 || right != 0) return 10;
    return 0;
}
