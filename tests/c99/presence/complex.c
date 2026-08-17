/**
 * @file complex.c
 * @brief Compiles the complete C99 complex-mathematics inventory.
 */

#include <complex.h>

#if !defined(complex) || !defined(_Complex_I) || !defined(I)
#error C99 complex type and imaginary-unit macros are required
#endif

#define WCRT_TAKE_COMPLEX(name) do { \
    unary_d = name; unary_f = name##f; unary_l = name##l; \
} while (0)

void wcrt_complex_presence(void)
{
    double complex (*unary_d)(double complex);
    float complex (*unary_f)(float complex);
    long double complex (*unary_l)(long double complex);
    double complex (*power_d)(double complex, double complex) = cpow;
    float complex (*power_f)(float complex, float complex) = cpowf;
    long double complex (*power_l)(long double complex,
        long double complex) = cpowl;
    double (*real_d)(double complex);
    float (*real_f)(float complex);
    long double (*real_l)(long double complex);

    WCRT_TAKE_COMPLEX(cacos); WCRT_TAKE_COMPLEX(casin);
    WCRT_TAKE_COMPLEX(catan); WCRT_TAKE_COMPLEX(ccos);
    WCRT_TAKE_COMPLEX(csin); WCRT_TAKE_COMPLEX(ctan);
    WCRT_TAKE_COMPLEX(cacosh); WCRT_TAKE_COMPLEX(casinh);
    WCRT_TAKE_COMPLEX(catanh); WCRT_TAKE_COMPLEX(ccosh);
    WCRT_TAKE_COMPLEX(csinh); WCRT_TAKE_COMPLEX(ctanh);
    WCRT_TAKE_COMPLEX(cexp); WCRT_TAKE_COMPLEX(clog);
    WCRT_TAKE_COMPLEX(csqrt); WCRT_TAKE_COMPLEX(conj);
    WCRT_TAKE_COMPLEX(cproj);

    real_d = cabs; real_f = cabsf; real_l = cabsl;
    real_d = carg; real_f = cargf; real_l = cargl;
    real_d = cimag; real_f = cimagf; real_l = cimagl;
    real_d = creal; real_f = crealf; real_l = creall;

    (void)unary_d; (void)unary_f; (void)unary_l;
    (void)power_d; (void)power_f; (void)power_l;
    (void)real_d; (void)real_f; (void)real_l;
}
