/**
 * @file fenv.c
 * @brief Exercises the C99 floating environment API.
 */

#include <fenv.h>
#include <stdio.h>

int main(void)
{
    fenv_t saved;
    fenv_t current;
    fexcept_t flags = 0;

    if (fegetenv(&saved) != 0) return 1;
    if (fesetround(FE_TOWARDZERO) != 0) return 2;
    if (fegetround() != FE_TOWARDZERO) return 3;
    if (feraiseexcept(FE_INVALID | FE_OVERFLOW) != 0) return 4;
    if (fetestexcept(FE_INVALID | FE_OVERFLOW) == 0) return 5;
    if (fegetexceptflag(&flags, FE_INVALID | FE_OVERFLOW) != 0) return 6;
    if (flags == 0) return 7;
    if (feclearexcept(FE_INVALID | FE_OVERFLOW) != 0) return 8;
    if (fetestexcept(FE_INVALID | FE_OVERFLOW) != 0) return 9;
    if (fesetenv(&saved) != 0) return 10;
    if (fegetenv(&current) != 0) return 11;
    if (current.control != saved.control) return 12;
    return 0;
}
