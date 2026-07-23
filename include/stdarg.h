/**
 * @file stdarg.h
 * @brief Declares C89 variable-argument traversal facilities.
 */

#ifndef WCRT_STDARG_H
#define WCRT_STDARG_H

/** @brief Compiler-owned state for one variable-argument traversal. */
typedef __builtin_va_list va_list;

/** @brief Initializes a traversal after the last named parameter. */
#define va_start(arguments, last_named) \
    __builtin_va_start(arguments, last_named)

/** @brief Retrieves the next argument using its promoted type. */
#define va_arg(arguments, type) __builtin_va_arg(arguments, type)

/** @brief Completes a variable-argument traversal. */
#define va_end(arguments) __builtin_va_end(arguments)

#endif
