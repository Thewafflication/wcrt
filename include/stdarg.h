/**
 * @file stdarg.h
 * @brief Declares C89 variable-argument traversal facilities.
 */

#ifndef WCRT_STDARG_H
#define WCRT_STDARG_H

#if defined(_MSC_VER) && _MSC_VER <= 1200 && defined(_M_IX86)
/** @brief Stack cursor for the VC6 x86 C calling convention. */
typedef char *va_list;
/** @brief Rounds one x86 stack argument to a four-byte slot. */
#define WCRT_VA_SLOT(type) ((sizeof(type) + sizeof(int) - 1) & \
    ~(sizeof(int) - 1))
/** @brief Starts VC6 x86 traversal after the last named stack argument. */
#define va_start(arguments, last_named) \
    ((arguments) = (char *)&(last_named) + WCRT_VA_SLOT(last_named))
/** @brief Advances the VC6 x86 stack cursor and reads a promoted argument. */
#define va_arg(arguments, type) \
    (*((type *)(((arguments) += WCRT_VA_SLOT(type)) - WCRT_VA_SLOT(type))))
/** @brief Invalidates a VC6 x86 traversal. */
#define va_end(arguments) ((arguments) = (va_list)0)
#else
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

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
/** @brief Creates an independent copy of a variable-argument traversal. */
#define va_copy(destination, source) \
    __builtin_va_copy(destination, source)
#endif

#endif
