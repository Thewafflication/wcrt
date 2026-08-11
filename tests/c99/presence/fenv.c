/**
 * @file fenv.c
 * @brief Compiles the complete supported C99 floating-environment API.
 */

#include <fenv.h>

#if !defined(FE_INVALID) || !defined(FE_DIVBYZERO) || \
    !defined(FE_OVERFLOW) || !defined(FE_UNDERFLOW) || \
    !defined(FE_INEXACT) || !defined(FE_ALL_EXCEPT)
#error All five standard exception flags are required
#endif
#if !defined(FE_TONEAREST) || !defined(FE_DOWNWARD) || \
    !defined(FE_UPWARD) || !defined(FE_TOWARDZERO) || !defined(FE_DFL_ENV)
#error All four standard rounding modes and FE_DFL_ENV are required
#endif

int main(void)
{
    fenv_t environment;
    fexcept_t flags = 0U;
    int (*clear_function)(int) = feclearexcept;
    int (*get_flag_function)(fexcept_t *, int) = fegetexceptflag;
    int (*raise_function)(int) = feraiseexcept;
    int (*set_flag_function)(const fexcept_t *, int) = fesetexceptflag;
    int (*test_function)(int) = fetestexcept;
    int (*get_round_function)(void) = fegetround;
    int (*set_round_function)(int) = fesetround;
    int (*get_environment_function)(fenv_t *) = fegetenv;
    int (*hold_function)(fenv_t *) = feholdexcept;
    int (*set_environment_function)(const fenv_t *) = fesetenv;
    int (*update_function)(const fenv_t *) = feupdateenv;

    (void)environment; (void)flags; (void)clear_function;
    (void)get_flag_function; (void)raise_function; (void)set_flag_function;
    (void)test_function; (void)get_round_function; (void)set_round_function;
    (void)get_environment_function; (void)hold_function;
    (void)set_environment_function; (void)update_function;
    return 0;
}
