/**
 * @file float-c89.c
 * @brief Verifies that C99 floating additions are absent in C89 mode.
 */
#include <float.h>

#if defined(DECIMAL_DIG) || defined(FLT_EVAL_METHOD)
#error C99 floating macros must not be exposed in C89 mode
#endif

static int wcrt_c89_rounding_model = FLT_ROUNDS;

