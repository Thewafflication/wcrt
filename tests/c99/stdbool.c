/**
 * @file stdbool.c
 * @brief Exercises C99 Boolean conversions and values.
 */
#include <stdbool.h>

/** @brief Runs Boolean conversion checks. */
int main(void)
{
    bool zero = 0;
    bool positive = 7;
    bool negative = -3;
    int object = 0;
    bool pointer = &object;

    if (zero != false) return 1;
    if (positive != true || positive != 1) return 2;
    if (negative != true || negative != 1) return 3;
    if (pointer != true) return 4;
    if (sizeof(bool) != sizeof(_Bool)) return 5;
    return 0;
}
