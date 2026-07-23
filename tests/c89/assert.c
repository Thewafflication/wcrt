/**
 * @file assert.c
 * @brief Exercises enabled assertions and assertion failure behavior.
 */
#include <assert.h>

/** @brief Returns the evaluation count of a disabled assertion. */
int wcrt_assert_disabled_probe(void);

/** @brief Returns the evaluation count of a re-enabled assertion. */
int wcrt_assert_reenabled_probe(void);

/**
 * @brief Evaluates an enabled assertion with a side effect.
 * @return The number of expression evaluations.
 */
static int wcrt_enabled_probe(void)
{
    int evaluations = 0;
    assert(++evaluations);
    return evaluations;
}

/**
 * @brief Runs normal assertion checks or deliberately fails an assertion.
 * @param argument_count Number of command-line arguments.
 * @param arguments Command-line argument vector.
 * @return Zero on success or a test-specific failure code.
 */
int main(int argument_count, char **arguments)
{
    if (argument_count > 1 && arguments[1][0] == 'f') {
        assert(0);
        return 90;
    }

    if (wcrt_enabled_probe() != 1) {
        return 1;
    }
    if (wcrt_assert_disabled_probe() != 0) {
        return 2;
    }
    if (wcrt_assert_reenabled_probe() != 1) {
        return 3;
    }

    assert(1);
    return 0;
}
