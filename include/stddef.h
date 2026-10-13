/**
 * @file stddef.h
 * @brief Declares C89 common types and macros.
 */

#ifndef WCRT_STDDEF_H
#define WCRT_STDDEF_H

#if defined(_MSC_VER) && _MSC_VER <= 1200 && defined(_M_IX86)
/** @brief Unsigned size type for the VC6 x86 C consumer ABI. */
typedef unsigned int size_t;
/** @brief Signed pointer difference for the VC6 x86 C consumer ABI. */
typedef int ptrdiff_t;
/** @brief Windows UTF-16 code unit for the VC6 C consumer ABI. */
typedef unsigned short wchar_t;
#else
/** @brief Unsigned type of the result of sizeof. */
typedef __SIZE_TYPE__ size_t;

/** @brief Signed type of pointer subtraction. */
typedef __PTRDIFF_TYPE__ ptrdiff_t;

/** @brief Integer type used to store a wide character. */
typedef __WCHAR_TYPE__ wchar_t;
#endif

/** @brief Integer null pointer constant. */
#ifndef NULL
#define NULL 0
#endif

/** @brief Constant byte offset of a structure or union member. */
#if defined(_MSC_VER) && _MSC_VER <= 1200 && defined(_M_IX86)
#define offsetof(type, member) ((size_t)&(((type *)0)->member))
#else
#define offsetof(type, member) __builtin_offsetof(type, member)
#endif

#endif
