/**
 * @file assert-diagnostic.c
 * @brief Produces a C99 assertion diagnostic for field verification.
 */

#include <assert.h>

/**
 * @brief Deliberately fails an assertion.
 * @return This function does not return normally.
 */
int main(void)
{
    assert(0);
    return 90;
}
