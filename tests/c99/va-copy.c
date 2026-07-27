/**
 * @file va-copy.c
 * @brief Exercises independent C99 variable-argument traversal.
 */
#include <stdarg.h>

/** @brief Traverses an original and copied argument list independently. */
static int verify_copy(int count, ...)
{
    va_list original;
    va_list copy;
    int original_first;
    int copy_first;
    int original_second;
    int copy_second;

    va_start(original, count);
    va_copy(copy, original);
    original_first = va_arg(original, int);
    original_second = va_arg(original, int);
    copy_first = va_arg(copy, int);
    copy_second = va_arg(copy, int);
    va_end(copy);
    va_end(original);
    if (count != 2) return 1;
    if (original_first != 17 || copy_first != 17) return 2;
    if (original_second != 29 || copy_second != 29) return 3;
    return 0;
}

/** @brief Runs variable-argument copy checks. */
int main(void)
{
    return verify_copy(2, 17, 29);
}
