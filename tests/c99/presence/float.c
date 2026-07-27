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
    DECIMAL_DIG >= DBL_DIG + 2 ? 1 : -1];
typedef char wcrt_evaluation_method[
    FLT_EVAL_METHOD >= -1 && FLT_EVAL_METHOD <= 2 ? 1 : -1];
