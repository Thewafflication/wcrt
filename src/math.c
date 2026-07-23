/**
 * @file math.c
 * @brief Implements the C89 binary64 mathematics library without a host CRT.
 * @details Ordinary finite results target about 12 significant decimal digits.
 */

#include <errno.h>
#include <float.h>
#include <math.h>

/** @brief High-precision pi constant used for range reduction. */
#define WCRT_PI 3.14159265358979323846
/** @brief High-precision half-pi constant. */
#define WCRT_HALF_PI 1.57079632679489661923
/** @brief High-precision quarter-pi constant. */
#define WCRT_QUARTER_PI 0.78539816339744830962
/** @brief High-precision two-pi constant. */
#define WCRT_TWO_PI 6.28318530717958647692
/** @brief Natural logarithm of two. */
#define WCRT_LN2 0.69314718055994530942
/** @brief Natural logarithm of ten. */
#define WCRT_LN10 2.30258509299404568402

/** @brief Binary64 representation used for exact integral splitting. */
union wcrt_double_shape {
    double value;              /**< Floating representation. */
    struct {
        unsigned long low;     /**< Low-order 32 bits. */
        unsigned long high;    /**< High-order 32 bits. */
    } words;                   /**< Little-endian Windows words. */
};

/** @brief Returns a signed zero with the sign of a supplied value. */
static double wcrt_signed_zero(double value)
{
    return value * 0.0;
}

/** @brief Tests whether a binary64 value is not a number. */
static int wcrt_is_nan(double value)
{
    return value != value;
}

/** @brief Reduces an angle to the interval centered on zero. */
static double wcrt_reduce_angle(double value)
{
    double quotient;
    double integral;

    quotient = value / WCRT_TWO_PI;
    if (quotient >= 0.0) {
        integral = floor(quotient + 0.5);
    } else {
        integral = ceil(quotient - 0.5);
    }
    return value - integral * WCRT_TWO_PI;
}

/** @brief Evaluates the sine series on the reduced interval. */
static double wcrt_sine_series(double value)
{
    double term = value;
    double sum = value;
    double square = value * value;
    int index;

    for (index = 1; index < 14; ++index) {
        term *= -square / ((2 * index) * (2 * index + 1));
        sum += term;
    }
    return sum;
}

/** @brief Evaluates the cosine series on the reduced interval. */
static double wcrt_cosine_series(double value)
{
    double term = 1.0;
    double sum = 1.0;
    double square = value * value;
    int index;

    for (index = 1; index < 14; ++index) {
        term *= -square / ((2 * index - 1) * (2 * index));
        sum += term;
    }
    return sum;
}

/** @brief Evaluates the arc-tangent series for small magnitudes. */
static double wcrt_atan_series(double value)
{
    double square = value * value;
    double term = value;
    double sum = value;
    int index;

    for (index = 1; index < 28; ++index) {
        term *= -square;
        sum += term / (2 * index + 1);
    }
    return sum;
}

/** @brief Implements the public fabs contract. */
double fabs(double value)
{
    return value < 0.0 ? -value : value;
}

/** @brief Implements the public modf contract. */
double modf(double value, double *integer_part)
{
    union wcrt_double_shape shape;
    int exponent;
    unsigned long mask;

    shape.value = value;
    exponent = (int)((shape.words.high >> 20) & 0x7ffUL) - 1023;
    if (exponent < 0) {
        *integer_part = wcrt_signed_zero(value);
        return value;
    }
    if (exponent >= 52) {
        *integer_part = value;
        return wcrt_signed_zero(value);
    }
    if (exponent < 20) {
        mask = (1UL << (20 - exponent)) - 1UL;
        shape.words.high &= ~mask;
        shape.words.low = 0UL;
    } else if (exponent == 20) {
        shape.words.low = 0UL;
    } else {
        mask = (1UL << (52 - exponent)) - 1UL;
        shape.words.low &= ~mask;
    }
    *integer_part = shape.value;
    return value - shape.value;
}

/** @brief Implements the public floor contract. */
double floor(double value)
{
    double integer_part;
    double fraction = modf(value, &integer_part);

    return fraction < 0.0 ? integer_part - 1.0 : integer_part;
}

/** @brief Implements the public ceil contract. */
double ceil(double value)
{
    double integer_part;
    double fraction = modf(value, &integer_part);

    return fraction > 0.0 ? integer_part + 1.0 : integer_part;
}

/** @brief Implements the public frexp contract. */
double frexp(double value, int *exponent)
{
    double magnitude = fabs(value);
    int result_exponent = 0;

    if (value == 0.0 || wcrt_is_nan(value)) {
        *exponent = 0;
        return value;
    }
    while (magnitude >= 1.0) {
        magnitude *= 0.5;
        ++result_exponent;
    }
    while (magnitude < 0.5) {
        magnitude *= 2.0;
        --result_exponent;
    }
    *exponent = result_exponent;
    return value < 0.0 ? -magnitude : magnitude;
}

/** @brief Implements the public ldexp contract. */
double ldexp(double value, int exponent)
{
    if (value == 0.0 || wcrt_is_nan(value)) {
        return value;
    }
    while (exponent > 0) {
        if (fabs(value) > DBL_MAX / 2.0) {
            errno = ERANGE;
            return value < 0.0 ? -HUGE_VAL : HUGE_VAL;
        }
        value *= 2.0;
        --exponent;
    }
    while (exponent < 0) {
        if (fabs(value) < DBL_MIN * 2.0) {
            errno = ERANGE;
            return wcrt_signed_zero(value);
        }
        value *= 0.5;
        ++exponent;
    }
    return value;
}

/** @brief Implements the public sqrt contract. */
double sqrt(double value)
{
    double estimate;
    int exponent;
    int index;

    if (value < 0.0) {
        errno = EDOM;
        return 0.0;
    }
    if (value == 0.0 || wcrt_is_nan(value)) {
        return value;
    }
    frexp(value, &exponent);
    estimate = ldexp(1.0, exponent / 2);
    for (index = 0; index < 12; ++index) {
        estimate = 0.5 * (estimate + value / estimate);
    }
    return estimate;
}

/** @brief Implements the public exp contract. */
double exp(double value)
{
    double reduced;
    double term = 1.0;
    double sum = 1.0;
    int power;
    int index;

    if (value > 709.782712893384) {
        errno = ERANGE;
        return HUGE_VAL;
    }
    if (value < -708.3964185322641) {
        errno = ERANGE;
        return 0.0;
    }
    power = (int)(value / WCRT_LN2 + (value >= 0.0 ? 0.5 : -0.5));
    reduced = value - power * WCRT_LN2;
    for (index = 1; index < 24; ++index) {
        term *= reduced / index;
        sum += term;
    }
    return ldexp(sum, power);
}

/** @brief Implements the public log contract. */
double log(double value)
{
    double fraction;
    double ratio;
    double square;
    double term;
    double sum;
    int exponent;
    int index;

    if (value <= 0.0) {
        errno = EDOM;
        return -HUGE_VAL;
    }
    fraction = frexp(value, &exponent) * 2.0;
    --exponent;
    ratio = (fraction - 1.0) / (fraction + 1.0);
    square = ratio * ratio;
    term = ratio;
    sum = ratio;
    for (index = 1; index < 30; ++index) {
        term *= square;
        sum += term / (2 * index + 1);
    }
    return 2.0 * sum + exponent * WCRT_LN2;
}

/** @brief Implements the public log10 contract. */
double log10(double value)
{
    return log(value) / WCRT_LN10;
}

/** @brief Implements the public sin contract. */
double sin(double value)
{
    return wcrt_sine_series(wcrt_reduce_angle(value));
}

/** @brief Implements the public cos contract. */
double cos(double value)
{
    return wcrt_cosine_series(wcrt_reduce_angle(value));
}

/** @brief Implements the public tan contract. */
double tan(double value)
{
    return sin(value) / cos(value);
}

/** @brief Implements the public atan contract. */
double atan(double value)
{
    double sign = value < 0.0 ? -1.0 : 1.0;
    double magnitude = fabs(value);
    double result;

    if (magnitude > 1.0) {
        result = WCRT_HALF_PI - wcrt_atan_series(1.0 / magnitude);
    } else if (magnitude > 0.4142135623730950) {
        result = WCRT_QUARTER_PI +
            wcrt_atan_series((magnitude - 1.0) / (magnitude + 1.0));
    } else {
        result = wcrt_atan_series(magnitude);
    }
    return sign * result;
}

/** @brief Implements the public atan2 contract. */
double atan2(double y, double x)
{
    if (x > 0.0) return atan(y / x);
    if (x < 0.0 && y >= 0.0) return atan(y / x) + WCRT_PI;
    if (x < 0.0 && y < 0.0) return atan(y / x) - WCRT_PI;
    if (x == 0.0 && y > 0.0) return WCRT_HALF_PI;
    if (x == 0.0 && y < 0.0) return -WCRT_HALF_PI;
    errno = EDOM;
    return 0.0;
}

/** @brief Implements the public asin contract. */
double asin(double value)
{
    if (fabs(value) > 1.0) {
        errno = EDOM;
        return 0.0;
    }
    return atan2(value, sqrt((1.0 - value) * (1.0 + value)));
}

/** @brief Implements the public acos contract. */
double acos(double value)
{
    if (fabs(value) > 1.0) {
        errno = EDOM;
        return 0.0;
    }
    return WCRT_HALF_PI - asin(value);
}

/** @brief Implements the public sinh contract. */
double sinh(double value)
{
    double positive = exp(value);
    double negative = exp(-value);

    return 0.5 * (positive - negative);
}

/** @brief Implements the public cosh contract. */
double cosh(double value)
{
    double positive = exp(value);
    double negative = exp(-value);

    return 0.5 * (positive + negative);
}

/** @brief Implements the public tanh contract. */
double tanh(double value)
{
    if (value > 20.0) return 1.0;
    if (value < -20.0) return -1.0;
    return sinh(value) / cosh(value);
}

/** @brief Implements the public fmod contract. */
double fmod(double numerator, double denominator)
{
    double integral;
    double quotient;

    if (denominator == 0.0) {
        errno = EDOM;
        return 0.0;
    }
    quotient = numerator / denominator;
    modf(quotient, &integral);
    return numerator - integral * denominator;
}

/** @brief Implements the public pow contract. */
double pow(double base, double power)
{
    double integral;
    double result;
    int negative_result = 0;

    if (power == 0.0) return 1.0;
    if (base == 0.0) {
        if (power < 0.0) {
            errno = EDOM;
            return HUGE_VAL;
        }
        return 0.0;
    }
    if (base < 0.0) {
        if (modf(power, &integral) != 0.0) {
            errno = EDOM;
            return 0.0;
        }
        negative_result = fmod(fabs(integral), 2.0) == 1.0;
        base = -base;
    }
    result = exp(power * log(base));
    return negative_result ? -result : result;
}
