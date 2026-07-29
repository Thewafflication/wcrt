/**
 * @file fileno.c
 * @brief Verifies the Microsoft _fileno compatibility declaration.
 */

#include <stdio.h>

typedef int (*wcrt_ms_fileno_fn)(FILE *);

/** Forces type checking of the public declaration. */
static wcrt_ms_fileno_fn wcrt_ms_fileno_address = (_fileno);

