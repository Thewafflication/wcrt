/* REQ-0001: <assert.h> presence. Compile only; do not link. */
#include <assert.h>

#ifndef assert
#error "assert must be defined as a macro"
#endif

static void wcrt_test_assert_enabled(int value)
{
    assert(value);
}

#define NDEBUG
#include <assert.h>

static void wcrt_test_assert_disabled(void)
{
    assert(0);
}
