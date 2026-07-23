/**
 * @file float.c
 * @brief Validates C89 floating-point model and limit macros.
 */

#include <float.h>

/**
 * @brief Checks model constraints and epsilon behavior.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    volatile float float_one = 1.0F;
    volatile float float_epsilon = FLT_EPSILON;
    volatile double double_one = 1.0;
    volatile double double_epsilon = DBL_EPSILON;
    volatile long double long_one = 1.0L;
    volatile long double long_epsilon = LDBL_EPSILON;

    if (FLT_RADIX < 2 || FLT_ROUNDS < -1 || FLT_ROUNDS > 3) return 1;
    if (FLT_MANT_DIG < 6 || DBL_MANT_DIG < 10) return 2;
    if (LDBL_MANT_DIG < DBL_MANT_DIG) return 3;
    if (FLT_DIG < 6 || DBL_DIG < 10 || LDBL_DIG < 10) return 4;
    if (FLT_MIN_EXP > -37 || DBL_MIN_EXP > -37) return 5;
    if (LDBL_MIN_EXP > -37) return 6;
    if (FLT_MAX_EXP < 37 || DBL_MAX_EXP < 37) return 7;
    if (LDBL_MAX_EXP < 37) return 8;
    if (FLT_MIN <= 0.0F || DBL_MIN <= 0.0) return 9;
    if (LDBL_MIN <= 0.0L) return 10;
    if (FLT_MAX <= 1.0F || DBL_MAX <= 1.0) return 11;
    if (LDBL_MAX <= 1.0L) return 12;
    if (float_one + float_epsilon == float_one) return 13;
    if (double_one + double_epsilon == double_one) return 14;
    if (long_one + long_epsilon == long_one) return 15;
    if (float_one + float_epsilon / 2.0F != float_one) return 16;
    if (double_one + double_epsilon / 2.0 != double_one) return 17;
    if (long_one + long_epsilon / 2.0L != long_one) return 18;
    if (FLT_MIN_10_EXP >= 0 || DBL_MIN_10_EXP >= 0) return 19;
    if (LDBL_MIN_10_EXP >= 0) return 20;
    if (FLT_MAX_10_EXP <= 0 || DBL_MAX_10_EXP <= 0) return 21;
    if (LDBL_MAX_10_EXP <= 0) return 22;
    return 0;
}
