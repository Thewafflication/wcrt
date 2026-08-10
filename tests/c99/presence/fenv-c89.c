/**
 * @file fenv-c89.c
 * @brief Verifies that the C99 floating environment additions are absent in C89 mode.
 */
#include <fenv.h>

#if defined(FE_TONEAREST) || defined(FE_UPWARD) || defined(FE_ALL_EXCEPT) || \
    defined(FE_DFL_ENV)
#error C99 floating-environment macros must not be exposed in C89 mode
#endif
