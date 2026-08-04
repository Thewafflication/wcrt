/**
 * @file float.c
 * @brief Exercises C99 floating-point model declarations.
 */
#include <float.h>

/** @brief Runs floating-model consistency checks. */
int main(void)
{
    volatile float one_float = 1.0F;
    volatile double one_double = 1.0;
    volatile double half_double_epsilon = DBL_EPSILON / 2.0;
    volatile long double one_long_double = 1.0L;

    if (DECIMAL_DIG != 17) return 1;
    if (FLT_EVAL_METHOD != 0) return 2;
    if (FLT_ROUNDS != 1) return 3;
    if (LDBL_MANT_DIG != DBL_MANT_DIG || sizeof(long double) != 8) return 4;
    if (one_float + FLT_EPSILON == one_float) return 5;
    if (one_double + DBL_EPSILON == one_double) return 6;
    if (one_long_double + LDBL_EPSILON == one_long_double) return 7;
    if (one_double + half_double_epsilon != one_double) return 8;
    return 0;
}
