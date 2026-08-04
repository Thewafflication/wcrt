/** @file wpm-snprintf.c @brief Checks the snprintf declaration. */
#include <stdio.h>
/** @brief Type of the standard bounded formatter. */
typedef int (*snprintf_fn)(char *, size_t, const char *, ...);
/** @brief Forces type checking of snprintf. */
static snprintf_fn snprintf_address = snprintf;
