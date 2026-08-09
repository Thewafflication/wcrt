/**
 * @file stdint-c89.c
 * @brief Verifies that the C99 integer interface is absent in C89 mode.
 */
#include <stdint.h>

#if defined(INT8_MAX) || defined(INTPTR_MAX) || defined(INTMAX_MAX)
#error C99 signed integer limits must not be exposed in C89 mode
#endif
#if defined(UINT8_MAX) || defined(UINTPTR_MAX) || defined(UINTMAX_MAX)
#error C99 unsigned integer limits must not be exposed in C89 mode
#endif
#if defined(INT8_C) || defined(UINT8_C) || defined(INTMAX_C)
#error C99 integer constant macros must not be exposed in C89 mode
#endif
#if defined(WCHAR_MIN) || defined(WCHAR_MAX) || defined(WINT_MIN) || \
    defined(WINT_MAX)
#error C99 wide-character limits must not be exposed in C89 mode
#endif

static int int8_t;
static int intptr_t;
static int intmax_t;
