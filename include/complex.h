/**
 * @file complex.h
 * @brief Declares the C99 complex-arithmetic library used by WCRT.
 */

#ifndef WCRT_COMPLEX_H
#define WCRT_COMPLEX_H

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L

/** @brief Makes the compiler-owned complex type available as `complex`. */
#define complex _Complex
/** @brief The required imaginary unit with type `const float complex`. */
#define _Complex_I (1.0fi)
/** @brief The imaginary unit selected from the available complex model. */
#define I _Complex_I

#ifdef __cplusplus
extern "C" {
#endif

#define __WCRT_COMPLEX_UNARY(name) \
    double complex name(double complex value); \
    float complex name##f(float complex value); \
    long double complex name##l(long double complex value)

__WCRT_COMPLEX_UNARY(cacos);
__WCRT_COMPLEX_UNARY(casin);
__WCRT_COMPLEX_UNARY(catan);
__WCRT_COMPLEX_UNARY(ccos);
__WCRT_COMPLEX_UNARY(csin);
__WCRT_COMPLEX_UNARY(ctan);
__WCRT_COMPLEX_UNARY(cacosh);
__WCRT_COMPLEX_UNARY(casinh);
__WCRT_COMPLEX_UNARY(catanh);
__WCRT_COMPLEX_UNARY(ccosh);
__WCRT_COMPLEX_UNARY(csinh);
__WCRT_COMPLEX_UNARY(ctanh);
__WCRT_COMPLEX_UNARY(cexp);
__WCRT_COMPLEX_UNARY(clog);
__WCRT_COMPLEX_UNARY(csqrt);
__WCRT_COMPLEX_UNARY(conj);
__WCRT_COMPLEX_UNARY(cproj);

#undef __WCRT_COMPLEX_UNARY

double complex cpow(double complex base, double complex power);
float complex cpowf(float complex base, float complex power);
long double complex cpowl(long double complex base,
    long double complex power);

double cabs(double complex value);
float cabsf(float complex value);
long double cabsl(long double complex value);
double carg(double complex value);
float cargf(float complex value);
long double cargl(long double complex value);
double cimag(double complex value);
float cimagf(float complex value);
long double cimagl(long double complex value);
double creal(double complex value);
float crealf(float complex value);
long double creall(long double complex value);

#ifdef __cplusplus
}
#endif

#endif

#endif
