/**
 * @file complex.c
 * @brief Implements the C99 complex mathematics library.
 */

#include <complex.h>
#include <math.h>

#define WCRT_PI 3.14159265358979323846264338327950288
#define WCRT_HALF_PI 1.57079632679489661923132169163975144
#define WCRT_LOG_MAX 709.782712893383973096206318587

typedef union {
    double complex value;
    double part[2];
} wcrt_complex_double;

typedef union {
    float complex value;
    float part[2];
} wcrt_complex_float;

typedef union {
    long double complex value;
    long double part[2];
} wcrt_complex_long_double;

static double complex wcrt_make_complex(double real, double imaginary)
{
    wcrt_complex_double result;
    result.part[0] = real;
    result.part[1] = imaginary;
    return result.value;
}

static float complex wcrt_make_complexf(float real, float imaginary)
{
    wcrt_complex_float result;
    result.part[0] = real;
    result.part[1] = imaginary;
    return result.value;
}

static long double complex wcrt_make_complexl(long double real,
    long double imaginary)
{
    wcrt_complex_long_double result;
    result.part[0] = real;
    result.part[1] = imaginary;
    return result.value;
}

static double wcrt_real(double complex value)
{
    wcrt_complex_double parts;
    parts.value = value;
    return parts.part[0];
}

static double wcrt_imag(double complex value)
{
    wcrt_complex_double parts;
    parts.value = value;
    return parts.part[1];
}

static double wcrt_nan_value(void)
{
    return nan("");
}

static double wcrt_log_magnitude(double real, double imaginary)
{
    double large;
    double small;
    double ratio;

    if (isinf(real) || isinf(imaginary)) return INFINITY;
    if (isnan(real) || isnan(imaginary)) return wcrt_nan_value();
    large = fmax(fabs(real), fabs(imaginary));
    small = fmin(fabs(real), fabs(imaginary));
    if (large == 0.0) return log(0.0);
    ratio = small / large;
    return log(large) + 0.5 * log1p(ratio * ratio);
}

static double wcrt_scale_infinite(double scale, double factor)
{
    if (factor == 0.0) return copysign(0.0, factor);
    return scale * factor;
}

static double complex wcrt_multiply(double complex left,
    double complex right)
{
    double a = wcrt_real(left);
    double b = wcrt_imag(left);
    double c = wcrt_real(right);
    double d = wcrt_imag(right);
    return wcrt_make_complex(a * c - b * d, a * d + b * c);
}

double creal(double complex value)
{
    return wcrt_real(value);
}

float crealf(float complex value)
{
    wcrt_complex_float parts;
    parts.value = value;
    return parts.part[0];
}

long double creall(long double complex value)
{
    wcrt_complex_long_double parts;
    parts.value = value;
    return parts.part[0];
}

double cimag(double complex value)
{
    return wcrt_imag(value);
}

float cimagf(float complex value)
{
    wcrt_complex_float parts;
    parts.value = value;
    return parts.part[1];
}

long double cimagl(long double complex value)
{
    wcrt_complex_long_double parts;
    parts.value = value;
    return parts.part[1];
}

double complex conj(double complex value)
{
    return wcrt_make_complex(wcrt_real(value), -wcrt_imag(value));
}

float complex conjf(float complex value)
{
    return wcrt_make_complexf(crealf(value), -cimagf(value));
}

long double complex conjl(long double complex value)
{
    return wcrt_make_complexl(creall(value), -cimagl(value));
}

double complex cproj(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    if (isinf(real) || isinf(imaginary)) {
        return wcrt_make_complex(INFINITY, copysign(0.0, imaginary));
    }
    return value;
}

float complex cprojf(float complex value)
{
    float real = crealf(value);
    float imaginary = cimagf(value);
    if (isinf(real) || isinf(imaginary)) {
        return wcrt_make_complexf(INFINITY, copysignf(0.0F, imaginary));
    }
    return value;
}

long double complex cprojl(long double complex value)
{
    long double real = creall(value);
    long double imaginary = cimagl(value);
    if (isinf(real) || isinf(imaginary)) {
        return wcrt_make_complexl(INFINITY,
            copysignl(0.0L, imaginary));
    }
    return value;
}

double cabs(double complex value)
{
    return hypot(wcrt_real(value), wcrt_imag(value));
}

float cabsf(float complex value)
{
    return hypotf(crealf(value), cimagf(value));
}

long double cabsl(long double complex value)
{
    return hypotl(creall(value), cimagl(value));
}

double carg(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    if (imaginary == 0.0 &&
        (real < 0.0 || (real == 0.0 && signbit(real)))) {
        return copysign(WCRT_PI, imaginary);
    }
    return atan2(imaginary, real);
}

float cargf(float complex value)
{
    return (float)carg((double complex)value);
}

long double cargl(long double complex value)
{
    return (long double)carg((double complex)value);
}

double complex cexp(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    double magnitude;
    double cosine;
    double sine;

    if (imaginary == 0.0) {
        return wcrt_make_complex(exp(real), imaginary);
    }
    if (isinf(imaginary) || isnan(imaginary)) {
        magnitude = wcrt_nan_value();
        return wcrt_make_complex(magnitude, magnitude);
    }
    magnitude = exp(real);
    cosine = cos(imaginary);
    sine = sin(imaginary);
    if (isinf(magnitude)) {
        return wcrt_make_complex(wcrt_scale_infinite(magnitude, cosine),
            wcrt_scale_infinite(magnitude, sine));
    }
    return wcrt_make_complex(magnitude * cosine, magnitude * sine);
}

double complex clog(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    return wcrt_make_complex(wcrt_log_magnitude(real, imaginary),
        carg(value));
}

double complex csqrt(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    double magnitude;
    double result_real;
    double result_imaginary;
    double scale;

    if (isinf(imaginary)) {
        return wcrt_make_complex(INFINITY, imaginary);
    }
    if (isinf(real)) {
        if (real > 0.0) {
            return wcrt_make_complex(INFINITY,
                copysign(0.0, imaginary));
        }
        return wcrt_make_complex(0.0, copysign(INFINITY, imaginary));
    }
    if (isnan(real) || isnan(imaginary)) {
        magnitude = wcrt_nan_value();
        return wcrt_make_complex(magnitude, magnitude);
    }
    if (imaginary == 0.0) {
        if (real == 0.0) {
            return wcrt_make_complex(0.0, imaginary);
        }
        if (real < 0.0) {
            return wcrt_make_complex(0.0,
                copysign(sqrt(-real), imaginary));
        }
        return wcrt_make_complex(sqrt(real), imaginary);
    }
    scale = fmax(fabs(real), fabs(imaginary));
    if (scale < 1.0e-200) {
        double complex scaled = csqrt(wcrt_make_complex(
            scalbn(real, 512), scalbn(imaginary, 512)));
        return wcrt_make_complex(scalbn(wcrt_real(scaled), -256),
            scalbn(wcrt_imag(scaled), -256));
    }
    magnitude = (0.5 * scale) * sqrt((real / scale) * (real / scale) +
        (imaginary / scale) * (imaginary / scale));
    if (real >= 0.0) {
        result_real = sqrt(scale) * sqrt(magnitude / scale +
            0.5 * (real / scale));
        result_imaginary = imaginary / (2.0 * result_real);
    } else {
        result_imaginary = copysign(
            sqrt(scale) * sqrt(magnitude / scale -
                0.5 * (real / scale)), imaginary);
        result_real = fabs(imaginary) / (2.0 * fabs(result_imaginary));
    }
    return wcrt_make_complex(result_real, result_imaginary);
}

double complex cpow(double complex base, double complex power)
{
    double power_real = wcrt_real(power);
    double power_imaginary = wcrt_imag(power);
    double complex logarithm;
    double a;
    double b;

    if (power_real == 0.0 && power_imaginary == 0.0) {
        return wcrt_make_complex(1.0, copysign(0.0, power_imaginary));
    }
    logarithm = clog(base);
    a = wcrt_real(logarithm);
    b = wcrt_imag(logarithm);
    return cexp(wcrt_make_complex(power_real * a - power_imaginary * b,
        power_real * b + power_imaginary * a));
}

double complex csin(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    return wcrt_make_complex(sin(real) * cosh(imaginary),
        cos(real) * sinh(imaginary));
}

double complex ccos(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    return wcrt_make_complex(cos(real) * cosh(imaginary),
        -sin(real) * sinh(imaginary));
}

double complex ctan(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    double denominator;

    if (fabs(imaginary) > WCRT_LOG_MAX / 2.0) {
        return wcrt_make_complex(copysign(0.0, sin(2.0 * real)),
            copysign(1.0, imaginary));
    }
    denominator = cos(2.0 * real) + cosh(2.0 * imaginary);
    return wcrt_make_complex(sin(2.0 * real) / denominator,
        sinh(2.0 * imaginary) / denominator);
}

double complex csinh(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    return wcrt_make_complex(sinh(real) * cos(imaginary),
        cosh(real) * sin(imaginary));
}

double complex ccosh(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    return wcrt_make_complex(cosh(real) * cos(imaginary),
        sinh(real) * sin(imaginary));
}

double complex ctanh(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    double denominator;

    if (fabs(real) > WCRT_LOG_MAX / 2.0) {
        return wcrt_make_complex(copysign(1.0, real),
            copysign(0.0, sin(2.0 * imaginary)));
    }
    denominator = cosh(2.0 * real) + cos(2.0 * imaginary);
    return wcrt_make_complex(sinh(2.0 * real) / denominator,
        sin(2.0 * imaginary) / denominator);
}

double complex casin(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    double magnitude;
    double complex square;
    double complex root;
    double complex logarithm;

    if (imaginary == 0.0) {
        if (fabs(real) <= 1.0) {
            return wcrt_make_complex(asin(real), imaginary);
        }
        magnitude = acosh(fabs(real));
        return wcrt_make_complex(copysign(WCRT_HALF_PI, real),
            copysign(magnitude, imaginary));
    }
    square = wcrt_multiply(value, value);
    root = csqrt(wcrt_make_complex(1.0 - wcrt_real(square),
        -wcrt_imag(square)));
    logarithm = clog(wcrt_make_complex(-imaginary + wcrt_real(root),
        real + wcrt_imag(root)));
    return wcrt_make_complex(wcrt_imag(logarithm),
        -wcrt_real(logarithm));
}

double complex cacos(double complex value)
{
    double complex sine = casin(value);
    return wcrt_make_complex(WCRT_HALF_PI - wcrt_real(sine),
        -wcrt_imag(sine));
}

double complex catan(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    double complex first;
    double complex second;
    double complex difference;

    if (imaginary == 0.0) {
        return wcrt_make_complex(atan(real), imaginary);
    }
    first = clog(wcrt_make_complex(1.0 + imaginary, -real));
    second = clog(wcrt_make_complex(1.0 - imaginary, real));
    difference = wcrt_make_complex(wcrt_real(first) - wcrt_real(second),
        wcrt_imag(first) - wcrt_imag(second));
    return wcrt_make_complex(-0.5 * wcrt_imag(difference),
        0.5 * wcrt_real(difference));
}

double complex casinh(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    double complex square;
    double complex root;

    if (imaginary == 0.0) {
        return wcrt_make_complex(asinh(real), imaginary);
    }
    square = wcrt_multiply(value, value);
    root = csqrt(wcrt_make_complex(wcrt_real(square) + 1.0,
        wcrt_imag(square)));
    return clog(wcrt_make_complex(real + wcrt_real(root),
        imaginary + wcrt_imag(root)));
}

double complex cacosh(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    double magnitude;
    double complex plus;
    double complex minus;
    double complex product;

    if (imaginary == 0.0) {
        if (real >= 1.0) {
            return wcrt_make_complex(acosh(real), imaginary);
        }
        if (real >= -1.0) magnitude = acos(real);
        else magnitude = WCRT_PI;
        return wcrt_make_complex(real < -1.0 ? acosh(-real) : 0.0,
            copysign(magnitude, imaginary));
    }
    plus = csqrt(wcrt_make_complex(real + 1.0, imaginary));
    minus = csqrt(wcrt_make_complex(real - 1.0, imaginary));
    product = wcrt_multiply(plus, minus);
    return clog(wcrt_make_complex(real + wcrt_real(product),
        imaginary + wcrt_imag(product)));
}

double complex catanh(double complex value)
{
    double real = wcrt_real(value);
    double imaginary = wcrt_imag(value);
    double magnitude;
    double complex first;
    double complex second;

    if (imaginary == 0.0) {
        if (isinf(real)) {
            return wcrt_make_complex(copysign(0.0, real),
                copysign(WCRT_HALF_PI, imaginary));
        }
        if (fabs(real) < 1.0) {
            return wcrt_make_complex(atanh(real), imaginary);
        }
        if (fabs(real) == 1.0) {
            return wcrt_make_complex(copysign(INFINITY, real), imaginary);
        }
        magnitude = 0.5 * log(fabs((1.0 + real) / (1.0 - real)));
        return wcrt_make_complex(magnitude,
            copysign(WCRT_HALF_PI, imaginary));
    }
    first = clog(wcrt_make_complex(1.0 + real, imaginary));
    second = clog(wcrt_make_complex(1.0 - real, -imaginary));
    return wcrt_make_complex(
        0.5 * (wcrt_real(first) - wcrt_real(second)),
        0.5 * (wcrt_imag(first) - wcrt_imag(second)));
}

#define WCRT_COMPLEX_WRAPPERS(name) \
float complex name##f(float complex value) \
{ \
    double complex result = name((double complex)value); \
    return wcrt_make_complexf((float)wcrt_real(result), \
        (float)wcrt_imag(result)); \
} \
long double complex name##l(long double complex value) \
{ \
    double complex result = name((double complex)value); \
    return wcrt_make_complexl((long double)wcrt_real(result), \
        (long double)wcrt_imag(result)); \
}

WCRT_COMPLEX_WRAPPERS(cacos)
WCRT_COMPLEX_WRAPPERS(casin)
WCRT_COMPLEX_WRAPPERS(catan)
WCRT_COMPLEX_WRAPPERS(ccos)
WCRT_COMPLEX_WRAPPERS(csin)
WCRT_COMPLEX_WRAPPERS(ctan)
WCRT_COMPLEX_WRAPPERS(cacosh)
WCRT_COMPLEX_WRAPPERS(casinh)
WCRT_COMPLEX_WRAPPERS(catanh)
WCRT_COMPLEX_WRAPPERS(ccosh)
WCRT_COMPLEX_WRAPPERS(csinh)
WCRT_COMPLEX_WRAPPERS(ctanh)
WCRT_COMPLEX_WRAPPERS(cexp)
WCRT_COMPLEX_WRAPPERS(clog)
WCRT_COMPLEX_WRAPPERS(csqrt)

float complex cpowf(float complex base, float complex power)
{
    double complex result = cpow((double complex)base,
        (double complex)power);
    return wcrt_make_complexf((float)wcrt_real(result),
        (float)wcrt_imag(result));
}

long double complex cpowl(long double complex base,
    long double complex power)
{
    double complex result = cpow((double complex)base,
        (double complex)power);
    return wcrt_make_complexl((long double)wcrt_real(result),
        (long double)wcrt_imag(result));
}
