/**
 * @file stdlib-exit.c
 * @brief Proves that C99 _Exit bypasses registered exit callbacks.
 */
#include <stdlib.h>

/** @brief Produces a distinguishable status if normal cleanup invokes it. */
static void unexpected_callback(void)
{
    _Exit(99);
}

/** @brief Registers cleanup and terminates without running it. */
int main(void)
{
    if (atexit(unexpected_callback) != 0) {
        return 98;
    }
    _Exit(23);
    return 97;
}
