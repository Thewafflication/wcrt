/**
 * @file limits.c
 * @brief Verifies the compile-time C89 limits.h interface.
 * @details File-scope declarations are typed API presence probes.
 */
#include <limits.h>

#ifndef CHAR_BIT
#error "CHAR_BIT must be defined"
#endif
#ifndef SCHAR_MIN
#error "SCHAR_MIN must be defined"
#endif
#ifndef SCHAR_MAX
#error "SCHAR_MAX must be defined"
#endif
#ifndef UCHAR_MAX
#error "UCHAR_MAX must be defined"
#endif
#ifndef CHAR_MIN
#error "CHAR_MIN must be defined"
#endif
#ifndef CHAR_MAX
#error "CHAR_MAX must be defined"
#endif
#ifndef MB_LEN_MAX
#error "MB_LEN_MAX must be defined"
#endif
#ifndef SHRT_MIN
#error "SHRT_MIN must be defined"
#endif
#ifndef SHRT_MAX
#error "SHRT_MAX must be defined"
#endif
#ifndef USHRT_MAX
#error "USHRT_MAX must be defined"
#endif
#ifndef INT_MIN
#error "INT_MIN must be defined"
#endif
#ifndef INT_MAX
#error "INT_MAX must be defined"
#endif
#ifndef UINT_MAX
#error "UINT_MAX must be defined"
#endif
#ifndef LONG_MIN
#error "LONG_MIN must be defined"
#endif
#ifndef LONG_MAX
#error "LONG_MAX must be defined"
#endif
#ifndef ULONG_MAX
#error "ULONG_MAX must be defined"
#endif

static signed char wcrt_schar_values[] = { SCHAR_MIN, SCHAR_MAX };
static unsigned char wcrt_uchar_value = UCHAR_MAX;
static char wcrt_char_values[] = { CHAR_MIN, CHAR_MAX };
static short wcrt_short_values[] = { SHRT_MIN, SHRT_MAX };
static unsigned short wcrt_ushort_value = USHRT_MAX;
static int wcrt_int_values[] = { INT_MIN, INT_MAX };
static unsigned int wcrt_uint_value = UINT_MAX;
static long wcrt_long_values[] = { LONG_MIN, LONG_MAX };
static unsigned long wcrt_ulong_value = ULONG_MAX;

enum {
    wcrt_char_bit_positive = 1 / (CHAR_BIT > 0),
    wcrt_mb_len_max_positive = 1 / (MB_LEN_MAX > 0)
};
