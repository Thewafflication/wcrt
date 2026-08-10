/**
 * @file math.c
 * @brief Verifies the public C99 real-math declarations and classification macros.
 */
#include <math.h>

#ifndef HUGE_VALF
#error HUGE_VALF is required in C99 mode
#endif
#ifndef HUGE_VALL
#error HUGE_VALL is required in C99 mode
#endif
#ifndef MATH_ERRNO
#error MATH_ERRNO is required in C99 mode
#endif
#ifndef MATH_ERREXCEPT
#error MATH_ERREXCEPT is required in C99 mode
#endif
#ifndef isfinite
#error isfinite is required in C99 mode
#endif
#ifndef isnan
#error isnan is required in C99 mode
#endif
#ifndef isinf
#error isinf is required in C99 mode
#endif
#ifndef signbit
#error signbit is required in C99 mode
#endif
#ifndef fpclassify
#error fpclassify is required in C99 mode
#endif

int main(void)
{
    int wcrt_math_errhandling = math_errhandling;
    int wcrt_finite = isfinite(1.0);
    int wcrt_nan = isnan(0.0 / 0.0);
    int wcrt_inf = isinf(HUGE_VAL);
    int wcrt_sign = signbit(-0.0);
    int wcrt_class = fpclassify(1.0);

    return (wcrt_math_errhandling == MATH_ERRNO &&
            wcrt_finite && wcrt_nan && wcrt_inf &&
            wcrt_sign && wcrt_class == FP_NORMAL) ? 0 : 1;
}
