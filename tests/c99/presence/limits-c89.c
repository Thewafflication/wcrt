/**
 * @file limits-c89.c
 * @brief Verifies that C99 long-long limits are absent in C89 mode.
 */
#include <limits.h>

#if defined(LLONG_MIN) || defined(LLONG_MAX) || defined(ULLONG_MAX)
#error C99 long-long limits must not be exposed in C89 mode
#endif

static int wcrt_c89_limits_present = INT_MAX;

