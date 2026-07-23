/**
 * @file setjmp.c
 * @brief Exercises C89 non-local jumps and saved stack contexts.
 */

#include <setjmp.h>

#if defined(__aarch64__)
/** @brief Compile-time check of the Windows ARM64 jump-buffer ABI size. */
typedef char wcrt_arm64_jmp_buf_size[
    sizeof(jmp_buf) == 168 ? 1 : -1
];
#endif

/** @brief Outer environment used by nested jump checks. */
static jmp_buf outer_environment;

/** @brief Inner environment used by nested jump checks. */
static jmp_buf inner_environment;

/**
 * @brief Jumps through an additional stack frame.
 * @param environment Destination environment.
 * @param value Value supplied to longjmp.
 */
static void jump_from_callee(jmp_buf environment, int value)
{
    longjmp(environment, value);
}

/**
 * @brief Verifies nested independent saved environments.
 * @return Zero on success or a test-specific failure code.
 */
static int check_nested(void)
{
    int outer_value = setjmp(outer_environment);

    if (outer_value == 0) {
        int inner_value = setjmp(inner_environment);
        if (inner_value == 0) {
            longjmp(inner_environment, 7);
        }
        if (inner_value != 7) {
            return 1;
        }
        longjmp(outer_environment, 9);
    }
    return outer_value == 9 ? 0 : 2;
}

/**
 * @brief Runs direct, zero-value, callee, volatile, and nested jump checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    jmp_buf environment;
    volatile int preserved = 3;
    int value = setjmp(environment);

    if (value == 0) {
        preserved = 11;
        jump_from_callee(environment, 0);
    }
    if (value != 1 || preserved != 11) {
        return 1;
    }

    value = setjmp(environment);
    if (value == 0) {
        jump_from_callee(environment, 42);
    }
    if (value != 42) {
        return 2;
    }
    if (check_nested() != 0) {
        return 3;
    }
    return 0;
}
