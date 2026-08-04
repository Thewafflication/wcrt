/** @file string-case.c @brief Checks Microsoft comparison declarations. */
#include <string.h>
/** @brief Type of the unbounded comparison function. */
typedef int (*cmp_fn)(const char *, const char *);
/** @brief Type of the bounded comparison function. */
typedef int (*ncmp_fn)(const char *, const char *, size_t);
/** @brief Forces type checking of _stricmp. */
static cmp_fn cmp_address = _stricmp;
/** @brief Forces type checking of _strnicmp. */
static ncmp_fn ncmp_address = _strnicmp;
