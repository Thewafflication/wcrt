/**
 * @file errno.c
 * @brief Exercises the C89 error-indicator lvalue and constants.
 */

#include <errno.h>

/** @brief Unrelated storage used to detect accidental errno aliasing. */
static int unrelated_object;

/**
 * @brief Runs error-indicator storage and constant checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    int *first_address;
    int *second_address;

    if (EDOM <= 0 || ERANGE <= 0 || EDOM == ERANGE) {
        return 1;
    }
    errno = EDOM;
    if (errno != EDOM) {
        return 2;
    }
    first_address = &errno;
    errno = ERANGE;
    second_address = &errno;
    if (errno != ERANGE || first_address != second_address) {
        return 3;
    }
    if (first_address == &unrelated_object) {
        return 4;
    }
    unrelated_object = 17;
    errno = 23;
    if (unrelated_object != 17 || errno != 23) {
        return 5;
    }
    errno = EDOM;
    if ((errno = ERANGE) != ERANGE) {
        return 6;
    }
    return 0;
}
