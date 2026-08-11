/**
 * @file fenv.c
 * @brief Exercises flags, rounding, and environment transitions in C99 fenv.
 */

#include <fenv.h>
#include <float.h>

struct wcrt_fenv_alignment {
    char prefix;
    fenv_t value;
};

#define WCRT_OFFSET(type, member) ((unsigned long)&(((type *)0)->member))

static int check_rounding(void)
{
    volatile double one = 1.0;
    volatile double half_ulp = DBL_EPSILON * 0.5;
    volatile double negative_one = -1.0;
    double result;

    if (fesetround(FE_TONEAREST) != 0) return 1;
    result = one + half_ulp;
    if (result != 1.0) return 2;
    if (fesetround(FE_UPWARD) != 0) return 3;
    result = one + half_ulp;
    if (result <= 1.0) return 4;
    if (fesetround(FE_DOWNWARD) != 0) return 5;
    result = negative_one - half_ulp;
    if (result >= -1.0) return 6;
    if (fesetround(FE_TOWARDZERO) != 0) return 7;
    result = one + half_ulp;
    if (result != 1.0) return 8;
    return 0;
}

static int check_hardware_flags(void)
{
    volatile double zero = 0.0;
    volatile double one = 1.0;
    volatile double maximum = DBL_MAX;
    volatile double minimum = DBL_MIN;
    volatile double result;

    if (feclearexcept(FE_ALL_EXCEPT) != 0) return 1;
    result = one / zero;
    (void)result;
    if ((fetestexcept(FE_DIVBYZERO) & FE_DIVBYZERO) == 0) return 2;
    if (feclearexcept(FE_ALL_EXCEPT) != 0) return 3;
    result = zero / zero;
    (void)result;
    if ((fetestexcept(FE_INVALID) & FE_INVALID) == 0) return 4;
    if (feclearexcept(FE_ALL_EXCEPT) != 0) return 5;
    result = maximum * maximum;
    (void)result;
    if ((fetestexcept(FE_OVERFLOW) & FE_OVERFLOW) == 0) return 6;
    if (feclearexcept(FE_ALL_EXCEPT) != 0) return 7;
    result = minimum * minimum;
    (void)result;
    if ((fetestexcept(FE_UNDERFLOW) & FE_UNDERFLOW) == 0) return 8;
    if (feclearexcept(FE_ALL_EXCEPT) != 0) return 9;
    result = one / 10.0;
    (void)result;
    if ((fetestexcept(FE_INEXACT) & FE_INEXACT) == 0) return 10;
    return 0;
}

int main(void)
{
    fenv_t saved;
    fenv_t alternate;
    fexcept_t flags;
    int prior_round;
    int check;

    if (sizeof(fenv_t) != 16U || sizeof(fexcept_t) != 4U) return 1;
    if (WCRT_OFFSET(struct wcrt_fenv_alignment, value) != 4U) return 2;
    if (fesetenv(FE_DFL_ENV) != 0) return 3;
    if (fegetround() != FE_TONEAREST) return 4;
    if (fetestexcept(FE_ALL_EXCEPT) != 0) return 5;

    prior_round = fegetround();
    if (fesetround(0x7fff) == 0 || fegetround() != prior_round) return 6;
    check = check_rounding();
    if (check != 0) return 10 + check;

    if (feclearexcept(FE_ALL_EXCEPT) != 0) return 20;
    if (feraiseexcept(FE_INVALID | FE_OVERFLOW) != 0) return 21;
    if ((fetestexcept(FE_ALL_EXCEPT) & (FE_INVALID | FE_OVERFLOW)) !=
        (FE_INVALID | FE_OVERFLOW)) return 22;
    if (fegetexceptflag(&flags, FE_INVALID | FE_OVERFLOW) != 0) return 23;
    if ((flags & (FE_INVALID | FE_OVERFLOW)) !=
        (FE_INVALID | FE_OVERFLOW)) return 24;
    if (feclearexcept(FE_INVALID) != 0) return 25;
    if (fetestexcept(FE_INVALID) != 0 ||
        fetestexcept(FE_OVERFLOW) == 0) return 26;
    flags = FE_DIVBYZERO | FE_INEXACT;
    if (fesetexceptflag(&flags, FE_DIVBYZERO | FE_INEXACT) != 0) return 27;
    if ((fetestexcept(FE_ALL_EXCEPT) &
        (FE_DIVBYZERO | FE_INEXACT | FE_OVERFLOW)) !=
        (FE_DIVBYZERO | FE_INEXACT | FE_OVERFLOW)) return 28;

    if (fegetenv(&saved) != 0) return 30;
    if (fesetround(FE_DOWNWARD) != 0 ||
        feclearexcept(FE_ALL_EXCEPT) != 0) return 31;
    if (fesetenv(&saved) != 0) return 32;
    if (fegetround() != saved.__control ||
        fetestexcept(FE_ALL_EXCEPT) != (int)saved.__status) return 33;

    if (feholdexcept(&alternate) != 0) return 34;
    if (fetestexcept(FE_ALL_EXCEPT) != 0) return 35;
    if (feraiseexcept(FE_UNDERFLOW) != 0) return 36;
    if (feupdateenv(&alternate) != 0) return 37;
    if (fegetround() != alternate.__control ||
        (fetestexcept(FE_UNDERFLOW) & FE_UNDERFLOW) == 0) return 38;

    if (fesetenv(FE_DFL_ENV) != 0) return 40;
    check = check_hardware_flags();
    if (check != 0) return 40 + check;
    if (fesetenv(FE_DFL_ENV) != 0) return 60;
    return 0;
}
