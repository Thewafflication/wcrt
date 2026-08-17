/**
 * @file tgmath.h
 * @brief Defines the C99 type-generic mathematics macros used by WCRT.
 */

#ifndef WCRT_TGMATH_H
#define WCRT_TGMATH_H

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L

#include <math.h>
#include <complex.h>

#define __WCRT_GENERIC_UNARY(x, rf, rd, rl, cf, cd, cl) \
    _Generic((x), \
        float: rf, \
        long double: rl, \
        float complex: cf, \
        double complex: cd, \
        long double complex: cl, \
        default: rd)(x)

#define __WCRT_GENERIC_REAL_UNARY(x, ff, fd, fl) \
    _Generic((x), float: ff, long double: fl, default: fd)(x)

#define __WCRT_REAL_BINARY_FROM_FLOAT(y, ff, fd, fl) \
    _Generic((y), float: ff, long double: fl, default: fd)
#define __WCRT_REAL_BINARY_FROM_DEFAULT(y, fd, fl) \
    _Generic((y), long double: fl, default: fd)
#define __WCRT_SELECT_REAL_BINARY(x, y, ff, fd, fl) \
    _Generic((x), \
        float: __WCRT_REAL_BINARY_FROM_FLOAT(y, ff, fd, fl), \
        long double: fl, \
        default: __WCRT_REAL_BINARY_FROM_DEFAULT(y, fd, fl))
#define __WCRT_GENERIC_REAL_BINARY(x, y, ff, fd, fl) \
    __WCRT_SELECT_REAL_BINARY(x, y, ff, fd, fl)(x, y)

#define __WCRT_REAL_TERNARY_FROM_FLOAT_FLOAT(z, ff, fd, fl) \
    _Generic((z), float: ff, long double: fl, default: fd)
#define __WCRT_REAL_TERNARY_FROM_DEFAULT(z, fd, fl) \
    _Generic((z), long double: fl, default: fd)
#define __WCRT_REAL_TERNARY_FROM_FLOAT(y, z, ff, fd, fl) \
    _Generic((y), \
        float: __WCRT_REAL_TERNARY_FROM_FLOAT_FLOAT(z, ff, fd, fl), \
        long double: fl, \
        default: __WCRT_REAL_TERNARY_FROM_DEFAULT(z, fd, fl))
#define __WCRT_REAL_TERNARY_FROM_DEFAULT_DEFAULT(y, z, fd, fl) \
    _Generic((y), \
        long double: fl, \
        default: __WCRT_REAL_TERNARY_FROM_DEFAULT(z, fd, fl))
#define __WCRT_SELECT_REAL_TERNARY(x, y, z, ff, fd, fl) \
    _Generic((x), \
        float: __WCRT_REAL_TERNARY_FROM_FLOAT(y, z, ff, fd, fl), \
        long double: fl, \
        default: __WCRT_REAL_TERNARY_FROM_DEFAULT_DEFAULT(y, z, fd, fl))
#define __WCRT_GENERIC_REAL_TERNARY(x, y, z, ff, fd, fl) \
    __WCRT_SELECT_REAL_TERNARY(x, y, z, ff, fd, fl)(x, y, z)

#define __WCRT_GENERIC_COMPLEX_UNARY(x, ff, fd, fl) \
    _Generic((x), \
        float: ff, \
        long double: fl, \
        float complex: ff, \
        long double complex: fl, \
        default: fd)(x)

#define acos(x) __WCRT_GENERIC_UNARY(x, acosf, acos, acosl, \
    cacosf, cacos, cacosl)
#define asin(x) __WCRT_GENERIC_UNARY(x, asinf, asin, asinl, \
    casinf, casin, casinl)
#define atan(x) __WCRT_GENERIC_UNARY(x, atanf, atan, atanl, \
    catanf, catan, catanl)
#define acosh(x) __WCRT_GENERIC_UNARY(x, acoshf, acosh, acoshl, \
    cacoshf, cacosh, cacoshl)
#define asinh(x) __WCRT_GENERIC_UNARY(x, asinhf, asinh, asinhl, \
    casinhf, casinh, casinhl)
#define atanh(x) __WCRT_GENERIC_UNARY(x, atanhf, atanh, atanhl, \
    catanhf, catanh, catanhl)
#define cos(x) __WCRT_GENERIC_UNARY(x, cosf, cos, cosl, \
    ccosf, ccos, ccosl)
#define sin(x) __WCRT_GENERIC_UNARY(x, sinf, sin, sinl, \
    csinf, csin, csinl)
#define tan(x) __WCRT_GENERIC_UNARY(x, tanf, tan, tanl, \
    ctanf, ctan, ctanl)
#define cosh(x) __WCRT_GENERIC_UNARY(x, coshf, cosh, coshl, \
    ccoshf, ccosh, ccoshl)
#define sinh(x) __WCRT_GENERIC_UNARY(x, sinhf, sinh, sinhl, \
    csinhf, csinh, csinhl)
#define tanh(x) __WCRT_GENERIC_UNARY(x, tanhf, tanh, tanhl, \
    ctanhf, ctanh, ctanhl)
#define exp(x) __WCRT_GENERIC_UNARY(x, expf, exp, expl, \
    cexpf, cexp, cexpl)
#define log(x) __WCRT_GENERIC_UNARY(x, logf, log, logl, \
    clogf, clog, clogl)
#define sqrt(x) __WCRT_GENERIC_UNARY(x, sqrtf, sqrt, sqrtl, \
    csqrtf, csqrt, csqrtl)
#define fabs(x) __WCRT_GENERIC_UNARY(x, fabsf, fabs, fabsl, \
    cabsf, cabs, cabsl)

#define __WCRT_POW_FROM_FLOAT(y) \
    _Generic((y), \
        float: powf, long double: powl, \
        float complex: cpowf, double complex: cpow, \
        long double complex: cpowl, default: pow)
#define __WCRT_POW_FROM_DEFAULT(y) \
    _Generic((y), \
        long double: powl, \
        float complex: cpow, double complex: cpow, \
        long double complex: cpowl, default: pow)
#define __WCRT_POW_FROM_LONG(y) \
    _Generic((y), \
        float complex: cpowl, double complex: cpowl, \
        long double complex: cpowl, default: powl)
#define __WCRT_POW_FROM_FLOAT_COMPLEX(y) \
    _Generic((y), \
        float: cpowf, long double: cpowl, \
        float complex: cpowf, double complex: cpow, \
        long double complex: cpowl, default: cpow)
#define __WCRT_POW_FROM_DOUBLE_COMPLEX(y) \
    _Generic((y), \
        long double: cpowl, long double complex: cpowl, default: cpow)
#define __WCRT_SELECT_POW(x, y) \
    _Generic((x), \
        float: __WCRT_POW_FROM_FLOAT(y), \
        long double: __WCRT_POW_FROM_LONG(y), \
        float complex: __WCRT_POW_FROM_FLOAT_COMPLEX(y), \
        double complex: __WCRT_POW_FROM_DOUBLE_COMPLEX(y), \
        long double complex: cpowl, \
        default: __WCRT_POW_FROM_DEFAULT(y))
#define pow(x, y) __WCRT_SELECT_POW(x, y)(x, y)

#define atan2(x, y) __WCRT_GENERIC_REAL_BINARY(x, y, atan2f, atan2, atan2l)
#define cbrt(x) __WCRT_GENERIC_REAL_UNARY(x, cbrtf, cbrt, cbrtl)
#define ceil(x) __WCRT_GENERIC_REAL_UNARY(x, ceilf, ceil, ceill)
#define copysign(x, y) \
    __WCRT_GENERIC_REAL_BINARY(x, y, copysignf, copysign, copysignl)
#define erf(x) __WCRT_GENERIC_REAL_UNARY(x, erff, erf, erfl)
#define erfc(x) __WCRT_GENERIC_REAL_UNARY(x, erfcf, erfc, erfcl)
#define exp2(x) __WCRT_GENERIC_REAL_UNARY(x, exp2f, exp2, exp2l)
#define expm1(x) __WCRT_GENERIC_REAL_UNARY(x, expm1f, expm1, expm1l)
#define fdim(x, y) __WCRT_GENERIC_REAL_BINARY(x, y, fdimf, fdim, fdiml)
#define floor(x) __WCRT_GENERIC_REAL_UNARY(x, floorf, floor, floorl)
#define fma(x, y, z) \
    __WCRT_GENERIC_REAL_TERNARY(x, y, z, fmaf, fma, fmal)
#define fmax(x, y) __WCRT_GENERIC_REAL_BINARY(x, y, fmaxf, fmax, fmaxl)
#define fmin(x, y) __WCRT_GENERIC_REAL_BINARY(x, y, fminf, fmin, fminl)
#define fmod(x, y) __WCRT_GENERIC_REAL_BINARY(x, y, fmodf, fmod, fmodl)
#define hypot(x, y) __WCRT_GENERIC_REAL_BINARY(x, y, hypotf, hypot, hypotl)
#define lgamma(x) __WCRT_GENERIC_REAL_UNARY(x, lgammaf, lgamma, lgammal)
#define log10(x) __WCRT_GENERIC_REAL_UNARY(x, log10f, log10, log10l)
#define log1p(x) __WCRT_GENERIC_REAL_UNARY(x, log1pf, log1p, log1pl)
#define log2(x) __WCRT_GENERIC_REAL_UNARY(x, log2f, log2, log2l)
#define logb(x) __WCRT_GENERIC_REAL_UNARY(x, logbf, logb, logbl)
#define nearbyint(x) \
    __WCRT_GENERIC_REAL_UNARY(x, nearbyintf, nearbyint, nearbyintl)
#define nextafter(x, y) \
    __WCRT_GENERIC_REAL_BINARY(x, y, nextafterf, nextafter, nextafterl)
#define remainder(x, y) \
    __WCRT_GENERIC_REAL_BINARY(x, y, remainderf, remainder, remainderl)
#define rint(x) __WCRT_GENERIC_REAL_UNARY(x, rintf, rint, rintl)
#define round(x) __WCRT_GENERIC_REAL_UNARY(x, roundf, round, roundl)
#define tgamma(x) __WCRT_GENERIC_REAL_UNARY(x, tgammaf, tgamma, tgammal)
#define trunc(x) __WCRT_GENERIC_REAL_UNARY(x, truncf, trunc, truncl)

#define frexp(x, p) \
    _Generic((x), float: frexpf, long double: frexpl, default: frexp)(x, p)
#define ilogb(x) \
    _Generic((x), float: ilogbf, long double: ilogbl, default: ilogb)(x)
#define ldexp(x, n) \
    _Generic((x), float: ldexpf, long double: ldexpl, default: ldexp)(x, n)
#define llrint(x) \
    _Generic((x), float: llrintf, long double: llrintl, default: llrint)(x)
#define llround(x) _Generic((x), float: llroundf, \
    long double: llroundl, default: llround)(x)
#define lrint(x) \
    _Generic((x), float: lrintf, long double: lrintl, default: lrint)(x)
#define lround(x) \
    _Generic((x), float: lroundf, long double: lroundl, default: lround)(x)
#define nexttoward(x, y) _Generic((x), float: nexttowardf, \
    long double: nexttowardl, default: nexttoward)(x, y)
#define remquo(x, y, p) \
    __WCRT_SELECT_REAL_BINARY(x, y, remquof, remquo, remquol)(x, y, p)
#define scalbln(x, n) _Generic((x), float: scalblnf, \
    long double: scalblnl, default: scalbln)(x, n)
#define scalbn(x, n) _Generic((x), float: scalbnf, \
    long double: scalbnl, default: scalbn)(x, n)

#define carg(x) __WCRT_GENERIC_COMPLEX_UNARY(x, cargf, carg, cargl)
#define cimag(x) __WCRT_GENERIC_COMPLEX_UNARY(x, cimagf, cimag, cimagl)
#define conj(x) __WCRT_GENERIC_COMPLEX_UNARY(x, conjf, conj, conjl)
#define cproj(x) __WCRT_GENERIC_COMPLEX_UNARY(x, cprojf, cproj, cprojl)
#define creal(x) __WCRT_GENERIC_COMPLEX_UNARY(x, crealf, creal, creall)

#endif

#endif
