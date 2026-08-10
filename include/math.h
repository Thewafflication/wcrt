/**
 * @file math.h
 * @brief Declares the C89 and C99 mathematics library used by WCRT.
 */

#ifndef WCRT_MATH_H
#define WCRT_MATH_H

/** @brief Positive range-error return value. */
#define HUGE_VAL 1.7976931348623157e+308

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
#define HUGE_VALF 3.402823466e+38F
#define HUGE_VALL 1.18973149535723176502e+4932L
#define MATH_ERRNO 1
#define MATH_ERREXCEPT 2
#define math_errhandling MATH_ERRNO
#define FP_NAN 0
#define FP_INFINITE 1
#define FP_ZERO 2
#define FP_SUBNORMAL 3
#define FP_NORMAL 4

#define isfinite(x) (!(isnan(x) || isinf(x)))
#define isnan(x) ((x) != (x))
#define isinf(x) (((x) == HUGE_VAL) || ((x) == -HUGE_VAL) || \
    ((x) == HUGE_VALF) || ((x) == -HUGE_VALF) || \
    ((x) == HUGE_VALL) || ((x) == -HUGE_VALL))
#define signbit(x) (((x) == 0.0) ? (((copysign(1.0, (x))) < 0.0)) : (((x) < 0.0)))
#define fpclassify(x) ((isnan(x)) ? FP_NAN : ((isinf(x)) ? FP_INFINITE : \
    (((x) == 0.0) ? FP_ZERO : FP_NORMAL)))

#define isgreater(x, y) ((x) > (y))
#define isgreaterequal(x, y) ((x) >= (y))
#define isless(x, y) ((x) < (y))
#define islessequal(x, y) ((x) <= (y))
#define islessgreater(x, y) ((x) < (y) || (x) > (y))
#define isunordered(x, y) (isnan(x) || isnan(y))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Returns the principal arc cosine of a value. */
double acos(double value);
/** @brief Returns the principal arc sine of a value. */
double asin(double value);
/** @brief Returns the principal arc tangent of a value. */
double atan(double value);
/** @brief Returns an arc tangent using signs to select a quadrant. */
double atan2(double y, double x);
/** @brief Returns the cosine of an angle in radians. */
double cos(double value);
/** @brief Returns the sine of an angle in radians. */
double sin(double value);
/** @brief Returns the tangent of an angle in radians. */
double tan(double value);
/** @brief Returns the hyperbolic cosine of a value. */
double cosh(double value);
/** @brief Returns the hyperbolic sine of a value. */
double sinh(double value);
/** @brief Returns the hyperbolic tangent of a value. */
double tanh(double value);
/** @brief Returns e raised to a supplied power. */
double exp(double value);
/** @brief Splits a value into a fraction and power of two. */
double frexp(double value, int *exponent);
/** @brief Multiplies a value by an integral power of two. */
double ldexp(double value, int exponent);
/** @brief Returns the natural logarithm of a value. */
double log(double value);
/** @brief Returns the base-ten logarithm of a value. */
double log10(double value);
/** @brief Splits a value into fractional and integral parts. */
double modf(double value, double *integer_part);
/** @brief Raises a base to a supplied power. */
double pow(double base, double power);
/** @brief Returns the nonnegative square root of a value. */
double sqrt(double value);
/** @brief Rounds a value upward to an integral value. */
double ceil(double value);
/** @brief Returns the absolute value of a floating value. */
double fabs(double value);
/** @brief Rounds a value downward to an integral value. */
double floor(double value);
/** @brief Returns the remainder with quotient truncated toward zero. */
double fmod(double numerator, double denominator);

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
float acosf(float value);
float asinf(float value);
float atanf(float value);
float atan2f(float y, float x);
float cosf(float value);
float sinf(float value);
float tanf(float value);
float coshf(float value);
float sinhf(float value);
float tanhf(float value);
float expf(float value);
float frexpf(float value, int *exponent);
float ldexpf(float value, int exponent);
float logf(float value);
float log10f(float value);
float modff(float value, float *integer_part);
float powf(float base, float power);
float sqrtf(float value);
float ceilf(float value);
float fabsf(float value);
float floorf(float value);
float fmodf(float numerator, float denominator);

long double acosl(long double value);
long double asinl(long double value);
long double atanl(long double value);
long double atan2l(long double y, long double x);
long double cosl(long double value);
long double sinl(long double value);
long double tanl(long double value);
long double coshl(long double value);
long double sinhl(long double value);
long double tanhl(long double value);
long double expl(long double value);
long double frexpl(long double value, int *exponent);
long double ldexpl(long double value, int exponent);
long double logl(long double value);
long double log10l(long double value);
long double modfl(long double value, long double *integer_part);
long double powl(long double base, long double power);
long double sqrtl(long double value);
long double ceill(long double value);
long double fabsl(long double value);
long double floorl(long double value);
long double fmodl(long double numerator, long double denominator);

double copysign(double value, double sign);
float copysignf(float value, float sign);
long double copysignl(long double value, long double sign);
double fmax(double lhs, double rhs);
float fmaxf(float lhs, float rhs);
long double fmaxl(long double lhs, long double rhs);
double fmin(double lhs, double rhs);
float fminf(float lhs, float rhs);
long double fminl(long double lhs, long double rhs);
double fdim(double lhs, double rhs);
float fdimf(float lhs, float rhs);
long double fdiml(long double lhs, long double rhs);
double fma(double lhs, double mid, double rhs);
float fmaf(float lhs, float mid, float rhs);
long double fmal(long double lhs, long double mid, long double rhs);
double nextafter(double value, double target);
float nextafterf(float value, float target);
long double nextafterl(long double value, long double target);
float nanf(const char *text);
long double nanl(const char *text);
double nan(const char *text);
#endif

#ifdef __cplusplus
}
#endif

#endif
