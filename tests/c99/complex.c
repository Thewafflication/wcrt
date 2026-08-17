/**
 * @file complex.c
 * @brief Verifies C99 complex arithmetic and numerical behavior.
 */

#include <complex.h>
#include <errno.h>
#include <math.h>

typedef union {
    double complex value;
    double part[2];
} wcrt_test_complex;

typedef char wcrt_complex_i_size[
    sizeof(_Complex_I) == sizeof(float complex) ? 1 : -1];

typedef double complex (*wcrt_complex_function)(double complex);

typedef struct {
    wcrt_complex_function function;
    double real;
    double imaginary;
} wcrt_complex_vector;

static double complex wcrt_make(double real, double imaginary)
{
    wcrt_test_complex value;
    value.part[0] = real;
    value.part[1] = imaginary;
    return value.value;
}

static int wcrt_near(double actual, double expected, double tolerance)
{
    double scale = fmax(1.0, fabs(expected));
    return fabs(actual - expected) <= tolerance * scale;
}

static int wcrt_near_complex(double complex actual, double real,
    double imaginary, double tolerance)
{
    return wcrt_near(creal(actual), real, tolerance) &&
        wcrt_near(cimag(actual), imaginary, tolerance);
}

static int wcrt_same_complex(double complex left, double complex right,
    double tolerance)
{
    return wcrt_near_complex(left, creal(right), cimag(right), tolerance);
}

static int wcrt_check_ordinary(void)
{
    static const wcrt_complex_vector vectors[] = {
        {cacos, 1.1725184532565882, 0.7433204263252785},
        {casin, 0.39827787353830835, -0.7433204263252785},
        {catan, 0.69272418839960093, -0.59021350027950537},
        {ccos, 1.1361914738033483, 0.3942396421115833},
        {csin, 0.62070423107805506, -0.72165082429756455},
        {ctan, 0.29089346182961801, -0.73608417055119091},
        {cacosh, 0.7433204263252785, -1.1725184532565882},
        {casinh, 0.60633499988735129, -0.68220396558343221},
        {catanh, 0.31042828307719578, -0.72322066612406755},
        {ccosh, 0.82507136699460726, -0.35519875789073846},
        {csinh, 0.38127963465217818, -0.76863356469339261},
        {ctanh, 0.72821180128047236, -0.61809639480620238},
        {cexp, 1.2063510016467855, -1.1238323225841311},
        {clog, -0.10381968238912224, -0.98279372324732905},
        {csqrt, 0.83707461401777006, -0.44798873806491907}
    };
    double complex input = wcrt_make(0.5, -0.75);
    unsigned int index;

    for (index = 0; index < sizeof(vectors) / sizeof(vectors[0]); ++index) {
        double complex actual = vectors[index].function(input);
        if (!wcrt_near_complex(actual, vectors[index].real,
                vectors[index].imaginary, 1e-11)) return 20 + (int)index;
    }
    if (!wcrt_near(cabs(input), 0.90138781886599728, 1e-11)) return 40;
    if (!wcrt_near(carg(input), -0.98279372324732905, 1e-11)) return 41;
    if (!wcrt_near_complex(cpow(input, wcrt_make(1.25, -0.5)),
            0.20637320176018403, -0.49610191134001025, 1e-11)) return 42;
    return 0;
}

static int wcrt_check_exact(void)
{
    double complex value = wcrt_make(3.0, -4.0);
    double complex result;
    float complex unit = _Complex_I;

    if (crealf(unit) != 0.0F || cimagf(unit) != 1.0F) return 49;
    result = 1.0 + 2.0 * I;
    if (creal(result) != 1.0 || cimag(result) != 2.0) return 48;

    if (creal(value) != 3.0 || cimag(value) != -4.0) return 50;
    result = conj(value);
    if (creal(result) != 3.0 || cimag(result) != 4.0) return 51;
    if (cabs(value) != 5.0) return 52;
    result = wcrt_make(1.0, 2.0) * wcrt_make(3.0, -4.0);
    if (creal(result) != 11.0 || cimag(result) != 2.0) return 53;
    result = result / wcrt_make(3.0, -4.0);
    if (creal(result) != 1.0 || cimag(result) != 2.0) return 55;
    result = cproj(wcrt_make(INFINITY, -2.0));
    if (!isinf(creal(result)) || cimag(result) != 0.0 ||
        !signbit(cimag(result))) return 54;
    return 0;
}

static int wcrt_check_cuts(void)
{
    double positive_zero = 0.0;
    double negative_zero = copysign(0.0, -1.0);
    double complex upper;
    double complex lower;

    upper = clog(wcrt_make(-2.0, positive_zero));
    lower = clog(wcrt_make(-2.0, negative_zero));
    if (!wcrt_near(cimag(upper), 3.1415926535897932, 1e-15) ||
        !wcrt_near(cimag(lower), -3.1415926535897932, 1e-15)) return 60;
    upper = clog(wcrt_make(negative_zero, positive_zero));
    lower = clog(wcrt_make(negative_zero, negative_zero));
    if (!(cimag(upper) > 3.0) || !(cimag(lower) < -3.0)) return 69;
    upper = csqrt(wcrt_make(-4.0, positive_zero));
    lower = csqrt(wcrt_make(-4.0, negative_zero));
    if (cimag(upper) != 2.0 || cimag(lower) != -2.0) return 61;
    upper = csqrt(wcrt_make(negative_zero, negative_zero));
    if (signbit(creal(upper)) || !signbit(cimag(upper))) return 130;
    upper = casin(wcrt_make(2.0, positive_zero));
    lower = casin(wcrt_make(2.0, negative_zero));
    if (!(cimag(upper) > 0.0) || !(cimag(lower) < 0.0)) return 62;
    upper = cacos(wcrt_make(2.0, positive_zero));
    lower = cacos(wcrt_make(2.0, negative_zero));
    if (!(cimag(upper) < 0.0) || !(cimag(lower) > 0.0)) return 63;
    upper = cacosh(wcrt_make(0.0, positive_zero));
    lower = cacosh(wcrt_make(0.0, negative_zero));
    if (!(cimag(upper) > 0.0) || !(cimag(lower) < 0.0)) return 64;
    upper = catanh(wcrt_make(2.0, positive_zero));
    lower = catanh(wcrt_make(2.0, negative_zero));
    if (!(cimag(upper) > 0.0) || !(cimag(lower) < 0.0)) return 65;
    upper = catan(wcrt_make(positive_zero, 2.0));
    lower = catan(wcrt_make(negative_zero, 2.0));
    if (!(creal(upper) > 0.0) || !(creal(lower) < 0.0)) return 66;
    upper = casinh(wcrt_make(positive_zero, 2.0));
    lower = casinh(wcrt_make(negative_zero, 2.0));
    if (!(creal(upper) > 0.0) || !(creal(lower) < 0.0)) return 67;
    upper = cpow(wcrt_make(-2.0, positive_zero), wcrt_make(0.5, 0.0));
    lower = cpow(wcrt_make(-2.0, negative_zero), wcrt_make(0.5, 0.0));
    if (!(cimag(upper) > 0.0) || !(cimag(lower) < 0.0)) return 68;
    return 0;
}

static int wcrt_check_special(void)
{
    static wcrt_complex_function functions[] = {
        cacos, casin, catan, ccos, csin, ctan, cacosh, casinh,
        catanh, ccosh, csinh, ctanh, cexp, clog, csqrt
    };
    double complex value;
    double nan_value = NAN;
    unsigned int index;

    errno = 0;
    value = cexp(wcrt_make(1000.0, 0.0));
    if (creal(value) != HUGE_VAL || cimag(value) != 0.0) return 70;
    if (errno != ERANGE) return 121;
    errno = 0;
    value = cexp(wcrt_make(-1000.0, -0.0));
    if (creal(value) != 0.0 || cimag(value) != 0.0 ||
        !signbit(cimag(value))) return 71;
    if (errno != ERANGE) return 122;
    errno = 0;
    value = cexp(wcrt_make(-744.0, 0.0));
    if (fpclassify(creal(value)) != FP_SUBNORMAL ||
        cimag(value) != 0.0) return 124;
    value = wcrt_make(HUGE_VAL / 2.0, HUGE_VAL / 2.0);
    if (!isfinite(cabs(value)) || cabs(value) <= HUGE_VAL / 2.0) return 125;
    value = csqrt(wcrt_make(HUGE_VAL, HUGE_VAL));
    if (!isfinite(creal(value)) || !isfinite(cimag(value))) return 126;
    value = csqrt(wcrt_make(0.0, nextafter(0.0, 1.0)));
    if (!(creal(value) > 0.0) || !(cimag(value) > 0.0)) return 127;
    value = csqrt(wcrt_make(-INFINITY, -0.0));
    if (creal(value) != 0.0 || !isinf(cimag(value)) ||
        !signbit(cimag(value))) return 72;
    value = cexp(wcrt_make(1.0, INFINITY));
    if (!isnan(creal(value)) || !isnan(cimag(value))) return 73;
    value = clog(wcrt_make(nan_value, nan_value));
    if (!isnan(creal(value)) || !isnan(cimag(value))) return 74;
    value = clog(wcrt_make(nan_value, 1.0));
    if (!isnan(creal(value)) || !isnan(cimag(value))) return 129;
    for (index = 0; index < sizeof(functions) / sizeof(functions[0]); ++index) {
        value = functions[index](wcrt_make(nan_value, nan_value));
        if (!isnan(creal(value)) && !isnan(cimag(value))) {
            return 100 + (int)index;
        }
    }
    if (!isinf(cabs(wcrt_make(INFINITY, 1.0)))) return 116;
    if (!isnan(cabs(wcrt_make(nan_value, 1.0)))) return 117;
    value = clog(wcrt_make(INFINITY, 1.0));
    if (!isinf(creal(value)) || cimag(value) != 0.0) return 118;
    value = clog(wcrt_make(HUGE_VAL, HUGE_VAL));
    if (!isfinite(creal(value)) ||
        !wcrt_near(cimag(value), 0.7853981633974483, 1e-15)) return 128;
    value = catanh(wcrt_make(INFINITY, 0.0));
    if (creal(value) != 0.0 || !wcrt_near(cimag(value),
            1.5707963267948966, 1e-15)) return 119;
    value = cpow(wcrt_make(nan_value, nan_value), wcrt_make(1.0, 0.0));
    if (!isnan(creal(value)) && !isnan(cimag(value))) return 120;
    errno = 0;
    value = clog(wcrt_make(0.0, 0.0));
    if (creal(value) != -HUGE_VAL || errno != EDOM) return 123;
    return 0;
}

static int wcrt_check_symmetry(void)
{
    static wcrt_complex_function functions[] = {
        cacos, casin, catan, ccos, csin, ctan, cacosh, casinh,
        catanh, ccosh, csinh, ctanh, cexp, clog, csqrt
    };
    double complex value = wcrt_make(0.375, 0.625);
    unsigned int index;

    for (index = 0; index < sizeof(functions) / sizeof(functions[0]); ++index) {
        double complex left = functions[index](conj(value));
        double complex right = conj(functions[index](value));
        if (!wcrt_same_complex(left, right, 1e-11)) return 80 + (int)index;
    }
    if (!wcrt_same_complex(cpow(conj(value), wcrt_make(1.25, 0.0)),
            conj(cpow(value, wcrt_make(1.25, 0.0))), 1e-11)) return 98;
    return 0;
}

static int wcrt_check_precisions(void)
{
    float complex single = wcrt_make(0.25, -0.5);
    long double complex extended = (long double complex)single;
    float complex single_result = cexpf(single);
    long double complex long_result = cexpl(extended);
    float complex single_product = single * single;
    long double complex long_product = extended * extended;

    if (!wcrt_near(crealf(single_result), 1.1268383, 1e-5) ||
        !wcrt_near(cimagf(single_result), -0.6155946, 1e-5)) return 90;
    if (!wcrt_near((double)creall(long_result), 1.1268383147091814,
            1e-11)) return 91;
    if (!wcrt_near((double)cimagl(long_result), -0.6155945769770066,
            1e-11)) return 92;
    if (crealf(single_product) != -0.1875F ||
        cimagf(single_product) != -0.25F) return 93;
    single_product = single_product / single;
    if (crealf(single_product) != 0.25F ||
        cimagf(single_product) != -0.5F) return 94;
    if (creall(long_product) != -0.1875L ||
        cimagl(long_product) != -0.25L) return 95;
    long_product = long_product / extended;
    if (creall(long_product) != 0.25L ||
        cimagl(long_product) != -0.5L) return 96;
    return 0;
}

int main(void)
{
    int result;
    result = wcrt_check_ordinary(); if (result) return result;
    result = wcrt_check_exact(); if (result) return result;
    result = wcrt_check_cuts(); if (result) return result;
    result = wcrt_check_special(); if (result) return result;
    result = wcrt_check_symmetry(); if (result) return result;
    result = wcrt_check_precisions(); if (result) return result;
    return 0;
}
