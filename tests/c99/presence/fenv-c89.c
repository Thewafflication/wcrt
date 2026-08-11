/**
 * @file fenv-c89.c
 * @brief Verifies C99 floating environment additions are absent in C89 mode.
 */
#include <fenv.h>

#if defined(FE_TONEAREST) || defined(FE_DOWNWARD) || defined(FE_UPWARD) || \
    defined(FE_TOWARDZERO) || defined(FE_ALL_EXCEPT) || \
    defined(FE_INVALID) || defined(FE_DFL_ENV)
#error C99 floating-environment macros must not be exposed in C89 mode
#endif
