/**
 * @file stdlib_exit.c
 * @brief Exercises reverse atexit ordering and normal process termination.
 */

#include <stdlib.h>

/** @brief State passed implicitly between reverse-order callbacks. */
static int callback_state;

/** @brief First-registered callback, expected to execute last. */
static void first_callback(void)
{
    if (callback_state != 1) {
        abort();
    }
}

/** @brief Last-registered callback, expected to execute first. */
static void second_callback(void)
{
    callback_state = 1;
}

/** @brief Registers callbacks and terminates normally with status 23. */
int main(void)
{
    if (atexit(first_callback) != 0 || atexit(second_callback) != 0) {
        return 1;
    }
    exit(23);
    return 2;
}
