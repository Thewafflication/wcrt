/**
 * @file limits.c
 * @brief Verifies the compile-time C99 long-long limits.
 */
#include <limits.h>

#ifndef LLONG_MIN
#error LLONG_MIN is required in C99 mode
#endif
#ifndef LLONG_MAX
#error LLONG_MAX is required in C99 mode
#endif
#ifndef ULLONG_MAX
#error ULLONG_MAX is required in C99 mode
#endif

typedef char wcrt_llong_range[LLONG_MIN == -LLONG_MAX - 1LL ? 1 : -1];
typedef char wcrt_ullong_range[ULLONG_MAX > LLONG_MAX ? 1 : -1];
