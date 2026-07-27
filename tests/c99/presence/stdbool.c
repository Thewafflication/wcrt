/**
 * @file stdbool.c
 * @brief Verifies the compile-time C99 Boolean interface.
 */
#include <stdbool.h>
#include <stdbool.h>

#if true != 1
#error true must expand to one
#endif
#if false != 0
#error false must expand to zero
#endif
#if __bool_true_false_are_defined != 1
#error Boolean definitions marker must be one
#endif

typedef char wcrt_true_constant_expression[true ? 1 : -1];
typedef char wcrt_false_constant_expression[false ? -1 : 1];

static bool wcrt_boolean_value = false;
