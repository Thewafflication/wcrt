/**
 * @file stdarg.c
 * @brief Verifies C99 va_copy and restrict-qualified declarations.
 */
#include <stdarg.h>
#include <stdio.h>

#ifndef va_copy
#error va_copy is required in C99 mode
#endif

typedef int (*wcrt_snprintf_signature)(char * restrict, size_t,
    const char * restrict, ...);

static wcrt_snprintf_signature wcrt_snprintf_address = (snprintf);
