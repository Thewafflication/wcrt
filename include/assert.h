/**
 * @file assert.h
 * @brief Declares the C89 assertion diagnostic macro.
 *
 * Waughtal C Run Time
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * C89 4.2 Diagnostics
 *
 * The assert macro intentionally lives outside the declaration guard. C89
 * permits a program to change NDEBUG and include this header again.
 */

#ifndef WCRT_ASSERT_DECLARATIONS_INCLUDED
#define WCRT_ASSERT_DECLARATIONS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports a failed assertion and terminates the program.
 * @param expression Text of the expression that evaluated to zero.
 * @param file Source file containing the failed assertion.
 * @param line Source line containing the failed assertion.
 * @note This function does not return.
 */
void __wcrt_assert_fail(const char *expression, const char *file, int line);

#ifdef __cplusplus
}
#endif

#endif

#undef assert

#ifdef NDEBUG
/** @brief Discards an assertion expression when diagnostics are disabled. */
#define assert(expression) ((void)0)
#else
/** @brief Tests an expression and reports a diagnostic when it is zero. */
#define assert(expression) \
    ((expression) ? (void)0 : \
        __wcrt_assert_fail(#expression, __FILE__, __LINE__))
#endif
