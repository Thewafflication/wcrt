/**
 * @file tgmath.c
 * @brief Verifies C99 type-generic selection and single evaluation.
 */

#include <tgmath.h>

#define WCRT_TYPE(value) _Generic((value), \
    float: 1, double: 2, long double: 3, \
    float complex: 4, double complex: 5, \
    long double complex: 6, long: 7, long long: 8, int: 9, default: 0)

#define WCRT_ASSERT_SHARED(name) \
    typedef char wcrt_##name##_float[ \
        WCRT_TYPE(name(1.0F)) == 1 ? 1 : -1]; \
    typedef char wcrt_##name##_complex[ \
        WCRT_TYPE(name((double complex)1.0)) == 5 ? 1 : -1]

#define WCRT_ASSERT_REAL_UNARY(name) \
    typedef char wcrt_##name##_float[ \
        WCRT_TYPE(name(1.0F)) == 1 ? 1 : -1]; \
    typedef char wcrt_##name##_long[ \
        WCRT_TYPE(name(1.0L)) == 3 ? 1 : -1]

#define WCRT_ASSERT_REAL_BINARY(name) \
    typedef char wcrt_##name##_float[ \
        WCRT_TYPE(name(1.0F, 2.0F)) == 1 ? 1 : -1]; \
    typedef char wcrt_##name##_mixed[ \
        WCRT_TYPE(name(1.0F, 2.0L)) == 3 ? 1 : -1]

WCRT_ASSERT_SHARED(acos);
WCRT_ASSERT_SHARED(asin);
WCRT_ASSERT_SHARED(atan);
WCRT_ASSERT_SHARED(acosh);
WCRT_ASSERT_SHARED(asinh);
WCRT_ASSERT_SHARED(atanh);
WCRT_ASSERT_SHARED(cos);
WCRT_ASSERT_SHARED(sin);
WCRT_ASSERT_SHARED(tan);
WCRT_ASSERT_SHARED(cosh);
WCRT_ASSERT_SHARED(sinh);
WCRT_ASSERT_SHARED(tanh);
WCRT_ASSERT_SHARED(exp);
WCRT_ASSERT_SHARED(log);
WCRT_ASSERT_SHARED(sqrt);

WCRT_ASSERT_REAL_UNARY(cbrt);
WCRT_ASSERT_REAL_UNARY(ceil);
WCRT_ASSERT_REAL_UNARY(erf);
WCRT_ASSERT_REAL_UNARY(erfc);
WCRT_ASSERT_REAL_UNARY(exp2);
WCRT_ASSERT_REAL_UNARY(expm1);
WCRT_ASSERT_REAL_UNARY(floor);
WCRT_ASSERT_REAL_UNARY(lgamma);
WCRT_ASSERT_REAL_UNARY(log10);
WCRT_ASSERT_REAL_UNARY(log1p);
WCRT_ASSERT_REAL_UNARY(log2);
WCRT_ASSERT_REAL_UNARY(logb);
WCRT_ASSERT_REAL_UNARY(nearbyint);
WCRT_ASSERT_REAL_UNARY(rint);
WCRT_ASSERT_REAL_UNARY(round);
WCRT_ASSERT_REAL_UNARY(tgamma);
WCRT_ASSERT_REAL_UNARY(trunc);

WCRT_ASSERT_REAL_BINARY(atan2);
WCRT_ASSERT_REAL_BINARY(copysign);
WCRT_ASSERT_REAL_BINARY(fdim);
WCRT_ASSERT_REAL_BINARY(fmax);
WCRT_ASSERT_REAL_BINARY(fmin);
WCRT_ASSERT_REAL_BINARY(fmod);
WCRT_ASSERT_REAL_BINARY(hypot);
WCRT_ASSERT_REAL_BINARY(nextafter);
WCRT_ASSERT_REAL_BINARY(remainder);

typedef char wcrt_acos_float[WCRT_TYPE(acos(1.0F)) == 1 ? 1 : -1];
typedef char wcrt_acos_integer[WCRT_TYPE(acos(1)) == 2 ? 1 : -1];
typedef char wcrt_acos_long[WCRT_TYPE(acos(1.0L)) == 3 ? 1 : -1];
typedef char wcrt_acos_complex[
    WCRT_TYPE(acos((float complex)1.0F)) == 4 ? 1 : -1];
typedef char wcrt_pow_mixed[
    WCRT_TYPE(pow(1.0F, 2.0L)) == 3 ? 1 : -1];
typedef char wcrt_pow_complex[
    WCRT_TYPE(pow((double complex)1.0, 2.0F)) == 5 ? 1 : -1];
typedef char wcrt_pow_float_complex[
    WCRT_TYPE(pow(1.0F, (float complex)2.0F)) == 4 ? 1 : -1];
typedef char wcrt_pow_integer_complex[
    WCRT_TYPE(pow(1, (float complex)2.0F)) == 5 ? 1 : -1];
typedef char wcrt_pow_long_complex_right[
    WCRT_TYPE(pow(1.0L, (float complex)2.0F)) == 6 ? 1 : -1];
typedef char wcrt_pow_long_complex_left[
    WCRT_TYPE(pow((float complex)1.0F, 2.0L)) == 6 ? 1 : -1];
typedef char wcrt_fabs_complex[
    WCRT_TYPE(fabs((float complex)1.0F)) == 1 ? 1 : -1];
typedef char wcrt_carg_float[
    WCRT_TYPE(carg((float complex)1.0F)) == 1 ? 1 : -1];
typedef char wcrt_conj_real[WCRT_TYPE(conj(1.0L)) == 6 ? 1 : -1];
typedef char wcrt_lround_type[WCRT_TYPE(lround(1.0F)) == 7 ? 1 : -1];
typedef char wcrt_llround_type[
    WCRT_TYPE(llround(1.0F)) == 8 ? 1 : -1];
typedef char wcrt_fma_type[
    WCRT_TYPE(fma(1.0F, 2.0F, 3.0F)) == 1 ? 1 : -1];
typedef char wcrt_fma_mixed_type[
    WCRT_TYPE(fma(1.0F, 2.0F, 3.0L)) == 3 ? 1 : -1];
typedef char wcrt_frexp_type[
    WCRT_TYPE(frexp(1.0F, (int *)0)) == 1 ? 1 : -1];
typedef char wcrt_ilogb_type[WCRT_TYPE(ilogb(1.0L)) == 9 ? 1 : -1];
typedef char wcrt_ldexp_type[WCRT_TYPE(ldexp(1.0F, 2)) == 1 ? 1 : -1];
typedef char wcrt_llrint_type[WCRT_TYPE(llrint(1.0L)) == 8 ? 1 : -1];
typedef char wcrt_lrint_type[WCRT_TYPE(lrint(1.0L)) == 7 ? 1 : -1];
typedef char wcrt_nexttoward_type[
    WCRT_TYPE(nexttoward(1.0F, 2.0L)) == 1 ? 1 : -1];
typedef char wcrt_remquo_type[
    WCRT_TYPE(remquo(1.0F, 2.0F, (int *)0)) == 1 ? 1 : -1];
typedef char wcrt_remquo_mixed_type[
    WCRT_TYPE(remquo(1.0F, 2.0L, (int *)0)) == 3 ? 1 : -1];
typedef char wcrt_scalbln_type[
    WCRT_TYPE(scalbln(1.0L, 2L)) == 3 ? 1 : -1];
typedef char wcrt_scalbn_type[
    WCRT_TYPE(scalbn(1.0F, 2)) == 1 ? 1 : -1];
typedef char wcrt_cimag_type[
    WCRT_TYPE(cimag((long double complex)1.0L)) == 3 ? 1 : -1];
typedef char wcrt_cproj_type[WCRT_TYPE(cproj(1.0F)) == 4 ? 1 : -1];
typedef char wcrt_creal_type[
    WCRT_TYPE(creal((double complex)1.0)) == 2 ? 1 : -1];

typedef union {
    double complex value;
    double part[2];
} wcrt_test_complex;

static int wcrt_left_count;
static int wcrt_middle_count;
static int wcrt_right_count;

static float wcrt_next_float(void)
{
    ++wcrt_left_count;
    return 0.25F;
}

static double wcrt_next_double(void)
{
    ++wcrt_middle_count;
    return 0.5;
}

static long double wcrt_next_long_double(void)
{
    ++wcrt_right_count;
    return 0.75L;
}

static double complex wcrt_next_complex(void)
{
    wcrt_test_complex value;
    ++wcrt_left_count;
    value.part[0] = 0.25;
    value.part[1] = -0.5;
    return value.value;
}

static int wcrt_near(double actual, double expected)
{
    return fabs(actual - expected) <= 1e-11;
}

int main(void)
{
    double complex complex_result;
    double (*ordinary_acos)(double) = (acos);
    const float qualified_float = 0.25F;
    volatile double qualified_double = 0.5;
    int exponent;
    int quotient;

    wcrt_left_count = 0;
    if (!wcrt_near(fabs(wcrt_next_float()), 0.25)) return 10;
    if (wcrt_left_count != 1) return 11;

    wcrt_left_count = 0;
    wcrt_middle_count = 0;
    if (!wcrt_near(pow(wcrt_next_float(), wcrt_next_double()), 0.5)) {
        return 12;
    }
    if (wcrt_left_count != 1 || wcrt_middle_count != 1) return 13;

    wcrt_left_count = 0;
    wcrt_middle_count = 0;
    wcrt_right_count = 0;
    (void)fma(wcrt_next_float(), wcrt_next_double(),
        wcrt_next_long_double());
    if (wcrt_left_count != 1 || wcrt_middle_count != 1 ||
        wcrt_right_count != 1) return 14;

    wcrt_left_count = 0;
    complex_result = exp(wcrt_next_complex());
    if (wcrt_left_count != 1) return 15;
    if (!wcrt_near(creal(complex_result), 1.1268383147091814)) return 16;

    wcrt_left_count = 0;
    if (!wcrt_near(fabs(wcrt_next_complex()), 0.55901699437494745)) {
        return 20;
    }
    if (wcrt_left_count != 1) return 21;

    if (!wcrt_near(ordinary_acos(0.5), 1.0471975511965977)) return 17;
    if (WCRT_TYPE(fabs(qualified_float)) != 1 ||
        WCRT_TYPE(exp(qualified_double)) != 2) return 22;
    if (!wcrt_near(frexp(0.75F, &exponent), 0.75) || exponent != 0) {
        return 18;
    }
    (void)remquo(3.0F, 2.0, &quotient);
    if (!wcrt_near(nexttoward(1.0F, 2.0L), nexttowardf(1.0F, 2.0L))) {
        return 19;
    }
    return 0;
}
