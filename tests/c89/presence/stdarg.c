/* REQ-0010: <stdarg.h> presence. Compile only; do not link. */
#include <stdarg.h>

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
