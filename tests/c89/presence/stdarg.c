/**
 * @file stdarg.c
 * @brief Verifies the compile-time C89 stdarg.h interface.
 * @details File-scope declarations are typed API presence probes.
 */
#include <stdarg.h>

/**
 * @brief Exercises all C89 variable-argument macros.
 * @param count Number of following double arguments.
 * @return Sum of the supplied arguments.
 */
static double wcrt_read_arguments(int count, ...)
{
    va_list arguments;
    int integer_value;
    double double_value;

    va_start(arguments, count);
    integer_value = va_arg(arguments, int);
    double_value = va_arg(arguments, double);
    va_end(arguments);

    return (double)integer_value + double_value;
}

static double (*wcrt_variadic_address)(int, ...) = wcrt_read_arguments;
