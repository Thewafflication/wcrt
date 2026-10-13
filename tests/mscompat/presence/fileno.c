/**
 * @file fileno.c
 * @brief Verifies the Microsoft _fileno compatibility declaration.
 */

#include <stdio.h>
typedef FILE *(*wcrt_ms_fdopen_fn)(int, const char *);
/** Forces type checking from stdio.h alone before io.h is included. */
static wcrt_ms_fdopen_fn wcrt_ms_fdopen_address = (_fdopen);
#include <io.h>

typedef int (*wcrt_ms_fileno_fn)(FILE *);

/** Forces type checking of the public declaration. */
static wcrt_ms_fileno_fn wcrt_ms_fileno_address = (_fileno);

typedef __wcrt_intptr_t (*wcrt_ms_get_osfhandle_fn)(int);
/** Forces type checking of the native-handle accessor. */
static wcrt_ms_get_osfhandle_fn wcrt_ms_get_osfhandle_address =
    (_get_osfhandle);
