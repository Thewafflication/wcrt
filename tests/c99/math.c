/**
 * @file math.c
 * @brief Exercises C99 real-math classifications and special-value behavior.
 */

#include <errno.h>
#include <float.h>
#include <math.h>

static int nearly_equal(double actual, double expected, double tolerance)
{
    double difference = actual - expected;
    if (difference < 0.0) difference = -difference;
    return difference <= tolerance;
}

int main(void)
{
    double zero = 0.0;
    double neg_zero = -0.0;
    double inf = 1.0 / zero;
    double neg_inf = -1.0 / zero;
    double nan_value = 0.0 / zero;

    if (!isfinite(1.0)) return 1;
    if (isfinite(inf)) return 2;
    if (!isnan(nan_value)) return 3;
    if (!isinf(inf)) return 4;
    if (!isinf(neg_inf)) return 5;
    if (signbit(neg_zero) == 0) return 6;
    if (signbit(zero) != 0) return 7;
    if (fpclassify(nan_value) != FP_NAN) return 8;
    if (fpclassify(inf) != FP_INFINITE) return 9;
    if (fpclassify(0.0) != FP_ZERO) return 10;
    if (fpclassify(1.0) != FP_NORMAL) return 11;
    if (fmax(2.0, 3.0) != 3.0) return 12;
    if (fmin(2.0, 3.0) != 2.0) return 13;
    if (fdim(3.0, 2.0) != 1.0) return 14;
    if (fma(2.0, 3.0, 4.0) != 10.0) return 15;
    if (nextafter(1.0, 2.0) <= 1.0) return 16;
    if (nextafter(1.0, 0.0) >= 1.0) return 17;
    if (!nearly_equal(pow(10.0, 0.5), 3.162277660168379, 1e-12)) return 18;
    if (!nearly_equal(log(1.0), 0.0, 1e-12)) return 19;
    if (!nearly_equal(sqrt(2.0), 1.4142135623730951, 1e-12)) return 20;
    if (!nearly_equal(tanh(1.0), 0.7615941559557649, 1e-12)) return 21;

    errno = 0;
    (void)pow(-2.0, 0.5);
    if (errno != EDOM) return 22;
    errno = 0;
    (void)log(0.0);
    if (errno != EDOM) return 23;
    errno = 0;
    (void)exp(1000.0);
    if (errno != ERANGE) return 24;
    errno = 0;
    (void)ldexp(DBL_MAX, 1);
    if (errno != ERANGE) return 25;
    return 0;
}
