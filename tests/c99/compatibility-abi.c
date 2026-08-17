/**
 * @file compatibility-abi.c
 * @brief Verifies cross-family C99 ABI types and layouts.
 */

#include <complex.h>
#include <fenv.h>
#include <locale.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#ifndef WCRT_EXPECT_POINTER_SIZE
#error WCRT_EXPECT_POINTER_SIZE is required
#endif

#ifndef WCRT_EXPECT_JMP_SIZE
#error WCRT_EXPECT_JMP_SIZE is required
#endif

typedef char wcrt_pointer_size[
    sizeof(void *) == WCRT_EXPECT_POINTER_SIZE ? 1 : -1];
typedef char wcrt_size_type_size[
    sizeof(size_t) == WCRT_EXPECT_POINTER_SIZE ? 1 : -1];
typedef char wcrt_ptrdiff_type_size[
    sizeof(ptrdiff_t) == WCRT_EXPECT_POINTER_SIZE ? 1 : -1];
typedef char wcrt_wchar_type_size[sizeof(wchar_t) == 2 ? 1 : -1];
typedef char wcrt_wint_type_size[sizeof(wint_t) == 4 ? 1 : -1];
typedef char wcrt_time_type_size[sizeof(time_t) == 8 ? 1 : -1];
typedef char wcrt_clock_type_size[sizeof(clock_t) == 4 ? 1 : -1];
typedef char wcrt_mbstate_size[sizeof(mbstate_t) == 8 ? 1 : -1];
typedef char wcrt_fenv_size[sizeof(fenv_t) == 16 ? 1 : -1];
typedef char wcrt_fexcept_size[sizeof(fexcept_t) == 4 ? 1 : -1];
typedef char wcrt_jmp_size[
    sizeof(jmp_buf) == WCRT_EXPECT_JMP_SIZE ? 1 : -1];
typedef char wcrt_float_complex_size[
    sizeof(float complex) == 8 ? 1 : -1];
typedef char wcrt_double_complex_size[
    sizeof(double complex) == 16 ? 1 : -1];
typedef char wcrt_long_double_complex_size[
    sizeof(long double complex) == 16 ? 1 : -1];
typedef char wcrt_mb_cur_max_type[
    __builtin_types_compatible_p(__typeof__(MB_CUR_MAX), size_t) ? 1 : -1];
typedef char wcrt_lconv_c99_tail[
    offsetof(struct lconv, int_n_sign_posn) >
        offsetof(struct lconv, n_sign_posn) ? 1 : -1];

/**
 * @brief Supplies a linkable translation-unit entry point.
 * @return Zero.
 */
int main(void)
{
    return 0;
}
