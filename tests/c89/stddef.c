/**
 * @file stddef.c
 * @brief Exercises C89 common types, null pointers, and member offsets.
 */

#include <stddef.h>

/** @brief Inner aggregate for nested offset checks. */
struct inner_value {
    char prefix; /**< Leading byte. */
    int number;  /**< Naturally aligned member. */
};

/** @brief Aggregate containing padding, an array, and a nested structure. */
struct outer_value {
    char prefix;             /**< Leading byte before padding. */
    long values[3];          /**< Array member. */
    struct inner_value inner; /**< Nested member. */
};

/** @brief Proves that offset expressions are integer constant expressions. */
enum offset_constants {
    values_offset = offsetof(struct outer_value, values),
    nested_offset = offsetof(struct outer_value, inner.number)
};

/**
 * @brief Verifies ABI types, pointer arithmetic, nulls, and exact offsets.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    int values[4];
    int *null_object = NULL;
    void (*null_function)(void) = NULL;
    ptrdiff_t difference = &values[3] - &values[0];
    size_t object_size = sizeof(values);
    wchar_t wide_character = (wchar_t)0x1234;

    if (difference != 3 || object_size != 4 * sizeof(int)) {
        return 1;
    }
    if ((ptrdiff_t)-1 >= 0 || (size_t)-1 <= 0) {
        return 2;
    }
    if (sizeof(ptrdiff_t) != sizeof(void *) ||
        sizeof(size_t) != sizeof(void *)) {
        return 3;
    }
    if (sizeof(wchar_t) != 2 || wide_character != (wchar_t)0x1234) {
        return 4;
    }
    if (null_object != NULL || null_function != NULL) {
        return 5;
    }
    if (values_offset != sizeof(long)) {
        return 6;
    }
    if (nested_offset != offsetof(struct outer_value, inner) +
        offsetof(struct inner_value, number)) {
        return 7;
    }
    return 0;
}
