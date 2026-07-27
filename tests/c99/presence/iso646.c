/**
 * @file iso646.c
 * @brief Verifies the complete C99 alternative-token macro set.
 */
#include <iso646.h>
#include <iso646.h>

#if !defined(and) || !defined(and_eq) || !defined(bitand)
#error Missing alternative operator macro
#endif
#if !defined(bitor) || !defined(compl) || !defined(not)
#error Missing alternative operator macro
#endif
#if !defined(not_eq) || !defined(or) || !defined(or_eq)
#error Missing alternative operator macro
#endif
#if !defined(xor) || !defined(xor_eq)
#error Missing alternative operator macro
#endif

typedef char wcrt_logical_tokens[(1 and 1) or 0 ? 1 : -1];
typedef char wcrt_bitwise_tokens[((1 bitor 2) xor 1) == 2 ? 1 : -1];
