/**
 * @file complex-c89.c
 * @brief Verifies that complex facilities are absent in selected C89 mode.
 */

#include <complex.h>

#if defined(complex) || defined(_Complex_I) || defined(I)
#error C99 complex macros must not be exposed in C89 mode
#endif

int complex;
