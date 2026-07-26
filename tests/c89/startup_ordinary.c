/**
 * @file startup_ordinary.c
 * @brief Verifies ordinary toolchain-owned startup with static WCRT.
 */

#include <string.h>

/**
 * @brief Uses WCRT through the ordinary TinyCC process startup.
 * @return Zero when the linked string function behaves as expected.
 */
int main(void)
{
    return strlen("wcrt") == 4 ? 0 : 1;
}
