/**
 * @file stddef.h
 * @brief Declares C89 common types and macros.
 */

#ifndef WCRT_STDDEF_H
#define WCRT_STDDEF_H

/** @brief Unsigned type of the result of sizeof. */
typedef __SIZE_TYPE__ size_t;

/** @brief Signed type of pointer subtraction. */
typedef __PTRDIFF_TYPE__ ptrdiff_t;

/** @brief Integer type used to store a wide character. */
typedef __WCHAR_TYPE__ wchar_t;

/** @brief Integer null pointer constant. */
#ifndef NULL
#define NULL 0
#endif

/** @brief Constant byte offset of a structure or union member. */
#define offsetof(type, member) __builtin_offsetof(type, member)

#endif
