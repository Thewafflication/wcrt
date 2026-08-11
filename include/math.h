/**
 * @file math.h
 * @brief Declares the C89 and C99 mathematics library used by WCRT.
 */

#ifndef WCRT_MATH_H
#define WCRT_MATH_H

/** @brief Finite positive range-error value retained by the C89 contract. */
#define HUGE_VAL 1.7976931348623157e+308

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
#define HUGE_VALF 3.40282347e+38F
#define HUGE_VALL 1.7976931348623157e+308L
#define INFINITY (1.0F / 0.0F)
#define NAN (0.0F / 0.0F)

#define MATH_ERRNO 1
#define MATH_ERREXCEPT 2
#define math_errhandling MATH_ERRNO

#define FP_NAN 0
#define FP_INFINITE 1
#define FP_ZERO 2
#define FP_SUBNORMAL 3
#define FP_NORMAL 4
#define FP_ILOGB0 (-2147483647 - 1)
#define FP_ILOGBNAN (-2147483647 - 1)
#endif

#ifdef __cplusplus
extern "C" {
#endif

double acos(double value);
double asin(double value);
double atan(double value);
double atan2(double y, double x);
double cos(double value);
double sin(double value);
double tan(double value);
double cosh(double value);
double sinh(double value);
double tanh(double value);
double exp(double value);
double frexp(double value, int *exponent);
double ldexp(double value, int exponent);
double log(double value);
double log10(double value);
double modf(double value, double *integer_part);
double pow(double base, double power);
double sqrt(double value);
double ceil(double value);
double fabs(double value);
double floor(double value);
double fmod(double numerator, double denominator);

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L

/* Implementation helpers used by the single-evaluation inquiry macros. */
int __wcrt_fpclassifyf(float value);
int __wcrt_fpclassify(double value);
int __wcrt_signbitf(float value);
int __wcrt_signbit(double value);
int __wcrt_isgreaterf(float lhs, float rhs);
int __wcrt_isgreater(double lhs, double rhs);
int __wcrt_isgreaterequalf(float lhs, float rhs);
int __wcrt_isgreaterequal(double lhs, double rhs);
int __wcrt_islessf(float lhs, float rhs);
int __wcrt_isless(double lhs, double rhs);
int __wcrt_islessequalf(float lhs, float rhs);
int __wcrt_islessequal(double lhs, double rhs);
int __wcrt_islessgreaterf(float lhs, float rhs);
int __wcrt_islessgreater(double lhs, double rhs);
int __wcrt_isunorderedf(float lhs, float rhs);
int __wcrt_isunordered(double lhs, double rhs);

float acosf(float value);
long double acosl(long double value);
float asinf(float value);
long double asinl(long double value);
float atanf(float value);
long double atanl(long double value);
float atan2f(float y, float x);
long double atan2l(long double y, long double x);
float cosf(float value);
long double cosl(long double value);
float sinf(float value);
long double sinl(long double value);
float tanf(float value);
long double tanl(long double value);

double acosh(double value);
float acoshf(float value);
long double acoshl(long double value);
double asinh(double value);
float asinhf(float value);
long double asinhl(long double value);
double atanh(double value);
float atanhf(float value);
long double atanhl(long double value);
float coshf(float value);
long double coshl(long double value);
float sinhf(float value);
long double sinhl(long double value);
float tanhf(float value);
long double tanhl(long double value);

float expf(float value);
long double expl(long double value);
double exp2(double value);
float exp2f(float value);
long double exp2l(long double value);
double expm1(double value);
float expm1f(float value);
long double expm1l(long double value);
float frexpf(float value, int *exponent);
long double frexpl(long double value, int *exponent);
int ilogb(double value);
int ilogbf(float value);
int ilogbl(long double value);
float ldexpf(float value, int exponent);
long double ldexpl(long double value, int exponent);
float logf(float value);
long double logl(long double value);
float log10f(float value);
long double log10l(long double value);
double log1p(double value);
float log1pf(float value);
long double log1pl(long double value);
double log2(double value);
float log2f(float value);
long double log2l(long double value);
double logb(double value);
float logbf(float value);
long double logbl(long double value);
float modff(float value, float *integer_part);
long double modfl(long double value, long double *integer_part);
double scalbn(double value, int exponent);
float scalbnf(float value, int exponent);
long double scalbnl(long double value, int exponent);
double scalbln(double value, long exponent);
float scalblnf(float value, long exponent);
long double scalblnl(long double value, long exponent);

double cbrt(double value);
float cbrtf(float value);
long double cbrtl(long double value);
float fabsf(float value);
long double fabsl(long double value);
double hypot(double lhs, double rhs);
float hypotf(float lhs, float rhs);
long double hypotl(long double lhs, long double rhs);
float powf(float base, float power);
long double powl(long double base, long double power);
float sqrtf(float value);
long double sqrtl(long double value);

double erf(double value);
float erff(float value);
long double erfl(long double value);
double erfc(double value);
float erfcf(float value);
long double erfcl(long double value);
double lgamma(double value);
float lgammaf(float value);
long double lgammal(long double value);
double tgamma(double value);
float tgammaf(float value);
long double tgammal(long double value);

float ceilf(float value);
long double ceill(long double value);
float floorf(float value);
long double floorl(long double value);
double nearbyint(double value);
float nearbyintf(float value);
long double nearbyintl(long double value);
double rint(double value);
float rintf(float value);
long double rintl(long double value);
long lrint(double value);
long lrintf(float value);
long lrintl(long double value);
long long llrint(double value);
long long llrintf(float value);
long long llrintl(long double value);
double round(double value);
float roundf(float value);
long double roundl(long double value);
long lround(double value);
long lroundf(float value);
long lroundl(long double value);
long long llround(double value);
long long llroundf(float value);
long long llroundl(long double value);
double trunc(double value);
float truncf(float value);
long double truncl(long double value);

float fmodf(float numerator, float denominator);
long double fmodl(long double numerator, long double denominator);
double remainder(double numerator, double denominator);
float remainderf(float numerator, float denominator);
long double remainderl(long double numerator, long double denominator);
double remquo(double numerator, double denominator, int *quotient);
float remquof(float numerator, float denominator, int *quotient);
long double remquol(long double numerator, long double denominator,
    int *quotient);

double copysign(double value, double sign);
float copysignf(float value, float sign);
long double copysignl(long double value, long double sign);
double nan(const char *tag);
float nanf(const char *tag);
long double nanl(const char *tag);
double nextafter(double value, double direction);
float nextafterf(float value, float direction);
long double nextafterl(long double value, long double direction);
double nexttoward(double value, long double direction);
float nexttowardf(float value, long double direction);
long double nexttowardl(long double value, long double direction);

double fdim(double lhs, double rhs);
float fdimf(float lhs, float rhs);
long double fdiml(long double lhs, long double rhs);
double fmax(double lhs, double rhs);
float fmaxf(float lhs, float rhs);
long double fmaxl(long double lhs, long double rhs);
double fmin(double lhs, double rhs);
float fminf(float lhs, float rhs);
long double fminl(long double lhs, long double rhs);
double fma(double lhs, double middle, double rhs);
float fmaf(float lhs, float middle, float rhs);
long double fmal(long double lhs, long double middle, long double rhs);

#endif

#ifdef __cplusplus
}
#endif

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L

#define __WCRT_FLOAT_OPERAND(x) (sizeof(x) == sizeof(float))
#define __WCRT_FLOAT_PAIR(x, y) (sizeof((x) + (y)) == sizeof(float))

#define fpclassify(x) (__WCRT_FLOAT_OPERAND(x) ? \
    __wcrt_fpclassifyf((float)(x)) : __wcrt_fpclassify((double)(x)))
#define isfinite(x) (fpclassify(x) >= FP_ZERO)
#define isinf(x) (fpclassify(x) == FP_INFINITE)
#define isnan(x) (fpclassify(x) == FP_NAN)
#define isnormal(x) (fpclassify(x) == FP_NORMAL)
#define signbit(x) (__WCRT_FLOAT_OPERAND(x) ? \
    __wcrt_signbitf((float)(x)) : __wcrt_signbit((double)(x)))

#define isgreater(x, y) (__WCRT_FLOAT_PAIR(x, y) ? \
    __wcrt_isgreaterf((float)(x), (float)(y)) : \
    __wcrt_isgreater((double)(x), (double)(y)))
#define isgreaterequal(x, y) (__WCRT_FLOAT_PAIR(x, y) ? \
    __wcrt_isgreaterequalf((float)(x), (float)(y)) : \
    __wcrt_isgreaterequal((double)(x), (double)(y)))
#define isless(x, y) (__WCRT_FLOAT_PAIR(x, y) ? \
    __wcrt_islessf((float)(x), (float)(y)) : \
    __wcrt_isless((double)(x), (double)(y)))
#define islessequal(x, y) (__WCRT_FLOAT_PAIR(x, y) ? \
    __wcrt_islessequalf((float)(x), (float)(y)) : \
    __wcrt_islessequal((double)(x), (double)(y)))
#define islessgreater(x, y) (__WCRT_FLOAT_PAIR(x, y) ? \
    __wcrt_islessgreaterf((float)(x), (float)(y)) : \
    __wcrt_islessgreater((double)(x), (double)(y)))
#define isunordered(x, y) (__WCRT_FLOAT_PAIR(x, y) ? \
    __wcrt_isunorderedf((float)(x), (float)(y)) : \
    __wcrt_isunordered((double)(x), (double)(y)))

#endif

#endif
