/**
 * @file math.c
 * @brief Exercises C89 mathematics accuracy, boundaries, and errors.
 */

#include <errno.h>
#include <float.h>
#include <math.h>

/** @brief Pi reference used by the behavioral checks. */
#define TEST_PI 3.14159265358979323846

/**
 * @brief Compares two finite values with scaled absolute tolerance.
 * @param actual Value returned by the function under test.
 * @param expected Reference value.
 * @param tolerance Maximum scaled error.
 * @return Nonzero when the values are sufficiently close.
 */
static int nearly_equal(double actual, double expected, double tolerance)
{
    double scale = fabs(expected);
    double difference = fabs(actual - expected);

    if (scale < 1.0) scale = 1.0;
    return difference <= tolerance * scale;
}

/**
 * @brief Runs ordinary-value, identity, pointer, and error checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    double integer_part;
    double fraction;
    int exponent;

    if (!nearly_equal(sin(0.0), 0.0, 1e-12)) return 1;
    if (!nearly_equal(sin(TEST_PI / 6.0), 0.5, 1e-11)) return 2;
    if (!nearly_equal(cos(0.0), 1.0, 1e-12)) return 3;
    if (!nearly_equal(cos(TEST_PI), -1.0, 1e-11)) return 4;
    if (!nearly_equal(tan(TEST_PI / 4.0), 1.0, 1e-11)) return 5;
    if (!nearly_equal(asin(0.5), TEST_PI / 6.0, 1e-11)) return 6;
    if (!nearly_equal(acos(0.5), TEST_PI / 3.0, 1e-11)) return 7;
    if (!nearly_equal(atan(1.0), TEST_PI / 4.0, 1e-11)) return 8;
    if (!nearly_equal(atan2(1.0, -1.0),
                      3.0 * TEST_PI / 4.0, 1e-11)) return 9;
    if (!nearly_equal(exp(1.0), 2.718281828459045, 1e-11)) return 10;
    if (!nearly_equal(log(2.718281828459045), 1.0, 1e-11)) return 11;
    if (!nearly_equal(log10(1000.0), 3.0, 1e-11)) return 12;
    if (!nearly_equal(sqrt(2.0), 1.414213562373095, 1e-11)) return 13;
    if (!nearly_equal(pow(2.0, 10.0), 1024.0, 1e-10)) return 14;
    if (!nearly_equal(pow(-2.0, 3.0), -8.0, 1e-11)) return 15;
    if (!nearly_equal(sinh(1.0), 1.175201193643801, 1e-11)) return 16;
    if (!nearly_equal(cosh(1.0), 1.543080634815244, 1e-11)) return 17;
    if (!nearly_equal(tanh(1.0), 0.761594155955765, 1e-11)) return 18;

    fraction = frexp(8.0, &exponent);
    if (fraction != 0.5 || exponent != 4) return 19;
    if (ldexp(0.5, 4) != 8.0) return 20;
    fraction = modf(-3.25, &integer_part);
    if (fraction != -0.25 || integer_part != -3.0) return 21;
    if (floor(2.75) != 2.0 || floor(-2.25) != -3.0) return 22;
    if (ceil(2.25) != 3.0 || ceil(-2.75) != -2.0) return 23;
    if (fabs(-3.5) != 3.5) return 24;
    if (!nearly_equal(fmod(7.0, 2.5), 2.0, 1e-12)) return 25;
    if (HUGE_VAL < DBL_MAX) return 26;

    errno = 0;
    (void)acos(2.0);
    if (errno != EDOM) return 27;
    errno = 0;
    (void)asin(-2.0);
    if (errno != EDOM) return 28;
    errno = 0;
    (void)sqrt(-1.0);
    if (errno != EDOM) return 29;
    errno = 0;
    (void)log(0.0);
    if (errno != EDOM) return 30;
    errno = 0;
    (void)atan2(0.0, 0.0);
    if (errno != EDOM) return 31;
    errno = 0;
    (void)fmod(1.0, 0.0);
    if (errno != EDOM) return 32;
    errno = 0;
    (void)pow(-2.0, 0.5);
    if (errno != EDOM) return 33;
    errno = 0;
    (void)exp(1000.0);
    if (errno != ERANGE) return 34;
    errno = 0;
    (void)ldexp(DBL_MAX, 1);
    if (errno != ERANGE) return 35;
    return 0;
}
