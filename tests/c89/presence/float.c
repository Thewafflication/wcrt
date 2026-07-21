/* REQ-0004: <float.h> presence. Compile only; do not link. */
#include <float.h>

#ifndef FLT_RADIX
#error "FLT_RADIX must be defined"
#endif
#ifndef FLT_ROUNDS
#error "FLT_ROUNDS must be defined"
#endif
#ifndef FLT_DIG
#error "FLT_DIG must be defined"
#endif
#ifndef DBL_DIG
#error "DBL_DIG must be defined"
#endif
#ifndef LDBL_DIG
#error "LDBL_DIG must be defined"
#endif
#ifndef FLT_MANT_DIG
#error "FLT_MANT_DIG must be defined"
#endif
#ifndef DBL_MANT_DIG
#error "DBL_MANT_DIG must be defined"
#endif
#ifndef LDBL_MANT_DIG
#error "LDBL_MANT_DIG must be defined"
#endif
#ifndef FLT_MIN_EXP
#error "FLT_MIN_EXP must be defined"
#endif
#ifndef DBL_MIN_EXP
#error "DBL_MIN_EXP must be defined"
#endif
#ifndef LDBL_MIN_EXP
#error "LDBL_MIN_EXP must be defined"
#endif
#ifndef FLT_MAX_EXP
#error "FLT_MAX_EXP must be defined"
#endif
#ifndef DBL_MAX_EXP
#error "DBL_MAX_EXP must be defined"
#endif
#ifndef LDBL_MAX_EXP
#error "LDBL_MAX_EXP must be defined"
#endif
#ifndef FLT_MIN_10_EXP
#error "FLT_MIN_10_EXP must be defined"
#endif
#ifndef DBL_MIN_10_EXP
#error "DBL_MIN_10_EXP must be defined"
#endif
#ifndef LDBL_MIN_10_EXP
#error "LDBL_MIN_10_EXP must be defined"
#endif
#ifndef FLT_MAX_10_EXP
#error "FLT_MAX_10_EXP must be defined"
#endif
#ifndef DBL_MAX_10_EXP
#error "DBL_MAX_10_EXP must be defined"
#endif
#ifndef LDBL_MAX_10_EXP
#error "LDBL_MAX_10_EXP must be defined"
#endif
#ifndef FLT_MAX
#error "FLT_MAX must be defined"
#endif
#ifndef DBL_MAX
#error "DBL_MAX must be defined"
#endif
#ifndef LDBL_MAX
#error "LDBL_MAX must be defined"
#endif
#ifndef FLT_EPSILON
#error "FLT_EPSILON must be defined"
#endif
#ifndef DBL_EPSILON
#error "DBL_EPSILON must be defined"
#endif
#ifndef LDBL_EPSILON
#error "LDBL_EPSILON must be defined"
#endif
#ifndef FLT_MIN
#error "FLT_MIN must be defined"
#endif
#ifndef DBL_MIN
#error "DBL_MIN must be defined"
#endif
#ifndef LDBL_MIN
#error "LDBL_MIN must be defined"
#endif

static float wcrt_float_values[] = { FLT_MIN, FLT_MAX, FLT_EPSILON };
static double wcrt_double_values[] = { DBL_MIN, DBL_MAX, DBL_EPSILON };
static long double wcrt_long_double_values[] = {
    LDBL_MIN, LDBL_MAX, LDBL_EPSILON
};

enum {
    wcrt_float_integer_macros_present =
        FLT_RADIX + FLT_ROUNDS + FLT_DIG + DBL_DIG + LDBL_DIG +
        FLT_MANT_DIG + DBL_MANT_DIG + LDBL_MANT_DIG +
        FLT_MIN_EXP + DBL_MIN_EXP + LDBL_MIN_EXP +
        FLT_MAX_EXP + DBL_MAX_EXP + LDBL_MAX_EXP +
        FLT_MIN_10_EXP + DBL_MIN_10_EXP + LDBL_MIN_10_EXP +
        FLT_MAX_10_EXP + DBL_MAX_10_EXP + LDBL_MAX_10_EXP
};
