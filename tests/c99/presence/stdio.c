/**
 * @file stdio.c
 * @brief Verifies the compile-time C99 bounded-formatting interface.
 */
#include <stdio.h>

typedef int (*wcrt_snprintf_fn)(char * restrict, size_t,
    const char * restrict, ...);
typedef int (*wcrt_vsnprintf_fn)(char * restrict, size_t,
    const char * restrict, va_list);

static wcrt_snprintf_fn wcrt_snprintf_address = (snprintf);
static wcrt_vsnprintf_fn wcrt_vsnprintf_address = (vsnprintf);
