/**
 * @file assert.c
 * @brief Verifies the compile-time C89 assert.h interface.
 */
#include <assert.h>

#ifndef assert
#error "assert must be defined as a macro"
#endif

/**
 * @brief Compiles an enabled assertion expression.
 * @param value Expression value supplied to the assertion.
 */
static void wcrt_test_assert_enabled(int value)
{
    assert(value);
}

#define NDEBUG
#include <assert.h>

/** @brief Compiles an assertion while NDEBUG is defined. */
static void wcrt_test_assert_disabled(void)
{
    assert(0);
}
