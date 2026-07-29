/**
 * @file io.c
 * @brief Verifies the minimal Microsoft-compatible io.h header.
 */

#include <io.h>
#include <io.h>

#include <stdio.h>

/** Confirms that repeated inclusion leaves a valid translation unit. */
static FILE *wcrt_io_header_stream;

