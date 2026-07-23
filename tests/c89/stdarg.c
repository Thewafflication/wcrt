/**
 * @file stdarg.c
 * @brief Exercises C89 variable-argument traversal across the Windows ABI.
 */

#include <stdarg.h>

/** @brief Aggregate used to verify supported by-value traversal. */
struct pair {
    int first;  /**< First member. */
    int second; /**< Second member. */
};

/**
 * @brief Checks promoted scalars, a pointer, and a by-value aggregate.
 * @param marker Last named parameter.
 * @return Zero on success or a test-specific failure code.
 */
static int check_mixed(int marker, ...)
{
    va_list arguments;
    int promoted_character;
    int promoted_short;
    double promoted_float;
    long long_value;
    void *pointer_value;
    struct pair pair_value;

    va_start(arguments, marker);
    promoted_character = va_arg(arguments, int);
    promoted_short = va_arg(arguments, int);
    promoted_float = va_arg(arguments, double);
    long_value = va_arg(arguments, long);
    pointer_value = va_arg(arguments, void *);
    pair_value = va_arg(arguments, struct pair);
    va_end(arguments);

    if (marker != 7 || promoted_character != 65 || promoted_short != -123) {
        return 1;
    }
    if (promoted_float != 1.25 || long_value != 123456L) {
        return 2;
    }
    if (pointer_value == (void *)0) {
        return 3;
    }
    return pair_value.first == 11 && pair_value.second == 19 ? 0 : 4;
}

/**
 * @brief Accepts an empty variable tail without beginning a traversal.
 * @param count Number of tail arguments.
 * @return The named count.
 */
static int check_empty(int count, ...)
{
    return count;
}

/**
 * @brief Runs repeated ABI traversal and empty-tail checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    char character = 65;
    short short_value = -123;
    float float_value = 1.25f;
    int pointed_to = 9;
    struct pair value = {11, 19};
    int result;

    result = check_mixed(7, character, short_value, float_value, 123456L,
        &pointed_to, value);
    if (result != 0) {
        return result;
    }
    result = check_mixed(7, character, short_value, float_value, 123456L,
        &pointed_to, value);
    if (result != 0) {
        return result + 10;
    }
    return check_empty(0) == 0 ? 0 : 20;
}
