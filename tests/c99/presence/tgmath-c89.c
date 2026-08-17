/**
 * @file tgmath-c89.c
 * @brief Verifies that type-generic macros are absent in selected C89 mode.
 */

#include <tgmath.h>

#if defined(acos) || defined(carg) || defined(fma) || defined(scalbn)
#error Type-generic macros must not be exposed in C89 mode
#endif
