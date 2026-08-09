/**
 * @file stdio-t2.c
 * @brief Verifies the compile-time C99 formatted-input interface.
 */

#include <stdarg.h>
#include <stdio.h>

typedef int (*wcrt_vfscanf_fn)(FILE * restrict,
    const char * restrict, va_list);
typedef int (*wcrt_vscanf_fn)(const char * restrict, va_list);
typedef int (*wcrt_vsscanf_fn)(const char * restrict,
    const char * restrict, va_list);

static wcrt_vfscanf_fn wcrt_vfscanf_address = (vfscanf);
static wcrt_vscanf_fn wcrt_vscanf_address = (vscanf);
static wcrt_vsscanf_fn wcrt_vsscanf_address = (vsscanf);
