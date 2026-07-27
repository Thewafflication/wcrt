/**
 * @file limits.c
 * @brief Exercises C99 long-long limits.
 */
#include <limits.h>

/** @brief Runs long-long range and representation checks. */
int main(void)
{
    long long minimum = LLONG_MIN;
    long long maximum = LLONG_MAX;
    unsigned long long unsigned_maximum = ULLONG_MAX;

    if (sizeof(long long) * CHAR_BIT < 64) return 1;
    if (minimum != -maximum - 1LL) return 2;
    if (unsigned_maximum != (unsigned long long)-1) return 3;
    if (maximum < 9223372036854775807LL) return 4;
    if (unsigned_maximum < 18446744073709551615ULL) return 5;
    return 0;
}
