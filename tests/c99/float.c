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
    volatile long double one_long_double = 1.0L;

    if (DECIMAL_DIG < 10) return 1;
    if (DECIMAL_DIG < DBL_DIG + 2) return 2;
    if (FLT_EVAL_METHOD < -1 || FLT_EVAL_METHOD > 2) return 3;
    if (FLT_ROUNDS < -1 || FLT_ROUNDS > 3) return 4;
    if (one_float + FLT_EPSILON == one_float) return 5;
    if (one_double + DBL_EPSILON == one_double) return 6;
    if (one_long_double + LDBL_EPSILON == one_long_double) return 7;
    return 0;
}
