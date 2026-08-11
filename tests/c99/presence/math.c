/**
 * @file math.c
 * @brief Compiles the complete supported C99 real-mathematics inventory.
 */

#include <math.h>

#if !defined(HUGE_VALF) || !defined(HUGE_VALL) || !defined(INFINITY) || \
    !defined(NAN) || !defined(MATH_ERRNO) || !defined(MATH_ERREXCEPT)
#error C99 mathematics constants are required
#endif
#if !defined(FP_NAN) || !defined(FP_INFINITE) || !defined(FP_ZERO) || \
    !defined(FP_SUBNORMAL) || !defined(FP_NORMAL) || !defined(FP_ILOGB0) || \
    !defined(FP_ILOGBNAN)
#error C99 classification constants are required
#endif
#if !defined(fpclassify) || !defined(isfinite) || !defined(isinf) || \
    !defined(isnan) || !defined(isnormal) || !defined(signbit) || \
    !defined(isgreater) || !defined(isgreaterequal) || !defined(isless) || \
    !defined(islessequal) || !defined(islessgreater) || !defined(isunordered)
#error C99 inquiry and comparison macros are required
#endif

#define WCRT_TAKE_UNARY(name) do { \
    unary_d = name; unary_f = name##f; unary_l = name##l; \
} while (0)
#define WCRT_TAKE_BINARY(name) do { \
    binary_d = name; binary_f = name##f; binary_l = name##l; \
} while (0)

void wcrt_math_presence(void)
{
    double (*unary_d)(double);
    float (*unary_f)(float);
    long double (*unary_l)(long double);
    double (*binary_d)(double, double);
    float (*binary_f)(float, float);
    long double (*binary_l)(long double, long double);
    double (*frexp_d)(double, int *) = frexp;
    float (*frexp_f)(float, int *) = frexpf;
    long double (*frexp_l)(long double, int *) = frexpl;
    double (*modf_d)(double, double *) = modf;
    float (*modf_f)(float, float *) = modff;
    long double (*modf_l)(long double, long double *) = modfl;
    double (*nan_d)(const char *) = nan;
    float (*nan_f)(const char *) = nanf;
    long double (*nan_l)(const char *) = nanl;
    double (*scale_d)(double, int) = scalbn;
    float (*scale_f)(float, int) = scalbnf;
    long double (*scale_l)(long double, int) = scalbnl;
    double (*long_scale_d)(double, long) = scalbln;
    float (*long_scale_f)(float, long) = scalblnf;
    long double (*long_scale_l)(long double, long) = scalblnl;
    int (*ilog_d)(double) = ilogb;
    int (*ilog_f)(float) = ilogbf;
    int (*ilog_l)(long double) = ilogbl;
    long (*long_d)(double) = lrint;
    long (*long_f)(float) = lrintf;
    long (*long_l)(long double) = lrintl;
    long long (*llong_d)(double) = llrint;
    long long (*llong_f)(float) = llrintf;
    long long (*llong_l)(long double) = llrintl;
    double (*next_d)(double, long double) = nexttoward;
    float (*next_f)(float, long double) = nexttowardf;
    long double (*next_l)(long double, long double) = nexttowardl;
    double (*ternary_d)(double, double, double) = fma;
    float (*ternary_f)(float, float, float) = fmaf;
    long double (*ternary_l)(long double, long double, long double) = fmal;
    double (*remquo_d)(double, double, int *) = remquo;
    float (*remquo_f)(float, float, int *) = remquof;
    long double (*remquo_l)(long double, long double, int *) = remquol;

    WCRT_TAKE_UNARY(acos); WCRT_TAKE_UNARY(asin); WCRT_TAKE_UNARY(atan);
    WCRT_TAKE_BINARY(atan2); WCRT_TAKE_UNARY(cos); WCRT_TAKE_UNARY(sin);
    WCRT_TAKE_UNARY(tan); WCRT_TAKE_UNARY(acosh); WCRT_TAKE_UNARY(asinh);
    WCRT_TAKE_UNARY(atanh); WCRT_TAKE_UNARY(cosh); WCRT_TAKE_UNARY(sinh);
    WCRT_TAKE_UNARY(tanh); WCRT_TAKE_UNARY(exp); WCRT_TAKE_UNARY(exp2);
    WCRT_TAKE_UNARY(expm1); WCRT_TAKE_UNARY(log); WCRT_TAKE_UNARY(log10);
    WCRT_TAKE_UNARY(log1p); WCRT_TAKE_UNARY(log2); WCRT_TAKE_UNARY(logb);
    WCRT_TAKE_UNARY(cbrt); WCRT_TAKE_BINARY(hypot); WCRT_TAKE_BINARY(pow);
    WCRT_TAKE_UNARY(sqrt); WCRT_TAKE_UNARY(fabs);
    WCRT_TAKE_UNARY(erf); WCRT_TAKE_UNARY(erfc);
    WCRT_TAKE_UNARY(lgamma); WCRT_TAKE_UNARY(tgamma);
    WCRT_TAKE_UNARY(ceil); WCRT_TAKE_UNARY(floor);
    WCRT_TAKE_UNARY(nearbyint); WCRT_TAKE_UNARY(rint);
    WCRT_TAKE_UNARY(round); WCRT_TAKE_UNARY(trunc);
    WCRT_TAKE_BINARY(fmod); WCRT_TAKE_BINARY(remainder);
    WCRT_TAKE_BINARY(copysign); WCRT_TAKE_BINARY(nextafter);
    WCRT_TAKE_BINARY(fdim); WCRT_TAKE_BINARY(fmax); WCRT_TAKE_BINARY(fmin);

    scale_d = ldexp; scale_f = ldexpf; scale_l = ldexpl;
    long_d = lround; long_f = lroundf; long_l = lroundl;
    llong_d = llround; llong_f = llroundf; llong_l = llroundl;

    (void)scale_d; (void)scale_f; (void)scale_l;
    (void)long_scale_d; (void)long_scale_f; (void)long_scale_l;
    (void)ilog_d; (void)ilog_f; (void)ilog_l;
    (void)long_d; (void)long_f; (void)long_l;
    (void)llong_d; (void)llong_f; (void)llong_l;
    (void)next_d; (void)next_f; (void)next_l;
    (void)ternary_d; (void)ternary_f; (void)ternary_l;
    (void)frexp_d; (void)frexp_f; (void)frexp_l;
    (void)modf_d; (void)modf_f; (void)modf_l;
    (void)nan_d; (void)nan_f; (void)nan_l;
    (void)remquo_d; (void)remquo_f; (void)remquo_l;
    if (math_errhandling != MATH_ERRNO) unary_d = 0;
    (void)unary_d; (void)unary_f; (void)unary_l;
    (void)binary_d; (void)binary_f; (void)binary_l;
}
