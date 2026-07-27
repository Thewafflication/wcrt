/**
 * @file stdint.c
 * @brief Exercises C99 integer limits, constants, and pointer types.
 */
#include <stdint.h>
#include <stddef.h>

/** @brief Runs fixed-width integer and pointer checks. */
int main(void)
{
    int object = 0;
    void *pointer = &object;
    intptr_t signed_pointer = (intptr_t)pointer;
    uintptr_t unsigned_pointer = (uintptr_t)pointer;

    if (INT8_MIN != -INT8_MAX - 1) return 1;
    if (INT16_MIN != -INT16_MAX - 1) return 2;
    if (INT32_MIN != -INT32_MAX - 1) return 3;
    if (INT64_MIN != -INT64_MAX - INT64_C(1)) return 4;
    if (UINT8_MAX != (uint8_t)-1) return 5;
    if (UINT16_MAX != (uint16_t)-1) return 6;
    if (UINT32_MAX != (uint32_t)-1) return 7;
    if (UINT64_MAX != (uint64_t)-1) return 8;
    if ((void *)signed_pointer != pointer) return 9;
    if ((void *)unsigned_pointer != pointer) return 10;
    if (SIZE_MAX != (size_t)-1) return 11;
    if (INTMAX_MAX < INT64_MAX || UINTMAX_MAX < UINT64_MAX) return 12;
    return 0;
}
