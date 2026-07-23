/**
 * @file assert_disabled.c
 * @brief Exercises NDEBUG and repeated inclusion of assert.h.
 */
#define NDEBUG
#include <assert.h>

/**
 * @brief Evaluates a disabled assertion expression.
 * @return Zero when the expression is not evaluated.
 */
int wcrt_assert_disabled_probe(void)
{
    int evaluations = 0;
    assert(++evaluations);
    return evaluations;
}

#undef NDEBUG
#include <assert.h>

/**
 * @brief Evaluates an assertion after NDEBUG is removed.
 * @return One when the expression is evaluated exactly once.
 */
int wcrt_assert_reenabled_probe(void)
{
    int evaluations = 0;
    assert(++evaluations);
    return evaluations;
}
