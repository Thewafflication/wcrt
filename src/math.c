/**
 * @file math.c
 * @brief Implements the C89 and C99 real mathematics library without a CRT.
 * @details Exact representation operations use IEEE binary32/binary64 bits;
 * ordinary transcendental results target the tolerances in REQ-0035.
 */

#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <limits.h>
#include <math.h>

#define WCRT_PI 3.14159265358979323846
#define WCRT_HALF_PI 1.57079632679489661923
#define WCRT_QUARTER_PI 0.78539816339744830962
#define WCRT_TWO_PI 6.28318530717958647692
#define WCRT_LN2 0.69314718055994530942
#define WCRT_LN10 2.30258509299404568402
#define WCRT_LOG_SQRT_PI 0.57236494292470008707
#define WCRT_TWO_OVER_SQRT_PI 1.12837916709551257390
#define WCRT_FMA_BASE_EXP (-2200)
#define WCRT_FMA_WORDS 140

union wcrt_double_shape {
    double value;
    unsigned long long bits;
};

union wcrt_float_shape {
    float value;
    unsigned int bits;
};

static double wcrt_infinity(void)
{
    union wcrt_double_shape shape;
    shape.bits = 0x7ff0000000000000ULL;
    return shape.value;
}

static float wcrt_infinityf(void)
{
    union wcrt_float_shape shape;
    shape.bits = 0x7f800000U;
    return shape.value;
}

static double wcrt_quiet_nan(void)
{
    union wcrt_double_shape shape;
    shape.bits = 0x7ff8000000000000ULL;
    return shape.value;
}

static float wcrt_quiet_nanf(void)
{
    union wcrt_float_shape shape;
    shape.bits = 0x7fc00000U;
    return shape.value;
}

int __wcrt_fpclassify(double value)
{
    union wcrt_double_shape shape;
    unsigned long long exponent;
    unsigned long long fraction;

    shape.value = value;
    exponent = (shape.bits >> 52) & 0x7ffULL;
    fraction = shape.bits & 0x000fffffffffffffULL;
    if (exponent == 0x7ffULL) return fraction ? FP_NAN : FP_INFINITE;
    if (exponent == 0ULL) return fraction ? FP_SUBNORMAL : FP_ZERO;
    return FP_NORMAL;
}

int __wcrt_fpclassifyf(float value)
{
    union wcrt_float_shape shape;
    unsigned int exponent;
    unsigned int fraction;

    shape.value = value;
    exponent = (shape.bits >> 23) & 0xffU;
    fraction = shape.bits & 0x007fffffU;
    if (exponent == 0xffU) return fraction ? FP_NAN : FP_INFINITE;
    if (exponent == 0U) return fraction ? FP_SUBNORMAL : FP_ZERO;
    return FP_NORMAL;
}

int __wcrt_signbit(double value)
{
    union wcrt_double_shape shape;
    shape.value = value;
    return (int)(shape.bits >> 63);
}

int __wcrt_signbitf(float value)
{
    union wcrt_float_shape shape;
    shape.value = value;
    return (int)(shape.bits >> 31);
}

static int wcrt_is_nan(double value)
{
    return __wcrt_fpclassify(value) == FP_NAN;
}

static int wcrt_is_infinite(double value)
{
    return __wcrt_fpclassify(value) == FP_INFINITE;
}

static int wcrt_is_finite(double value)
{
    return __wcrt_fpclassify(value) >= FP_ZERO;
}

static int wcrt_is_nanf(float value)
{
    return __wcrt_fpclassifyf(value) == FP_NAN;
}

#define WCRT_ORDERED_HELPERS(suffix, type, nan_test) \
int __wcrt_isgreater##suffix(type lhs, type rhs) \
{ return !(nan_test(lhs) || nan_test(rhs)) && lhs > rhs; } \
int __wcrt_isgreaterequal##suffix(type lhs, type rhs) \
{ return !(nan_test(lhs) || nan_test(rhs)) && lhs >= rhs; } \
int __wcrt_isless##suffix(type lhs, type rhs) \
{ return !(nan_test(lhs) || nan_test(rhs)) && lhs < rhs; } \
int __wcrt_islessequal##suffix(type lhs, type rhs) \
{ return !(nan_test(lhs) || nan_test(rhs)) && lhs <= rhs; } \
int __wcrt_islessgreater##suffix(type lhs, type rhs) \
{ return !(nan_test(lhs) || nan_test(rhs)) && (lhs < rhs || lhs > rhs); } \
int __wcrt_isunordered##suffix(type lhs, type rhs) \
{ return nan_test(lhs) || nan_test(rhs); }

WCRT_ORDERED_HELPERS(f, float, wcrt_is_nanf)
WCRT_ORDERED_HELPERS(, double, wcrt_is_nan)

double copysign(double value, double sign)
{
    union wcrt_double_shape source;
    union wcrt_double_shape sign_source;
    source.value = value;
    sign_source.value = sign;
    source.bits = (source.bits & 0x7fffffffffffffffULL) |
        (sign_source.bits & 0x8000000000000000ULL);
    return source.value;
}

float copysignf(float value, float sign)
{
    union wcrt_float_shape source;
    union wcrt_float_shape sign_source;
    source.value = value;
    sign_source.value = sign;
    source.bits = (source.bits & 0x7fffffffU) |
        (sign_source.bits & 0x80000000U);
    return source.value;
}

long double copysignl(long double value, long double sign)
{
    return (long double)copysign((double)value, (double)sign);
}

double fabs(double value)
{
    union wcrt_double_shape shape;
    shape.value = value;
    shape.bits &= 0x7fffffffffffffffULL;
    return shape.value;
}

float fabsf(float value)
{
    union wcrt_float_shape shape;
    shape.value = value;
    shape.bits &= 0x7fffffffU;
    return shape.value;
}

long double fabsl(long double value)
{
    return (long double)fabs((double)value);
}

double nan(const char *tag)
{
    (void)tag;
    return wcrt_quiet_nan();
}

float nanf(const char *tag)
{
    (void)tag;
    return wcrt_quiet_nanf();
}

long double nanl(const char *tag)
{
    (void)tag;
    return (long double)wcrt_quiet_nan();
}

double nextafter(double value, double direction)
{
    union wcrt_double_shape shape;
    union wcrt_double_shape target;
    int result_class;

    if (wcrt_is_nan(value) || wcrt_is_nan(direction)) return value + direction;
    if (value == direction) return direction;
    shape.value = value;
    target.value = direction;
    if ((shape.bits & 0x7fffffffffffffffULL) == 0ULL) {
        shape.bits = (target.bits & 0x8000000000000000ULL) | 1ULL;
    } else if ((value > 0.0) == (value < direction)) {
        ++shape.bits;
    } else {
        --shape.bits;
    }
    result_class = __wcrt_fpclassify(shape.value);
    if (result_class == FP_INFINITE || result_class == FP_SUBNORMAL ||
        result_class == FP_ZERO) errno = ERANGE;
    return shape.value;
}

float nextafterf(float value, float direction)
{
    union wcrt_float_shape shape;
    union wcrt_float_shape target;
    int result_class;

    if (wcrt_is_nanf(value) || wcrt_is_nanf(direction)) {
        return value + direction;
    }
    if (value == direction) return direction;
    shape.value = value;
    target.value = direction;
    if ((shape.bits & 0x7fffffffU) == 0U) {
        shape.bits = (target.bits & 0x80000000U) | 1U;
    } else if ((value > 0.0F) == (value < direction)) {
        ++shape.bits;
    } else {
        --shape.bits;
    }
    result_class = __wcrt_fpclassifyf(shape.value);
    if (result_class == FP_INFINITE || result_class == FP_SUBNORMAL ||
        result_class == FP_ZERO) errno = ERANGE;
    return shape.value;
}

long double nextafterl(long double value, long double direction)
{
    return (long double)nextafter((double)value, (double)direction);
}

double nexttoward(double value, long double direction)
{
    return nextafter(value, (double)direction);
}

float nexttowardf(float value, long double direction)
{
    double wide_value = (double)value;
    double wide_direction = (double)direction;
    if (wide_value == wide_direction) return (float)direction;
    return nextafterf(value, wide_value < wide_direction ?
        wcrt_infinityf() : -wcrt_infinityf());
}

long double nexttowardl(long double value, long double direction)
{
    return nextafterl(value, direction);
}

double modf(double value, double *integer_part)
{
    union wcrt_double_shape shape;
    union wcrt_double_shape integral;
    unsigned long long mask;
    int exponent;

    shape.value = value;
    exponent = (int)((shape.bits >> 52) & 0x7ffULL) - 1023;
    if (((shape.bits >> 52) & 0x7ffULL) == 0x7ffULL) {
        *integer_part = value;
        return wcrt_is_nan(value) ? value : copysign(0.0, value);
    }
    if (exponent < 0) {
        *integer_part = copysign(0.0, value);
        return value;
    }
    if (exponent >= 52) {
        *integer_part = value;
        return copysign(0.0, value);
    }
    mask = (1ULL << (52 - exponent)) - 1ULL;
    if ((shape.bits & mask) == 0ULL) {
        *integer_part = value;
        return copysign(0.0, value);
    }
    integral.bits = shape.bits & ~mask;
    *integer_part = integral.value;
    return value - integral.value;
}

float modff(float value, float *integer_part)
{
    union wcrt_float_shape shape;
    union wcrt_float_shape integral;
    unsigned int mask;
    int exponent;

    shape.value = value;
    exponent = (int)((shape.bits >> 23) & 0xffU) - 127;
    if (((shape.bits >> 23) & 0xffU) == 0xffU) {
        *integer_part = value;
        return wcrt_is_nanf(value) ? value : copysignf(0.0F, value);
    }
    if (exponent < 0) {
        *integer_part = copysignf(0.0F, value);
        return value;
    }
    if (exponent >= 23) {
        *integer_part = value;
        return copysignf(0.0F, value);
    }
    mask = (1U << (23 - exponent)) - 1U;
    if ((shape.bits & mask) == 0U) {
        *integer_part = value;
        return copysignf(0.0F, value);
    }
    integral.bits = shape.bits & ~mask;
    *integer_part = integral.value;
    return value - integral.value;
}

long double modfl(long double value, long double *integer_part)
{
    double integral;
    double fraction = modf((double)value, &integral);
    *integer_part = (long double)integral;
    return (long double)fraction;
}

double floor(double value)
{
    double integral;
    double fraction;
    if (!wcrt_is_finite(value) || value == 0.0) return value;
    fraction = modf(value, &integral);
    return fraction < 0.0 ? integral - 1.0 : integral;
}

double ceil(double value)
{
    double integral;
    double fraction;
    if (!wcrt_is_finite(value) || value == 0.0) return value;
    fraction = modf(value, &integral);
    return fraction > 0.0 ? integral + 1.0 : integral;
}

double trunc(double value)
{
    double integral;
    if (!wcrt_is_finite(value) || value == 0.0) return value;
    (void)modf(value, &integral);
    return integral;
}

double round(double value)
{
    double integral;
    double fraction;
    if (!wcrt_is_finite(value) || value == 0.0) return value;
    fraction = modf(value, &integral);
    if (fraction >= 0.5) return integral + 1.0;
    if (fraction <= -0.5) return integral - 1.0;
    return integral;
}

static double wcrt_round_even(double value)
{
    double integral;
    double fraction;
    double magnitude;

    if (!wcrt_is_finite(value) || value == 0.0) return value;
    fraction = modf(value, &integral);
    magnitude = fabs(fraction);
    if (magnitude > 0.5 ||
        (magnitude == 0.5 && fmod(fabs(integral), 2.0) == 1.0)) {
        integral += fraction < 0.0 ? -1.0 : 1.0;
    }
    return integral;
}

static double wcrt_round_mode(double value)
{
    int mode = fegetround();
    if (mode == FE_DOWNWARD) return floor(value);
    if (mode == FE_UPWARD) return ceil(value);
    if (mode == FE_TOWARDZERO) return trunc(value);
    return wcrt_round_even(value);
}

double rint(double value)
{
    double result = wcrt_round_mode(value);
    if (wcrt_is_finite(value) && result != value) {
        (void)feraiseexcept(FE_INEXACT);
    }
    return result;
}

double nearbyint(double value)
{
    fexcept_t inexact;
    double result;
    (void)fegetexceptflag(&inexact, FE_INEXACT);
    result = wcrt_round_mode(value);
    (void)fesetexceptflag(&inexact, FE_INEXACT);
    return result;
}

static long wcrt_to_long(double value, int use_environment)
{
    double rounded = use_environment ? rint(value) : round(value);
    if (wcrt_is_nan(rounded) || rounded > (double)LONG_MAX ||
        rounded < (double)LONG_MIN) {
        errno = EDOM;
        return rounded < 0.0 ? LONG_MIN : LONG_MAX;
    }
    return (long)rounded;
}

static long long wcrt_to_long_long(double value, int use_environment)
{
    double rounded = use_environment ? rint(value) : round(value);
    if (wcrt_is_nan(rounded) || rounded >= 9223372036854775808.0 ||
        rounded < -9223372036854775808.0) {
        errno = EDOM;
        return rounded < 0.0 ? LLONG_MIN : LLONG_MAX;
    }
    return (long long)rounded;
}

long lrint(double value) { return wcrt_to_long(value, 1); }
long long llrint(double value) { return wcrt_to_long_long(value, 1); }
long lround(double value) { return wcrt_to_long(value, 0); }
long long llround(double value) { return wcrt_to_long_long(value, 0); }

double frexp(double value, int *exponent)
{
    union wcrt_double_shape shape;
    unsigned int raw_exponent;

    shape.value = value;
    raw_exponent = (unsigned int)((shape.bits >> 52) & 0x7ffULL);
    if (raw_exponent == 0U) {
        if ((shape.bits & 0x7fffffffffffffffULL) == 0ULL) {
            *exponent = 0;
            return value;
        }
        value *= 18014398509481984.0;
        shape.value = value;
        raw_exponent = (unsigned int)((shape.bits >> 52) & 0x7ffULL);
        *exponent = (int)raw_exponent - 1022 - 54;
    } else if (raw_exponent == 0x7ffU) {
        *exponent = 0;
        return value;
    } else {
        *exponent = (int)raw_exponent - 1022;
    }
    shape.bits = (shape.bits & 0x800fffffffffffffULL) |
        (1022ULL << 52);
    return shape.value;
}

float frexpf(float value, int *exponent)
{
    union wcrt_float_shape shape;
    unsigned int raw_exponent;

    shape.value = value;
    raw_exponent = (shape.bits >> 23) & 0xffU;
    if (raw_exponent == 0U) {
        if ((shape.bits & 0x7fffffffU) == 0U) {
            *exponent = 0;
            return value;
        }
        value *= 33554432.0F;
        shape.value = value;
        raw_exponent = (shape.bits >> 23) & 0xffU;
        *exponent = (int)raw_exponent - 126 - 25;
    } else if (raw_exponent == 0xffU) {
        *exponent = 0;
        return value;
    } else {
        *exponent = (int)raw_exponent - 126;
    }
    shape.bits = (shape.bits & 0x807fffffU) | (126U << 23);
    return shape.value;
}

long double frexpl(long double value, int *exponent)
{
    return (long double)frexp((double)value, exponent);
}

static double wcrt_scalbn_core(double value, long exponent)
{
    double result = value;
    double factor;
    union wcrt_double_shape shape;
    int result_class;

    if (!wcrt_is_finite(value) || value == 0.0) return value;
    if (exponent > 4096L) {
        errno = ERANGE;
        return copysign(HUGE_VAL, value);
    }
    if (exponent < -4096L) {
        errno = ERANGE;
        return copysign(0.0, value);
    }
    while (exponent > 512L) {
        result *= 1.3407807929942597e+154;
        exponent -= 512L;
        if (wcrt_is_infinite(result)) {
            errno = ERANGE;
            return copysign(HUGE_VAL, value);
        }
    }
    while (exponent < -512L) {
        result *= 7.4583407312002070e-155;
        exponent += 512L;
        if (result == 0.0) {
            errno = ERANGE;
            return copysign(0.0, value);
        }
    }
    shape.bits = ((unsigned long long)(exponent + 1023L)) << 52;
    factor = shape.value;
    result *= factor;
    result_class = __wcrt_fpclassify(result);
    if (result_class == FP_INFINITE) {
        errno = ERANGE;
        return copysign(HUGE_VAL, value);
    }
    if (result_class == FP_SUBNORMAL || result_class == FP_ZERO) errno = ERANGE;
    return result;
}

double ldexp(double value, int exponent)
{
    return wcrt_scalbn_core(value, (long)exponent);
}

double scalbn(double value, int exponent)
{
    return wcrt_scalbn_core(value, (long)exponent);
}

double scalbln(double value, long exponent)
{
    return wcrt_scalbn_core(value, exponent);
}

int ilogb(double value)
{
    int exponent;
    int classification = __wcrt_fpclassify(value);
    if (classification == FP_ZERO) {
        errno = EDOM;
        return FP_ILOGB0;
    }
    if (classification == FP_NAN) {
        errno = EDOM;
        return FP_ILOGBNAN;
    }
    if (classification == FP_INFINITE) return INT_MAX;
    (void)frexp(value, &exponent);
    return exponent - 1;
}

double logb(double value)
{
    int classification = __wcrt_fpclassify(value);
    if (classification == FP_ZERO) {
        errno = ERANGE;
        return -wcrt_infinity();
    }
    if (classification == FP_NAN) return value;
    if (classification == FP_INFINITE) return wcrt_infinity();
    return (double)ilogb(value);
}

static double wcrt_reduce_angle(double value)
{
    double quotient = value / WCRT_TWO_PI;
    double integral = quotient >= 0.0 ? floor(quotient + 0.5) :
        ceil(quotient - 0.5);
    return value - integral * WCRT_TWO_PI;
}

static double wcrt_sine_series(double value)
{
    double term = value;
    double sum = value;
    double square = value * value;
    int index;
    for (index = 1; index < 18; ++index) {
        term *= -square / ((2 * index) * (2 * index + 1));
        sum += term;
    }
    return sum;
}

static double wcrt_cosine_series(double value)
{
    double term = 1.0;
    double sum = 1.0;
    double square = value * value;
    int index;
    for (index = 1; index < 18; ++index) {
        term *= -square / ((2 * index - 1) * (2 * index));
        sum += term;
    }
    return sum;
}

static double wcrt_atan_series(double value)
{
    double square = value * value;
    double term = value;
    double sum = value;
    int index;
    for (index = 1; index < 36; ++index) {
        term *= -square;
        sum += term / (2 * index + 1);
    }
    return sum;
}

double sin(double value)
{
    if (wcrt_is_nan(value) || value == 0.0) return value;
    if (wcrt_is_infinite(value)) {
        errno = EDOM;
        return wcrt_quiet_nan();
    }
    return wcrt_sine_series(wcrt_reduce_angle(value));
}

double cos(double value)
{
    if (wcrt_is_nan(value)) return value;
    if (wcrt_is_infinite(value)) {
        errno = EDOM;
        return wcrt_quiet_nan();
    }
    return wcrt_cosine_series(wcrt_reduce_angle(value));
}

double tan(double value)
{
    double cosine;
    if (value == 0.0) return value;
    cosine = cos(value);
    return sin(value) / cosine;
}

double atan(double value)
{
    double magnitude;
    double result;
    if (wcrt_is_nan(value) || value == 0.0) return value;
    if (wcrt_is_infinite(value)) return copysign(WCRT_HALF_PI, value);
    magnitude = fabs(value);
    if (magnitude > 1.0) {
        result = WCRT_HALF_PI - wcrt_atan_series(1.0 / magnitude);
    } else if (magnitude > 0.4142135623730950) {
        result = WCRT_QUARTER_PI +
            wcrt_atan_series((magnitude - 1.0) / (magnitude + 1.0));
    } else {
        result = wcrt_atan_series(magnitude);
    }
    return copysign(result, value);
}

double atan2(double y, double x)
{
    if (wcrt_is_nan(x) || wcrt_is_nan(y)) return x + y;
    if (x == 0.0 && y == 0.0) {
        errno = EDOM;
        return copysign(0.0, y);
    }
    if (wcrt_is_infinite(y) && wcrt_is_infinite(x)) {
        if (x > 0.0) return copysign(WCRT_QUARTER_PI, y);
        return copysign(3.0 * WCRT_QUARTER_PI, y);
    }
    if (wcrt_is_infinite(y)) return copysign(WCRT_HALF_PI, y);
    if (wcrt_is_infinite(x)) {
        if (x > 0.0) return copysign(0.0, y);
        return copysign(WCRT_PI, y);
    }
    if (x > 0.0) return atan(y / x);
    if (x < 0.0) return y >= 0.0 ? atan(y / x) + WCRT_PI :
        atan(y / x) - WCRT_PI;
    return copysign(WCRT_HALF_PI, y);
}

double sqrt(double value)
{
    double estimate;
    int exponent;
    int index;

    if (wcrt_is_nan(value) || value == 0.0 || value == wcrt_infinity())
        return value;
    if (value < 0.0) {
        errno = EDOM;
        return 0.0;
    }
    (void)frexp(value, &exponent);
    estimate = wcrt_scalbn_core(1.0, (long)(exponent / 2));
    if (estimate == 0.0) estimate = 1.0;
    for (index = 0; index < 16; ++index)
        estimate = 0.5 * (estimate + value / estimate);
    return estimate;
}

double asin(double value)
{
    if (wcrt_is_nan(value) || value == 0.0) return value;
    if (fabs(value) > 1.0) {
        errno = EDOM;
        return 0.0;
    }
    if (fabs(value) == 1.0) return copysign(WCRT_HALF_PI, value);
    return atan2(value, sqrt((1.0 - value) * (1.0 + value)));
}

double acos(double value)
{
    if (wcrt_is_nan(value)) return value;
    if (fabs(value) > 1.0) {
        errno = EDOM;
        return 0.0;
    }
    if (value == 1.0) return 0.0;
    if (value == -1.0) return WCRT_PI;
    return WCRT_HALF_PI - asin(value);
}

double exp(double value)
{
    double reduced;
    double term = 1.0;
    double sum = 1.0;
    int power;
    int index;

    if (wcrt_is_nan(value)) return value;
    if (value == wcrt_infinity()) return value;
    if (value == -wcrt_infinity()) return 0.0;
    if (value > 709.782712893384) {
        errno = ERANGE;
        return HUGE_VAL;
    }
    if (value < -745.1332191019411) {
        errno = ERANGE;
        return 0.0;
    }
    power = (int)(value / WCRT_LN2 + (value >= 0.0 ? 0.5 : -0.5));
    reduced = value - power * WCRT_LN2;
    for (index = 1; index < 28; ++index) {
        term *= reduced / index;
        sum += term;
    }
    return wcrt_scalbn_core(sum, (long)power);
}

double exp2(double value)
{
    if (wcrt_is_finite(value) && value >= -1074.0 && value <= 1023.0 &&
        trunc(value) == value) return wcrt_scalbn_core(1.0, (long)value);
    return exp(value * WCRT_LN2);
}

double expm1(double value)
{
    double term;
    double sum;
    int index;
    if (fabs(value) > 0.5 || !wcrt_is_finite(value)) return exp(value) - 1.0;
    term = value;
    sum = value;
    for (index = 2; index < 32; ++index) {
        term *= value / index;
        sum += term;
    }
    return sum;
}

double log(double value)
{
    double fraction;
    double ratio;
    double square;
    double term;
    double sum;
    int exponent;
    int index;

    if (wcrt_is_nan(value)) return value;
    if (value == wcrt_infinity()) return value;
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
    for (index = 1; index < 36; ++index) {
        term *= square;
        sum += term / (2 * index + 1);
    }
    return 2.0 * sum + exponent * WCRT_LN2;
}

double log10(double value)
{
    return log(value) / WCRT_LN10;
}

double log2(double value)
{
    return log(value) / WCRT_LN2;
}

double log1p(double value)
{
    double term;
    double sum;
    int index;
    if (value <= -1.0) {
        errno = EDOM;
        return value == -1.0 ? -HUGE_VAL : wcrt_quiet_nan();
    }
    if (fabs(value) > 0.25) return log(1.0 + value);
    term = value;
    sum = value;
    for (index = 2; index < 80; ++index) {
        term *= -value;
        sum += term / index;
    }
    return sum;
}

double pow(double base, double power)
{
    double integral;
    double result;
    int negative_result = 0;

    if (power == 0.0 || base == 1.0) return 1.0;
    if (wcrt_is_nan(base) || wcrt_is_nan(power)) return base + power;
    if (wcrt_is_infinite(power)) {
        double magnitude = fabs(base);
        if (magnitude == 1.0) return 1.0;
        if ((magnitude > 1.0) == (power > 0.0)) return wcrt_infinity();
        return 0.0;
    }
    if (base == 0.0) {
        if (power < 0.0) {
            errno = EDOM;
            return copysign(HUGE_VAL,
                __wcrt_signbit(base) && fmod(fabs(power), 2.0) == 1.0 ?
                -1.0 : 1.0);
        }
        return copysign(0.0,
            __wcrt_signbit(base) && fmod(fabs(power), 2.0) == 1.0 ?
            -1.0 : 1.0);
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

double sinh(double value)
{
    double magnitude;
    double positive;
    double negative;
    if (wcrt_is_nan(value) || wcrt_is_infinite(value) || value == 0.0)
        return value;
    magnitude = fabs(value);
    positive = exp(magnitude);
    negative = 1.0 / positive;
    return copysign(0.5 * (positive - negative), value);
}

double cosh(double value)
{
    double magnitude;
    double positive;
    if (wcrt_is_nan(value)) return value;
    if (wcrt_is_infinite(value)) return wcrt_infinity();
    magnitude = fabs(value);
    positive = exp(magnitude);
    return 0.5 * (positive + 1.0 / positive);
}

double tanh(double value)
{
    double twice;
    if (wcrt_is_nan(value) || value == 0.0) return value;
    if (value > 20.0) return 1.0;
    if (value < -20.0) return -1.0;
    twice = expm1(2.0 * fabs(value));
    return copysign(twice / (twice + 2.0), value);
}

double hypot(double lhs, double rhs)
{
    double large = fabs(lhs);
    double small = fabs(rhs);
    double ratio;
    if (wcrt_is_infinite(large) || wcrt_is_infinite(small))
        return wcrt_infinity();
    if (wcrt_is_nan(large) || wcrt_is_nan(small)) return large + small;
    if (small > large) {
        double swap = large;
        large = small;
        small = swap;
    }
    if (large == 0.0) return 0.0;
    ratio = small / large;
    return large * sqrt(1.0 + ratio * ratio);
}

double acosh(double value)
{
    if (wcrt_is_nan(value)) return value;
    if (value < 1.0) {
        errno = EDOM;
        return wcrt_quiet_nan();
    }
    if (value == 1.0 || wcrt_is_infinite(value)) return value - 1.0;
    if (value > 1.0e154) return log(value) + WCRT_LN2;
    return log(value + sqrt(value - 1.0) * sqrt(value + 1.0));
}

double asinh(double value)
{
    double magnitude;
    double result;
    if (wcrt_is_nan(value) || wcrt_is_infinite(value) || value == 0.0)
        return value;
    magnitude = fabs(value);
    result = magnitude > 1.0e154 ? log(magnitude) + WCRT_LN2 :
        log(magnitude + hypot(magnitude, 1.0));
    return copysign(result, value);
}

double atanh(double value)
{
    double magnitude = fabs(value);
    if (wcrt_is_nan(value) || value == 0.0) return value;
    if (magnitude > 1.0) {
        errno = EDOM;
        return wcrt_quiet_nan();
    }
    if (magnitude == 1.0) {
        errno = ERANGE;
        return copysign(HUGE_VAL, value);
    }
    return 0.5 * log1p(2.0 * value / (1.0 - value));
}

double cbrt(double value)
{
    double magnitude;
    double result;
    int index;
    if (!wcrt_is_finite(value) || value == 0.0) return value;
    magnitude = fabs(value);
    result = exp(log(magnitude) / 3.0);
    for (index = 0; index < 5; ++index)
        result = (2.0 * result + magnitude / (result * result)) / 3.0;
    return copysign(result, value);
}

double fmod(double numerator, double denominator)
{
    union wcrt_double_shape dividend;
    union wcrt_double_shape divisor;
    unsigned long long sign;
    unsigned long long difference;
    int dividend_exponent;
    int divisor_exponent;
    if (wcrt_is_nan(numerator) || wcrt_is_nan(denominator))
        return numerator + denominator;
    if (denominator == 0.0 || wcrt_is_infinite(numerator)) {
        errno = EDOM;
        return 0.0;
    }
    if (wcrt_is_infinite(denominator)) return numerator;
    dividend.value = numerator;
    divisor.value = denominator;
    sign = dividend.bits & 0x8000000000000000ULL;
    dividend.bits &= 0x7fffffffffffffffULL;
    divisor.bits &= 0x7fffffffffffffffULL;
    if (dividend.bits < divisor.bits) return numerator;
    if (dividend.bits == divisor.bits) return copysign(0.0, numerator);

    dividend_exponent = (int)(dividend.bits >> 52);
    divisor_exponent = (int)(divisor.bits >> 52);
    if (dividend_exponent == 0) {
        while ((dividend.bits >> 52) == 0ULL) {
            dividend.bits <<= 1;
            --dividend_exponent;
        }
        ++dividend_exponent;
    } else {
        dividend.bits &= 0x000fffffffffffffULL;
        dividend.bits |= 0x0010000000000000ULL;
    }
    if (divisor_exponent == 0) {
        while ((divisor.bits >> 52) == 0ULL) {
            divisor.bits <<= 1;
            --divisor_exponent;
        }
        ++divisor_exponent;
    } else {
        divisor.bits &= 0x000fffffffffffffULL;
        divisor.bits |= 0x0010000000000000ULL;
    }
    while (dividend_exponent > divisor_exponent) {
        difference = dividend.bits - divisor.bits;
        if ((difference >> 63) == 0ULL) {
            if (difference == 0ULL) return copysign(0.0, numerator);
            dividend.bits = difference;
        }
        dividend.bits <<= 1;
        --dividend_exponent;
    }
    difference = dividend.bits - divisor.bits;
    if ((difference >> 63) == 0ULL) {
        if (difference == 0ULL) return copysign(0.0, numerator);
        dividend.bits = difference;
    }
    while ((dividend.bits >> 52) == 0ULL) {
        dividend.bits <<= 1;
        --dividend_exponent;
    }
    if (dividend_exponent > 0) {
        dividend.bits -= 0x0010000000000000ULL;
        dividend.bits |= (unsigned long long)dividend_exponent << 52;
    } else {
        dividend.bits >>= 1 - dividend_exponent;
    }
    dividend.bits |= sign;
    return dividend.value;
}

double remainder(double numerator, double denominator)
{
    double quotient;
    double nearest;
    if (wcrt_is_nan(numerator) || wcrt_is_nan(denominator))
        return numerator + denominator;
    if (denominator == 0.0 || wcrt_is_infinite(numerator)) {
        errno = EDOM;
        return wcrt_quiet_nan();
    }
    if (wcrt_is_infinite(denominator)) return numerator;
    quotient = numerator / denominator;
    nearest = wcrt_round_even(quotient);
    return numerator - nearest * denominator;
}

double remquo(double numerator, double denominator, int *quotient)
{
    double divided;
    double nearest;
    double low;
    if (denominator == 0.0 || wcrt_is_infinite(numerator) ||
        wcrt_is_nan(numerator) || wcrt_is_nan(denominator)) {
        *quotient = 0;
        return remainder(numerator, denominator);
    }
    divided = numerator / denominator;
    nearest = wcrt_round_even(divided);
    low = fmod(fabs(nearest), 8.0);
    *quotient = (int)low;
    if (nearest < 0.0) *quotient = -*quotient;
    return numerator - nearest * denominator;
}

double fmax(double lhs, double rhs)
{
    if (wcrt_is_nan(lhs)) return rhs;
    if (wcrt_is_nan(rhs)) return lhs;
    if (lhs > rhs) return lhs;
    if (rhs > lhs) return rhs;
    if (lhs == 0.0) return __wcrt_signbit(lhs) ? rhs : lhs;
    return lhs;
}

double fmin(double lhs, double rhs)
{
    if (wcrt_is_nan(lhs)) return rhs;
    if (wcrt_is_nan(rhs)) return lhs;
    if (lhs < rhs) return lhs;
    if (rhs < lhs) return rhs;
    if (lhs == 0.0) return __wcrt_signbit(lhs) ? lhs : rhs;
    return lhs;
}

double fdim(double lhs, double rhs)
{
    double result;
    if (wcrt_is_nan(lhs) || wcrt_is_nan(rhs)) return lhs + rhs;
    if (lhs <= rhs) return 0.0;
    result = lhs - rhs;
    if (wcrt_is_infinite(result) && wcrt_is_finite(lhs) && wcrt_is_finite(rhs))
        errno = ERANGE;
    return result;
}

/** @brief Exact finite operand decomposition used by fused operations. */
struct wcrt_fma_operand {
    unsigned long long significand;
    int exponent;
    int negative;
    int zero;
    int infinite;
    int not_a_number;
};

/** @brief Fixed exact magnitude spanning every binary64 product and addend. */
struct wcrt_fma_magnitude {
    unsigned int word[WCRT_FMA_WORDS];
};

/** @brief Decodes a binary64 operand without a floating-point operation. */
static struct wcrt_fma_operand wcrt_fma_decode(double value)
{
    union wcrt_double_shape shape;
    struct wcrt_fma_operand result;
    unsigned int encoded_exponent;

    shape.value = value;
    encoded_exponent = (unsigned int)((shape.bits >> 52) & 0x7ffULL);
    result.negative = (int)(shape.bits >> 63);
    result.significand = shape.bits & 0x000fffffffffffffULL;
    result.zero = encoded_exponent == 0U && result.significand == 0ULL;
    result.infinite = encoded_exponent == 0x7ffU &&
        result.significand == 0ULL;
    result.not_a_number = encoded_exponent == 0x7ffU &&
        result.significand != 0ULL;
    if (encoded_exponent == 0U) {
        result.exponent = -1074;
    } else {
        result.significand |= 0x0010000000000000ULL;
        result.exponent = (int)encoded_exponent - 1023 - 52;
    }
    return result;
}

/** @brief Clears an exact fused-operation magnitude. */
static void wcrt_fma_clear(struct wcrt_fma_magnitude *value)
{
    int index;
    for (index = 0; index < WCRT_FMA_WORDS; ++index) {
        value->word[index] = 0U;
    }
}

/** @brief Adds one exact power-of-two bit to a fused magnitude. */
static void wcrt_fma_add_bit(struct wcrt_fma_magnitude *value, int bit)
{
    while (bit < WCRT_FMA_WORDS * 32) {
        int word = bit / 32;
        unsigned int mask = 1U << (bit % 32);
        if ((value->word[word] & mask) == 0U) {
            value->word[word] |= mask;
            return;
        }
        value->word[word] &= ~mask;
        ++bit;
    }
}

/** @brief Adds an exact decoded operand at its binary exponent. */
static void wcrt_fma_add_operand(struct wcrt_fma_magnitude *value,
    const struct wcrt_fma_operand *operand)
{
    int bit;
    for (bit = 0; bit < 53; ++bit) {
        if ((operand->significand & (1ULL << bit)) != 0ULL) {
            wcrt_fma_add_bit(value,
                operand->exponent + bit - WCRT_FMA_BASE_EXP);
        }
    }
}

/** @brief Forms an exact product of two decoded binary64 significands. */
static void wcrt_fma_product(struct wcrt_fma_magnitude *value,
    const struct wcrt_fma_operand *lhs,
    const struct wcrt_fma_operand *middle)
{
    int lhs_bit;
    int middle_bit;

    for (lhs_bit = 0; lhs_bit < 53; ++lhs_bit) {
        if ((lhs->significand & (1ULL << lhs_bit)) == 0ULL) continue;
        for (middle_bit = 0; middle_bit < 53; ++middle_bit) {
            if ((middle->significand & (1ULL << middle_bit)) != 0ULL) {
                wcrt_fma_add_bit(value, lhs->exponent + middle->exponent +
                    lhs_bit + middle_bit - WCRT_FMA_BASE_EXP);
            }
        }
    }
}

/** @brief Compares two exact fused-operation magnitudes. */
static int wcrt_fma_compare(const struct wcrt_fma_magnitude *lhs,
    const struct wcrt_fma_magnitude *rhs)
{
    int index;
    for (index = WCRT_FMA_WORDS - 1; index >= 0; --index) {
        if (lhs->word[index] > rhs->word[index]) return 1;
        if (lhs->word[index] < rhs->word[index]) return -1;
    }
    return 0;
}

/** @brief Adds two exact magnitudes. */
static void wcrt_fma_add(struct wcrt_fma_magnitude *result,
    const struct wcrt_fma_magnitude *rhs)
{
    unsigned long long carry = 0ULL;
    int index;
    for (index = 0; index < WCRT_FMA_WORDS; ++index) {
        unsigned long long sum = (unsigned long long)result->word[index] +
            rhs->word[index] + carry;
        result->word[index] = (unsigned int)sum;
        carry = sum >> 32;
    }
}

/** @brief Subtracts a no-larger exact magnitude. */
static void wcrt_fma_subtract(struct wcrt_fma_magnitude *result,
    const struct wcrt_fma_magnitude *rhs)
{
    unsigned long long borrow = 0ULL;
    int index;
    for (index = 0; index < WCRT_FMA_WORDS; ++index) {
        unsigned long long left = result->word[index];
        unsigned long long subtrahend =
            (unsigned long long)rhs->word[index] + borrow;
        if (left < subtrahend) {
            result->word[index] = (unsigned int)(
                left + 0x100000000ULL - subtrahend);
            borrow = 1ULL;
        } else {
            result->word[index] = (unsigned int)(left - subtrahend);
            borrow = 0ULL;
        }
    }
}

/** @brief Returns one bit from an exact magnitude. */
static int wcrt_fma_bit(const struct wcrt_fma_magnitude *value, int bit)
{
    if (bit < 0 || bit >= WCRT_FMA_WORDS * 32) return 0;
    return (value->word[bit / 32] >> (bit % 32)) & 1U;
}

/** @brief Reports whether any exact magnitude bit below a limit is set. */
static int wcrt_fma_any_below(const struct wcrt_fma_magnitude *value,
    int limit)
{
    int bit;
    for (bit = 0; bit < limit; ++bit) {
        if (wcrt_fma_bit(value, bit)) return 1;
    }
    return 0;
}

/** @brief Finds the most significant set bit in an exact magnitude. */
static int wcrt_fma_high_bit(const struct wcrt_fma_magnitude *value)
{
    int bit;
    for (bit = WCRT_FMA_WORDS * 32 - 1; bit >= 0; --bit) {
        if (wcrt_fma_bit(value, bit)) return bit;
    }
    return -1;
}

/** @brief Determines whether discarded bits increment the target encoding. */
static int wcrt_fma_round_up(const struct wcrt_fma_magnitude *value,
    int cutoff, unsigned long long retained, int negative)
{
    int discarded = wcrt_fma_any_below(value, cutoff);
    int mode;

    if (!discarded) return 0;
    mode = fegetround();
    if (mode == FE_UPWARD) return !negative;
    if (mode == FE_DOWNWARD) return negative;
    if (mode == FE_TOWARDZERO) return 0;
    return wcrt_fma_bit(value, cutoff - 1) &&
        (wcrt_fma_any_below(value, cutoff - 1) ||
            (retained & 1ULL) != 0ULL);
}

/** @brief Rounds an exact magnitude to a selected IEEE binary format. */
static unsigned long long wcrt_fma_round(
    const struct wcrt_fma_magnitude *value, int negative, int precision,
    int minimum_exponent, int maximum_exponent, int bias,
    unsigned long long sign_mask, unsigned long long maximum_bits)
{
    int high = wcrt_fma_high_bit(value);
    int exponent;
    int cutoff;
    int bit;
    int underflow;
    unsigned long long retained = 0ULL;
    unsigned long long fraction_mask =
        (1ULL << (precision - 1)) - 1ULL;

    if (high < 0) return negative ? sign_mask : 0ULL;
    exponent = WCRT_FMA_BASE_EXP + high;
    if (exponent > maximum_exponent) {
        errno = ERANGE;
        return maximum_bits | (negative ? sign_mask : 0ULL);
    }
    underflow = exponent < minimum_exponent;
    cutoff = underflow ?
        minimum_exponent - (precision - 1) - WCRT_FMA_BASE_EXP :
        high - (precision - 1);
    for (bit = high; bit >= cutoff; --bit) {
        retained = (retained << 1) |
            (unsigned long long)wcrt_fma_bit(value, bit);
    }
    if (wcrt_fma_round_up(value, cutoff, retained, negative)) {
        ++retained;
    }
    if (underflow) {
        errno = ERANGE;
        if (retained >= (1ULL << (precision - 1))) {
            return (1ULL << (precision - 1)) |
                (negative ? sign_mask : 0ULL);
        }
        return retained | (negative ? sign_mask : 0ULL);
    }
    if (retained == (1ULL << precision)) {
        retained >>= 1;
        ++exponent;
        if (exponent > maximum_exponent) {
            errno = ERANGE;
            return maximum_bits | (negative ? sign_mask : 0ULL);
        }
    }
    return ((unsigned long long)(exponent + bias) << (precision - 1)) |
        (retained & fraction_mask) | (negative ? sign_mask : 0ULL);
}

/** @brief Computes exact finite product-plus-addend target bits. */
static unsigned long long wcrt_fma_finite_bits(
    const struct wcrt_fma_operand *lhs,
    const struct wcrt_fma_operand *middle,
    const struct wcrt_fma_operand *rhs, int precision,
    int minimum_exponent, int maximum_exponent, int bias,
    unsigned long long sign_mask, unsigned long long maximum_bits)
{
    struct wcrt_fma_magnitude product;
    struct wcrt_fma_magnitude addend;
    int product_negative = lhs->negative != middle->negative;
    int result_negative;
    int order;

    wcrt_fma_clear(&product);
    wcrt_fma_clear(&addend);
    wcrt_fma_product(&product, lhs, middle);
    wcrt_fma_add_operand(&addend, rhs);
    if (product_negative == rhs->negative) {
        wcrt_fma_add(&product, &addend);
        result_negative = product_negative;
    } else {
        order = wcrt_fma_compare(&product, &addend);
        if (order > 0) {
            wcrt_fma_subtract(&product, &addend);
            result_negative = product_negative;
        } else if (order < 0) {
            wcrt_fma_subtract(&addend, &product);
            product = addend;
            result_negative = rhs->negative;
        } else {
            wcrt_fma_clear(&product);
            result_negative = fegetround() == FE_DOWNWARD;
        }
    }
    return wcrt_fma_round(&product, result_negative, precision,
        minimum_exponent, maximum_exponent, bias, sign_mask, maximum_bits);
}

double fma(double lhs, double middle, double rhs)
{
    struct wcrt_fma_operand left = wcrt_fma_decode(lhs);
    struct wcrt_fma_operand center = wcrt_fma_decode(middle);
    struct wcrt_fma_operand addend = wcrt_fma_decode(rhs);
    int product_negative = left.negative != center.negative;
    union wcrt_double_shape result;

    if (left.not_a_number || center.not_a_number || addend.not_a_number) {
        return lhs + middle + rhs;
    }
    if ((left.infinite && center.zero) ||
        (center.infinite && left.zero)) {
        errno = EDOM;
        return wcrt_quiet_nan();
    }
    if (left.infinite || center.infinite) {
        if (addend.infinite && product_negative != addend.negative) {
            errno = EDOM;
            return wcrt_quiet_nan();
        }
        return copysign(wcrt_infinity(), product_negative ? -1.0 : 1.0);
    }
    if (addend.infinite) return rhs;
    result.bits = wcrt_fma_finite_bits(&left, &center, &addend, 53,
        -1022, 1023, 1023, 0x8000000000000000ULL,
        0x7fefffffffffffffULL);
    return result.value;
}

static double wcrt_erfc_positive(double value)
{
    double square = value * value;
    double factor;
    double sum;
    double delta;
    double parameter;
    int index;

    if (value == 0.0) return 1.0;
    factor = exp(-square + 0.5 * log(square) - WCRT_LOG_SQRT_PI);
    if (square < 1.5) {
        parameter = 0.5;
        sum = 1.0 / parameter;
        delta = sum;
        for (index = 1; index < 160; ++index) {
            parameter += 1.0;
            delta *= square / parameter;
            sum += delta;
            if (fabs(delta) < fabs(sum) * 1.0e-16) break;
        }
        return 1.0 - sum * factor;
    }
    {
        double b = square + 0.5;
        double c = 1.0e300;
        double d = 1.0 / b;
        double h = d;
        for (index = 1; index < 160; ++index) {
            double an = -(double)index * ((double)index - 0.5);
            double multiplier;
            b += 2.0;
            d = an * d + b;
            if (fabs(d) < 1.0e-300) d = 1.0e-300;
            c = b + an / c;
            if (fabs(c) < 1.0e-300) c = 1.0e-300;
            d = 1.0 / d;
            multiplier = d * c;
            h *= multiplier;
            if (fabs(multiplier - 1.0) < 1.0e-15) break;
        }
        return factor * h;
    }
}

double erf(double value)
{
    double result;
    if (wcrt_is_nan(value) || value == 0.0) return value;
    if (wcrt_is_infinite(value)) return copysign(1.0, value);
    result = 1.0 - wcrt_erfc_positive(fabs(value));
    return copysign(result, value);
}

double erfc(double value)
{
    if (wcrt_is_nan(value)) return value;
    if (value == wcrt_infinity()) return 0.0;
    if (value == -wcrt_infinity()) return 2.0;
    if (value < 0.0) return 2.0 - wcrt_erfc_positive(-value);
    return wcrt_erfc_positive(value);
}

static double wcrt_lgamma_positive(double value)
{
    static const double coefficients[9] = {
        0.99999999999980993,
        676.5203681218851,
        -1259.1392167224028,
        771.32342877765313,
        -176.61502916214059,
        12.507343278686905,
        -0.13857109526572012,
        0.0000099843695780195716,
        0.00000015056327351493116
    };
    double shifted = value - 1.0;
    double series = coefficients[0];
    double base;
    int index;
    for (index = 1; index < 9; ++index)
        series += coefficients[index] / (shifted + index);
    base = shifted + 7.5;
    return 0.91893853320467274178 + (shifted + 0.5) * log(base) - base +
        log(series);
}

double lgamma(double value)
{
    double integral;
    if (wcrt_is_nan(value)) return value;
    if (value == wcrt_infinity()) return value;
    if (value <= 0.0 && modf(value, &integral) == 0.0) {
        errno = ERANGE;
        return HUGE_VAL;
    }
    if (value < 0.5)
        return log(WCRT_PI) - log(fabs(sin(WCRT_PI * value))) -
            wcrt_lgamma_positive(1.0 - value);
    return wcrt_lgamma_positive(value);
}

double tgamma(double value)
{
    double integral;
    double magnitude;
    if (wcrt_is_nan(value) || value == wcrt_infinity()) return value;
    if (value <= 0.0 && modf(value, &integral) == 0.0) {
        errno = EDOM;
        return HUGE_VAL;
    }
    magnitude = exp(lgamma(value));
    if (value < 0.0 && sin(WCRT_PI * value) < 0.0) magnitude = -magnitude;
    return magnitude;
}

/** @brief Applies the binary32 range contract to a binary64 core result. */
static float wcrt_float_result(double value)
{
    float converted;

    if (!wcrt_is_finite(value)) return (float)value;
    if (value > (double)FLT_MAX || value < -(double)FLT_MAX) {
        errno = ERANGE;
        return copysignf(HUGE_VALF, (float)value);
    }
    converted = (float)value;
    if (value != 0.0 && converted == 0.0F) errno = ERANGE;
    return converted;
}

#define WCRT_UNARY_WRAPPERS(name) \
float name##f(float value) { return wcrt_float_result(name((double)value)); } \
long double name##l(long double value) \
{ return (long double)name((double)value); }

#define WCRT_BINARY_WRAPPERS(name) \
float name##f(float lhs, float rhs) \
{ return wcrt_float_result(name((double)lhs, (double)rhs)); } \
long double name##l(long double lhs, long double rhs) \
{ return (long double)name((double)lhs, (double)rhs); }

WCRT_UNARY_WRAPPERS(acos)
WCRT_UNARY_WRAPPERS(asin)
WCRT_UNARY_WRAPPERS(atan)
WCRT_BINARY_WRAPPERS(atan2)
WCRT_UNARY_WRAPPERS(cos)
WCRT_UNARY_WRAPPERS(sin)
WCRT_UNARY_WRAPPERS(tan)
WCRT_UNARY_WRAPPERS(acosh)
WCRT_UNARY_WRAPPERS(asinh)
WCRT_UNARY_WRAPPERS(atanh)
WCRT_UNARY_WRAPPERS(cosh)
WCRT_UNARY_WRAPPERS(sinh)
WCRT_UNARY_WRAPPERS(tanh)
WCRT_UNARY_WRAPPERS(exp)
WCRT_UNARY_WRAPPERS(exp2)
WCRT_UNARY_WRAPPERS(expm1)
WCRT_UNARY_WRAPPERS(log)
WCRT_UNARY_WRAPPERS(log10)
WCRT_UNARY_WRAPPERS(log1p)
WCRT_UNARY_WRAPPERS(log2)
WCRT_UNARY_WRAPPERS(logb)
WCRT_UNARY_WRAPPERS(cbrt)
WCRT_BINARY_WRAPPERS(hypot)
WCRT_BINARY_WRAPPERS(pow)
WCRT_UNARY_WRAPPERS(sqrt)
WCRT_UNARY_WRAPPERS(erf)
WCRT_UNARY_WRAPPERS(erfc)
WCRT_UNARY_WRAPPERS(lgamma)
WCRT_UNARY_WRAPPERS(tgamma)
WCRT_UNARY_WRAPPERS(ceil)
WCRT_UNARY_WRAPPERS(floor)
WCRT_UNARY_WRAPPERS(nearbyint)
WCRT_UNARY_WRAPPERS(rint)
WCRT_UNARY_WRAPPERS(round)
WCRT_UNARY_WRAPPERS(trunc)
WCRT_BINARY_WRAPPERS(fmod)
WCRT_BINARY_WRAPPERS(remainder)
WCRT_BINARY_WRAPPERS(fdim)
WCRT_BINARY_WRAPPERS(fmax)
WCRT_BINARY_WRAPPERS(fmin)

float ldexpf(float value, int exponent)
{ return wcrt_float_result(ldexp((double)value, exponent)); }
long double ldexpl(long double value, int exponent)
{ return (long double)ldexp((double)value, exponent); }
float scalbnf(float value, int exponent)
{ return wcrt_float_result(scalbn((double)value, exponent)); }
long double scalbnl(long double value, int exponent)
{ return (long double)scalbn((double)value, exponent); }
float scalblnf(float value, long exponent)
{ return wcrt_float_result(scalbln((double)value, exponent)); }
long double scalblnl(long double value, long exponent)
{ return (long double)scalbln((double)value, exponent); }
int ilogbf(float value) { return ilogb((double)value); }
int ilogbl(long double value) { return ilogb((double)value); }

long lrintf(float value) { return lrint((double)value); }
long lrintl(long double value) { return lrint((double)value); }
long long llrintf(float value) { return llrint((double)value); }
long long llrintl(long double value) { return llrint((double)value); }
long lroundf(float value) { return lround((double)value); }
long lroundl(long double value) { return lround((double)value); }
long long llroundf(float value) { return llround((double)value); }
long long llroundl(long double value) { return llround((double)value); }

float remquof(float numerator, float denominator, int *quotient)
{
    return wcrt_float_result(remquo((double)numerator,
        (double)denominator, quotient));
}

long double remquol(long double numerator, long double denominator,
    int *quotient)
{
    return (long double)remquo((double)numerator, (double)denominator,
        quotient);
}

float fmaf(float lhs, float middle, float rhs)
{
    struct wcrt_fma_operand left = wcrt_fma_decode((double)lhs);
    struct wcrt_fma_operand center = wcrt_fma_decode((double)middle);
    struct wcrt_fma_operand addend = wcrt_fma_decode((double)rhs);
    int product_negative = left.negative != center.negative;
    union wcrt_float_shape result;

    if (left.not_a_number || center.not_a_number || addend.not_a_number) {
        return lhs + middle + rhs;
    }
    if ((left.infinite && center.zero) ||
        (center.infinite && left.zero)) {
        errno = EDOM;
        return wcrt_quiet_nanf();
    }
    if (left.infinite || center.infinite) {
        if (addend.infinite && product_negative != addend.negative) {
            errno = EDOM;
            return wcrt_quiet_nanf();
        }
        return copysignf(wcrt_infinityf(),
            product_negative ? -1.0F : 1.0F);
    }
    if (addend.infinite) return rhs;
    result.bits = (unsigned int)wcrt_fma_finite_bits(&left, &center,
        &addend, 24, -126, 127, 127, 0x80000000ULL, 0x7f7fffffULL);
    return result.value;
}

long double fmal(long double lhs, long double middle, long double rhs)
{
    return (long double)fma((double)lhs, (double)middle, (double)rhs);
}
