/** @file secure-stdio.c @brief Checks secure stdio declarations. */
#include <stdio.h>
/** @brief Type of fopen_s. */
typedef errno_t (*open_fn)(FILE **, const char *, const char *);
/** @brief Type of sscanf_s. */
typedef int (*scan_fn)(const char *, const char *, ...);
/** @brief Forces type checking of fopen_s. */
static open_fn open_address = fopen_s;
/** @brief Forces type checking of sscanf_s. */
static scan_fn scan_address = sscanf_s;
