/**
 * @file fenv.c
 * @brief TinyCC-compatible C99 floating-point environment support.
 */

#include <fenv.h>
#include <stdlib.h>

static fenv_t wcrt_fenv_state = { FE_TONEAREST };

int feclearexcept(int excepts)
{
    if (excepts & FE_ALL_EXCEPT) {
        wcrt_fenv_state.control &= ~(excepts & FE_ALL_EXCEPT);
    }
    return 0;
}

int fegetexceptflag(fexcept_t *flagp, int excepts)
{
    if (flagp == 0) {
        return 1;
    }
    *flagp = (fexcept_t)(wcrt_fenv_state.control & (excepts & FE_ALL_EXCEPT));
    return 0;
}

int feraiseexcept(int excepts)
{
    if (excepts & FE_ALL_EXCEPT) {
        wcrt_fenv_state.control |= (excepts & FE_ALL_EXCEPT);
    }
    return 0;
}

int fesetexceptflag(const fexcept_t *flagp, int excepts)
{
    if (flagp == 0) {
        return 1;
    }
    wcrt_fenv_state.control = (wcrt_fenv_state.control & ~(excepts & FE_ALL_EXCEPT)) |
        ((unsigned int)(*flagp) & (excepts & FE_ALL_EXCEPT));
    return 0;
}

int fetestexcept(int excepts)
{
    return (int)(wcrt_fenv_state.control & (excepts & FE_ALL_EXCEPT));
}

int fegetround(void)
{
    return (int)(wcrt_fenv_state.control & 0x0003u);
}

int fesetround(int round)
{
    if (round != FE_TONEAREST && round != FE_DOWNWARD &&
        round != FE_UPWARD && round != FE_TOWARDZERO) {
        return 1;
    }
    wcrt_fenv_state.control = (wcrt_fenv_state.control & ~0x0003u) | (unsigned int)round;
    return 0;
}

int fegetenv(fenv_t *envp)
{
    if (envp == 0) {
        return 1;
    }
    *envp = wcrt_fenv_state;
    return 0;
}

int fesetenv(const fenv_t *envp)
{
    if (envp == 0) {
        return 1;
    }
    wcrt_fenv_state = *envp;
    return 0;
}

int feupdateenv(const fenv_t *envp)
{
    if (envp == 0) {
        return 1;
    }
    wcrt_fenv_state = *envp;
    return 0;
}
