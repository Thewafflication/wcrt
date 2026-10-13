/** @file vc6-c89-calls.c @brief Exercises C89 call/return and callback ABI. */
#define WCRT_C89 1
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static int comparator_calls;

/** @brief Compares signed integers without subtraction overflow. */
static int compare_numbers(const void *left, const void *right)
{
    int first = *(const int *)left;
    int second = *(const int *)right;
    ++comparator_calls;
    return (first > second) - (first < second);
}

/** @brief Forces real calls for aggregate, floating and callback interfaces. */
int check_vc6_c89_calls(void)
{
    div_t (*volatile divide_call)(int, int) = div;
    ldiv_t (*volatile long_divide_call)(long, long) = ldiv;
    double (*volatile square_call)(double) = sqrt;
    double (*volatile power_call)(double, double) = pow;
    div_t quotient;
    ldiv_t long_quotient;
    volatile double value;
    double integral;
    int exponent;
    int values[] = {4, -1, 4, 0, INT_MIN, INT_MAX};
    int wanted = 4;
    int *found;
    int *zeroes;
    int index;
    char *allocation;
    char *grown;
    char *end;
    const char *real_text = " -12.5tail";
    const char *integer_text = " -123tail";
    quotient = divide_call(-17, 5);
    long_quotient = long_divide_call(-2147483647L - 1L, 7L);
    if (quotient.quot != -3 || quotient.rem != -2 ||
        long_quotient.quot != -306783378L || long_quotient.rem != -2)
        return 1;
    errno = EDOM;
    if (square_call(9.0) != 3.0 || power_call(2.0, 3.0) != 8.0 ||
        errno != EDOM) return 2;
    if (frexp(1.5, &exponent) != 0.75 || exponent != 1 ||
        ldexp(0.75, 1) != 1.5 || modf(-2.25, &integral) != -0.25 ||
        integral != -2.0) return 3;
    errno = 0;
    value = square_call(-1.0);
    if (value != 0.0 || errno != EDOM) return 4;
    errno = EDOM;
    if (strtod(real_text, &end) != -12.5 || end != real_text + 6 ||
        strtol(integer_text, &end, 10) != -123 || end != integer_text + 5 ||
        abs(-17) != 17 || labs(-1234L) != 1234L || errno != EDOM)
        return 5;
    qsort(values, 6, sizeof(values[0]), compare_numbers);
    if (values[0] != INT_MIN || values[1] != -1 || values[2] != 0 ||
        values[3] != 4 || values[4] != 4 || values[5] != INT_MAX ||
        comparator_calls == 0) return 6;
    found = bsearch(&wanted, values, 6, sizeof(values[0]), compare_numbers);
    if (found == NULL || *found != 4) return 7;
    allocation = malloc(8);
    if (allocation == NULL) return 8;
    strcpy(allocation, "abc");
    grown = realloc(allocation, 16);
    if (grown == NULL) { free(allocation); return 9; }
    if (strcmp(grown, "abc")) { free(grown); return 10; }
    free(grown);
    zeroes = calloc(4, sizeof(int));
    if (zeroes == NULL) return 11;
    for (index = 0; index < 4; ++index) {
        if (zeroes[index] != 0) { free(zeroes); return 12; }
    }
    free(zeroes);
    return 0;
}
