/**
 * @file stdio.c
 * @brief Verifies Microsoft bounded-formatting compatibility declarations.
 */
#include <stdio.h>

typedef int (*wcrt_ms_snprintf_fn)(char *, size_t, const char *, ...);
typedef int (*wcrt_ms_vsnprintf_fn)(char *, size_t, const char *, va_list);

static wcrt_ms_snprintf_fn wcrt_ms_snprintf_address = (_snprintf);
static wcrt_ms_vsnprintf_fn wcrt_ms_vsnprintf_address = (_vsnprintf);
