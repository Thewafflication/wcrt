/**
 * @file fenv.h
 * @brief Declares the C99 floating-point environment supported by WCRT.
 */

#ifndef WCRT_FENV_H
#define WCRT_FENV_H

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L

/** @brief Normalized floating-point environment with a stable Windows ABI. */
typedef struct {
    unsigned int __control;
    unsigned int __status;
    unsigned int __reserved[2];
} fenv_t;

/** @brief Object representation of the supported exception flags. */
typedef unsigned int fexcept_t;

/** @name Supported exception flags */
/** @{ */
#define FE_INVALID 0x0001
#define FE_DIVBYZERO 0x0004
#define FE_OVERFLOW 0x0008
#define FE_UNDERFLOW 0x0010
#define FE_INEXACT 0x0020
#define FE_ALL_EXCEPT (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | \
    FE_UNDERFLOW | FE_INEXACT)
/** @} */

/** @name Supported rounding directions */
/** @{ */
#define FE_TONEAREST 0x0000
#define FE_DOWNWARD 0x0400
#define FE_UPWARD 0x0800
#define FE_TOWARDZERO 0x0c00
/** @} */

/** @brief Denotes WCRT's startup floating-point environment. */
#define FE_DFL_ENV ((const fenv_t *)-1)

#ifdef __cplusplus
extern "C" {
#endif

int feclearexcept(int excepts);
int fegetexceptflag(fexcept_t *flagp, int excepts);
int feraiseexcept(int excepts);
int fesetexceptflag(const fexcept_t *flagp, int excepts);
int fetestexcept(int excepts);
int fegetround(void);
int fesetround(int round);
int fegetenv(fenv_t *envp);
int feholdexcept(fenv_t *envp);
int fesetenv(const fenv_t *envp);
int feupdateenv(const fenv_t *envp);

#ifdef __cplusplus
}
#endif

#endif

#endif
