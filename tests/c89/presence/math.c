/* REQ-0007: <math.h> presence. Compile only; do not link. */
#include <math.h>

#ifndef HUGE_VAL
#error "HUGE_VAL must be defined"
#endif

typedef double (*wcrt_math_unary_fn)(double);
typedef double (*wcrt_math_binary_fn)(double, double);
typedef double (*wcrt_math_int_pointer_fn)(double, int *);
typedef double (*wcrt_math_int_fn)(double, int);
typedef double (*wcrt_math_double_pointer_fn)(double, double *);

static wcrt_math_unary_fn wcrt_math_unary_functions[] = {
    (acos), (asin), (atan), (cos), (sin), (tan),
    (cosh), (sinh), (tanh), (exp), (log), (log10),
    (sqrt), (ceil), (fabs), (floor)
};
static wcrt_math_binary_fn wcrt_math_binary_functions[] = {
    (atan2), (pow), (fmod)
};
static wcrt_math_int_pointer_fn wcrt_frexp_address = (frexp);
static wcrt_math_int_fn wcrt_ldexp_address = (ldexp);
static wcrt_math_double_pointer_fn wcrt_modf_address = (modf);
static double wcrt_huge_val = HUGE_VAL;
