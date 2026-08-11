/**
 * @file math.c
 * @brief Exercises the C99 real-math families and representation behavior.
 */

#include <errno.h>
#include <float.h>
#include <math.h>

union wcrt_test_double {
    double value;
    unsigned long long bits;
};

union wcrt_test_float {
    float value;
    unsigned int bits;
};

static int wcrt_evaluations;

static double once_double(double value)
{
    ++wcrt_evaluations;
    return value;
}

static float once_float(float value)
{
    ++wcrt_evaluations;
    return value;
}

static int nearly_equal(double actual, double expected, double tolerance)
{
    double scale = fabs(expected);
    double difference = fabs(actual - expected);
    if (scale < 1.0) scale = 1.0;
    return difference <= tolerance * scale;
}

static int check_macros(void)
{
    union wcrt_test_double positive_subnormal;
    union wcrt_test_double negative_zero;
    union wcrt_test_double positive_infinity;
    union wcrt_test_double quiet_nan;
    union wcrt_test_float float_subnormal;

    positive_subnormal.bits = 1ULL;
    negative_zero.bits = 0x8000000000000000ULL;
    positive_infinity.bits = 0x7ff0000000000000ULL;
    quiet_nan.bits = 0x7ff8000000000000ULL;
    float_subnormal.bits = 1U;

    if (fpclassify(positive_subnormal.value) != FP_SUBNORMAL) return 1;
    if (fpclassify(float_subnormal.value) != FP_SUBNORMAL) return 2;
    if (fpclassify(negative_zero.value) != FP_ZERO) return 3;
    if (fpclassify(positive_infinity.value) != FP_INFINITE) return 4;
    if (fpclassify(quiet_nan.value) != FP_NAN) return 5;
    if (!isfinite(1.0) || isfinite(positive_infinity.value)) return 6;
    if (!isnormal(1.0) || isnormal(positive_subnormal.value)) return 7;
    if (!isinf(positive_infinity.value) || !isnan(quiet_nan.value)) return 8;
    if (!signbit(negative_zero.value) || signbit(0.0)) return 9;
    if (!isgreater(2.0, 1.0) || !isgreaterequal(2.0, 2.0) ||
        !isless(1.0, 2.0) || !islessequal(2.0, 2.0) ||
        !islessgreater(1.0, 2.0) ||
        !isunordered(quiet_nan.value, 1.0)) return 10;
    if (isgreater(quiet_nan.value, 1.0) ||
        isless(quiet_nan.value, 1.0)) return 11;

    wcrt_evaluations = 0;
    if (!isfinite(once_double(1.0)) || wcrt_evaluations != 1) return 12;
    wcrt_evaluations = 0;
    if (!isgreater(once_float(2.0F), once_float(1.0F)) ||
        wcrt_evaluations != 2) return 13;
    return 0;
}

static int check_exact_operations(void)
{
    union wcrt_test_double value;
    union wcrt_test_double result;
    union wcrt_test_float float_result;
    double integral;
    float float_integral;
    int exponent;

    value.bits = 0x3ff0000000000000ULL;
    result.value = nextafter(value.value, 2.0);
    if (result.bits != 0x3ff0000000000001ULL) return 1;
    result.value = nextafter(value.value, 0.0);
    if (result.bits != 0x3fefffffffffffffULL) return 2;
    result.value = nextafter(0.0, -1.0);
    if (result.bits != 0x8000000000000001ULL) return 3;
    float_result.value = nextafterf(1.0F, 2.0F);
    if (float_result.bits != 0x3f800001U) return 4;
    result.value = copysign(1.0, -0.0);
    if (result.bits != 0xbff0000000000000ULL) return 5;
    if (!isnan(nan("ignored")) || !isnan(nanf("ignored")) ||
        !isnan(nanl("ignored"))) return 6;
    if (modf(-3.25, &integral) != -0.25 || integral != -3.0) return 7;
    if (modff(-3.25F, &float_integral) != -0.25F ||
        float_integral != -3.0F) return 8;
    if (frexp(8.0, &exponent) != 0.5 || exponent != 4) return 9;
    value.bits = 1ULL;
    result.value = frexp(value.value, &exponent);
    if (result.value != 0.5 || exponent != -1073) return 10;
    if (scalbn(0.5, 4) != 8.0 || scalbln(0.5, 4L) != 8.0 ||
        ldexp(0.5, 4) != 8.0) return 11;
    if (ilogb(8.0) != 3 || logb(8.0) != 3.0) return 12;
    if (floor(-2.25) != -3.0 || ceil(-2.75) != -2.0 ||
        trunc(-2.75) != -2.0 || round(-2.5) != -3.0) return 13;
    value.bits = 5ULL;
    result.bits = 2ULL;
    result.value = fmod(value.value, result.value);
    if (result.bits != 1ULL) return 14;
    return 0;
}

static int check_function_families(void)
{
    int quotient;
    double fused;
    double small = 0.000000007450580596923828125;

    if (!nearly_equal(acos(0.5), 1.047197551196598, 1e-11) ||
        !nearly_equal(asin(0.5), 0.523598775598299, 1e-11) ||
        !nearly_equal(atan(1.0), 0.785398163397448, 1e-11) ||
        !nearly_equal(atan2(1.0, -1.0), 2.356194490192345, 1e-11) ||
        !nearly_equal(cos(1.0), 0.540302305868140, 1e-11) ||
        !nearly_equal(sin(1.0), 0.841470984807897, 1e-11) ||
        !nearly_equal(tan(0.5), 0.546302489843790, 1e-11)) return 1;
    if (!nearly_equal(acosh(2.0), 1.316957896924817, 1e-11) ||
        !nearly_equal(asinh(1.0), 0.881373587019543, 1e-11) ||
        !nearly_equal(atanh(0.5), 0.549306144334055, 1e-11) ||
        !nearly_equal(cosh(1.0), 1.543080634815244, 1e-11) ||
        !nearly_equal(sinh(1.0), 1.175201193643801, 1e-11) ||
        !nearly_equal(tanh(1.0), 0.761594155955765, 1e-11) ||
        !isfinite(acosh(DBL_MAX)) || !isfinite(asinh(DBL_MAX))) return 2;
    if (!nearly_equal(exp(1.0), 2.718281828459045, 1e-11) ||
        exp2(10.0) != 1024.0 ||
        !nearly_equal(expm1(0.001), 0.001000500166708, 1e-12) ||
        !nearly_equal(log(2.0), 0.693147180559945, 1e-11) ||
        !nearly_equal(log10(1000.0), 3.0, 1e-11) ||
        !nearly_equal(log1p(0.5), 0.405465108108164, 1e-11) ||
        !nearly_equal(log2(8.0), 3.0, 1e-11)) return 3;
    if (!nearly_equal(cbrt(27.0), 3.0, 1e-11) ||
        hypot(3.0, 4.0) != 5.0 ||
        !nearly_equal(pow(10.0, 0.5), 3.162277660168379, 1e-11) ||
        !nearly_equal(sqrt(2.0), 1.414213562373095, 1e-11) ||
        fabs(-2.0) != 2.0) return 4;
    if (!nearly_equal(erf(1.0), 0.842700792949715, 1e-11) ||
        !nearly_equal(erfc(1.0), 0.157299207050285, 1e-11) ||
        !nearly_equal(lgamma(5.0), 3.178053830347946, 1e-10) ||
        !nearly_equal(tgamma(5.0), 24.0, 1e-10)) return 5;
    if (remainder(5.5, 2.0) != -0.5 ||
        remquo(5.5, 2.0, &quotient) != -0.5 || quotient != 3 ||
        fmod(7.0, 2.5) != 2.0) return 6;
    if (fmax(2.0, 3.0) != 3.0 || fmin(2.0, 3.0) != 2.0 ||
        fdim(3.0, 2.0) != 1.0 || fdim(2.0, 3.0) != 0.0) return 7;
    if (signbit(fmax(-0.0, 0.0)) || !signbit(fmin(-0.0, 0.0))) return 8;
    fused = fma(1.0 + small, 1.0 - small, -1.0);
    if (fused != -5.5511151231257827e-17) return 9;
    if (fma(DBL_MAX, 2.0, -DBL_MAX) != DBL_MAX) return 10;

    if (!nearly_equal((double)acosf(0.5F), 1.04719755, 1e-6) ||
        !nearly_equal((double)asinl(0.5L), 0.523598775598299, 1e-11) ||
        !nearly_equal((double)exp2f(3.0F), 8.0, 1e-6) ||
        !nearly_equal((double)log1pl(0.5L), 0.405465108108164, 1e-11) ||
        !nearly_equal((double)cbrtf(8.0F), 2.0, 1e-6) ||
        !nearly_equal((double)hypotl(3.0L, 4.0L), 5.0, 1e-11) ||
        lroundf(2.5F) != 3L || llroundl(-2.5L) != -3LL ||
        fmaf(2.0F, 3.0F, 4.0F) != 10.0F ||
        fmal(2.0L, 3.0L, 4.0L) != 10.0L) return 11;
    return 0;
}

static int check_errors(void)
{
    errno = 0;
    (void)pow(-2.0, 0.5);
    if (errno != EDOM) return 1;
    errno = 0;
    (void)log(0.0);
    if (errno != EDOM) return 2;
    errno = 0;
    (void)acosh(0.5);
    if (errno != EDOM) return 3;
    errno = 0;
    (void)atanh(2.0);
    if (errno != EDOM) return 4;
    errno = 0;
    (void)exp(1000.0);
    if (errno != ERANGE) return 5;
    errno = 0;
    (void)ldexp(DBL_MAX, 1);
    if (errno != ERANGE) return 6;
    errno = 0;
    (void)fmod(1.0, 0.0);
    if (errno != EDOM) return 7;
    errno = 0;
    if (expf(100.0F) != HUGE_VALF || errno != ERANGE) return 8;
    errno = 0;
    if (expf(-200.0F) != 0.0F || errno != ERANGE) return 9;
    return 0;
}

int main(void)
{
    int check;
    if (math_errhandling != MATH_ERRNO) return 1;
    if (sizeof(long double) != sizeof(double)) return 2;
    check = check_macros();
    if (check != 0) return 10 + check;
    check = check_exact_operations();
    if (check != 0) return 30 + check;
    check = check_function_families();
    if (check != 0) return 50 + check;
    check = check_errors();
    if (check != 0) return 70 + check;
    return 0;
}
