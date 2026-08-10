/**
 * @file fenv.c
 * @brief Presence tests for the supported C99 floating environment API.
 */

#include <fenv.h>

#ifndef FE_TONEAREST
#error FE_TONEAREST must be defined
#endif

#ifndef FE_UPWARD
#error FE_UPWARD must be defined
#endif

#ifndef FE_ALL_EXCEPT
#error FE_ALL_EXCEPT must be defined
#endif

int main(void)
{
    fenv_t wcrt_env;
    fexcept_t wcrt_flags;
    int wcrt_rounding = FE_TONEAREST;
    int wcrt_test = fegetround();

    wcrt_env = *FE_DFL_ENV;
    if (fegetenv(&wcrt_env) != 0) return 1;
    if (fesetround(FE_TOWARDZERO) != 0) return 2;
    if (fegetround() != FE_TOWARDZERO) return 3;
    if (fegetexceptflag(&wcrt_flags, FE_INVALID) != 0) return 4;
    if (feraiseexcept(FE_INVALID) != 0) return 5;
    if (fetestexcept(FE_INVALID) == 0) return 6;
    if (feclearexcept(FE_INVALID) != 0) return 7;
    if (fetestexcept(FE_INVALID) != 0) return 8;
    if (fesetenv(FE_DFL_ENV) != 0) return 9;
    if (fegetround() != FE_TONEAREST) return 10;
    return wcrt_test == wcrt_rounding ? 0 : 11;
}
