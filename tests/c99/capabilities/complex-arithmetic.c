/**
 * @file complex-arithmetic.c
 * @brief Probes complex language and ABI support.
 */
typedef union {
    double _Complex value;
    double part[2];
} wcrt_complex_parts;

static float _Complex wcrt_float_value = 1.0F;
static double _Complex wcrt_double_value = 2.0;
static long double _Complex wcrt_long_value = 3.0L;

static double _Complex wcrt_complex_add(double _Complex left,
    double _Complex right)
{
    return left + right;
}

static double _Complex wcrt_complex_operations(double _Complex left,
    double _Complex right)
{
    wcrt_complex_parts parts;
    parts.value = left * right / (right + 1.0);
    return wcrt_complex_add(parts.value, wcrt_double_value) +
        wcrt_float_value + wcrt_long_value;
}

double _Complex (*wcrt_complex_abi)(double _Complex,
    double _Complex) = wcrt_complex_operations;
