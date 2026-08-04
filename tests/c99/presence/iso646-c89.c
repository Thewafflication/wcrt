/**
 * @file iso646-c89.c
 * @brief Verifies that alternative operator macros are absent in C89 mode.
 */
#include <iso646.h>

#if defined(and) || defined(and_eq) || defined(bitand) || defined(bitor)
#error C95 and C99 alternative tokens must not be exposed in C89 mode
#endif
#if defined(compl) || defined(not) || defined(not_eq) || defined(or)
#error C95 and C99 alternative tokens must not be exposed in C89 mode
#endif
#if defined(or_eq) || defined(xor) || defined(xor_eq)
#error C95 and C99 alternative tokens must not be exposed in C89 mode
#endif

static int wcrt_c89_iso646_translation_unit;

