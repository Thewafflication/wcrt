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

/** @brief Copies and consumes a list forwarded by its caller. */
static int verify_forwarded_copy(va_list source, const char **copied_pointer)
{
    va_list copy;
    double copy_double;
    const char *copy_pointer;

    va_copy(copy, source);
    copy_double = va_arg(copy, double);
    copy_pointer = va_arg(copy, const char *);
    va_end(copy);
    if (copy_double != 2.5) return 1;
    if (*copy_pointer != 'x') return 2;
    *copied_pointer = copy_pointer;
    return 0;
}

/** @brief Copies a partially traversed list and verifies mixed arguments. */
static int verify_partial_copy(int count, ...)
{
    va_list original;
    int marker;
    int forwarded_result;
    double original_double;
    const char *copied_pointer;
    const char *original_pointer;

    va_start(original, count);
    marker = va_arg(original, int);
    forwarded_result = verify_forwarded_copy(original, &copied_pointer);
    original_double = va_arg(original, double);
    original_pointer = va_arg(original, const char *);
    va_end(original);
    if (count != 3 || marker != 11) return 1;
    if (forwarded_result != 0) return 2;
    if (original_double != 2.5) return 3;
    if (original_pointer != copied_pointer ||
        *original_pointer != 'x') return 4;
    return 0;
}

/** @brief Runs variable-argument copy checks. */
int main(void)
{
    static const char value[] = "x";
    int result = verify_copy(2, 17, 29);

    if (result != 0) return result;
    result = verify_partial_copy(3, 11, 2.5, value);
    return result == 0 ? 0 : result + 3;
}
