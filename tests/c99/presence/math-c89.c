/**
 * @file math-c89.c
 * @brief Verifies that the C99 real-math additions are absent in C89 mode.
 */
#include <math.h>

#if defined(HUGE_VALF) || defined(HUGE_VALL) || defined(MATH_ERRNO) || \
    defined(MATH_ERREXCEPT) || defined(isfinite)
#error C99 mathematics macros must not be exposed in C89 mode
#endif
