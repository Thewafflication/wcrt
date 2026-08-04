/**
 * @file float.c
 * @brief Verifies the compile-time C99 floating-point characteristics.
 */
#include <float.h>

#ifndef DECIMAL_DIG
#error DECIMAL_DIG is required in C99 mode
#endif
#ifndef FLT_EVAL_METHOD
#error FLT_EVAL_METHOD is required in C99 mode
#endif

typedef char wcrt_decimal_digits[
    DECIMAL_DIG == 17 ? 1 : -1];
typedef char wcrt_evaluation_method[
    FLT_EVAL_METHOD == 0 ? 1 : -1];
typedef char wcrt_rounding_method[FLT_ROUNDS == 1 ? 1 : -1];
typedef char wcrt_binary64_long_double[
    LDBL_MANT_DIG == 53 && sizeof(long double) == 8 ? 1 : -1];
