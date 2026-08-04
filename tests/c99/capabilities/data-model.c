/**
 * @file data-model.c
 * @brief Verifies the WCRT Windows data model at compile time.
 */
#include <stddef.h>

#ifndef WCRT_EXPECT_POINTER_SIZE
#error WCRT_EXPECT_POINTER_SIZE must select the target data model
#endif

typedef char wcrt_char_size[sizeof(char) == 1 ? 1 : -1];
typedef char wcrt_short_size[sizeof(short) == 2 ? 1 : -1];
typedef char wcrt_int_size[sizeof(int) == 4 ? 1 : -1];
typedef char wcrt_long_size[sizeof(long) == 4 ? 1 : -1];
typedef char wcrt_long_long_size[sizeof(long long) == 8 ? 1 : -1];
typedef char wcrt_float_size[sizeof(float) == 4 ? 1 : -1];
typedef char wcrt_double_size[sizeof(double) == 8 ? 1 : -1];
typedef char wcrt_long_double_size[sizeof(long double) == 8 ? 1 : -1];
typedef char wcrt_bool_size[sizeof(_Bool) == 1 ? 1 : -1];
typedef char wcrt_pointer_size[
    sizeof(void *) == WCRT_EXPECT_POINTER_SIZE ? 1 : -1];
typedef char wcrt_size_size[
    sizeof(size_t) == WCRT_EXPECT_POINTER_SIZE ? 1 : -1];
typedef char wcrt_ptrdiff_size[
    sizeof(ptrdiff_t) == WCRT_EXPECT_POINTER_SIZE ? 1 : -1];
typedef char wcrt_wchar_size[sizeof(wchar_t) == 2 ? 1 : -1];
typedef char wcrt_plain_char_signed[((char)-1 < 0) ? 1 : -1];

