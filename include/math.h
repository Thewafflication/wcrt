/**
 * @file math.h
 * @brief Declares the C89 double-precision mathematics library.
 */

#ifndef WCRT_MATH_H
#define WCRT_MATH_H

/** @brief Positive range-error return value. */
#define HUGE_VAL 1.7976931348623157e+308

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

#ifdef __cplusplus
}
#endif

#endif
