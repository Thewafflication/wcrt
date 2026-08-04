/** @file secure-string.c @brief Checks secure string declarations. */
#include <string.h>
/** @brief Type of strcpy_s. */
typedef errno_t (*copy_fn)(char *, rsize_t, const char *);
/** @brief Type of strncpy_s. */
typedef errno_t (*ncopy_fn)(char *, rsize_t, const char *, rsize_t);
/** @brief Forces type checking of strcpy_s. */
static copy_fn copy_address = strcpy_s;
/** @brief Forces type checking of strncpy_s. */
static ncopy_fn ncopy_address = strncpy_s;
/** @brief Forces type checking of _TRUNCATE. */
static rsize_t truncate_value = _TRUNCATE;
