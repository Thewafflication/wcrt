/**
 * @file stdbool-c89.c
 * @brief Verifies that the C99 Boolean macros are absent in C89 mode.
 */
#include <stdbool.h>

#if defined(bool) || defined(true) || defined(false)
#error C99 Boolean macros must not be exposed in C89 mode
#endif
#if defined(__bool_true_false_are_defined)
#error C99 Boolean marker must not be exposed in C89 mode
#endif

static int bool;

