/**
 * @file errno.c
 * @brief Defines process-global storage for the C89 error indicator.
 */

#include <errno.h>

/** @brief Process-global error indicator used by the C89-only runtime. */
int wcrt_errno;
