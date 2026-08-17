/**
 * @file tgmath.c
 * @brief Compiles the complete C99 type-generic mathematics inventory.
 */

#include <tgmath.h>

#if !defined(acos) || !defined(carg) || !defined(fma) || \
    !defined(nexttoward) || !defined(scalbn)
#error C99 type-generic macro inventory is incomplete
#endif
#if defined(modf) || defined(nan)
#error modf and nan must not be type-generic macros
#endif

void wcrt_tgmath_presence(float f, double d, long double l,
    float complex fc, double complex dc, long double complex lc, int *ip)
{
    (void)acos(f); (void)asin(d); (void)atan(l);
    (void)acosh(fc); (void)asinh(dc); (void)atanh(lc);
    (void)cos(f); (void)sin(d); (void)tan(l);
    (void)cosh(fc); (void)sinh(dc); (void)tanh(lc);
    (void)exp(fc); (void)log(dc); (void)sqrt(lc);
    (void)fabs(fc); (void)pow(dc, fc);

    (void)atan2(f, d); (void)cbrt(l); (void)ceil(f);
    (void)copysign(d, f); (void)erf(l); (void)erfc(f);
    (void)exp2(d); (void)expm1(l); (void)fdim(f, d);
    (void)floor(l); (void)fma(f, d, l); (void)fmax(f, d);
    (void)fmin(d, l); (void)fmod(f, d); (void)frexp(l, ip);
    (void)hypot(f, d); (void)ilogb(l); (void)ldexp(f, *ip);
    (void)lgamma(d); (void)llrint(l); (void)llround(f);
    (void)log10(d); (void)log1p(l); (void)log2(f);
    (void)logb(d); (void)lrint(l); (void)lround(f);
    (void)nearbyint(d); (void)nextafter(f, d);
    (void)nexttoward(f, l); (void)remainder(d, l);
    (void)remquo(f, d, ip); (void)rint(l); (void)round(f);
    (void)scalbln(d, 2L); (void)scalbn(l, 2);
    (void)tgamma(f); (void)trunc(d);

    (void)carg(fc); (void)cimag(dc); (void)conj(lc);
    (void)cproj(fc); (void)creal(dc);
}
