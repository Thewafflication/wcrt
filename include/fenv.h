/**
 * @file fenv.h
 * @brief Minimal C99 floating environment state for the WCRT TinyCC model.
 */

#ifndef WCRT_FENV_H
#define WCRT_FENV_H

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L

typedef struct {
    unsigned int control;
} fenv_t;

typedef unsigned int fexcept_t;

#define FE_TONEAREST 0x0000u
#define FE_DOWNWARD 0x0001u
#define FE_UPWARD 0x0002u
#define FE_TOWARDZERO 0x0003u

#define FE_DIVBYZERO 0x0004u
#define FE_INEXACT 0x0008u
#define FE_INVALID 0x0010u
#define FE_OVERFLOW 0x0020u
#define FE_UNDERFLOW 0x0040u
#define FE_ALL_EXCEPT (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

#define FE_DFL_ENV ((const fenv_t *)0)

int feclearexcept(int excepts);
int fegetexceptflag(fexcept_t *flagp, int excepts);
int feraiseexcept(int excepts);
int fesetexceptflag(const fexcept_t *flagp, int excepts);
int fetestexcept(int excepts);
int fegetround(void);
int fesetround(int round);
int fegetenv(fenv_t *envp);
int fesetenv(const fenv_t *envp);
int feupdateenv(const fenv_t *envp);

#endif

#endif
