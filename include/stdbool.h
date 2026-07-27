/**
 * @file stdbool.h
 * @brief Defines the C99 Boolean type and values.
 */

#ifndef WCRT_STDBOOL_H
#define WCRT_STDBOOL_H

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/** @brief Convenient spelling of the compiler-owned Boolean type. */
#define bool _Bool
/** @brief Boolean true value. */
#define true 1
/** @brief Boolean false value. */
#define false 0
/** @brief Indicates that the standard Boolean macros are defined. */
#define __bool_true_false_are_defined 1
#endif

#endif
