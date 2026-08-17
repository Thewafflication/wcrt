/**
 * @file existing-headers.c
 * @brief Verifies C99 additions to the inherited C89 header families.
 */

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <locale.h>
#include <stddef.h>
#include <stdlib.h>

typedef char wcrt_mb_cur_max_type[
    __builtin_types_compatible_p(__typeof__(MB_CUR_MAX), size_t) ? 1 : -1];
typedef char wcrt_mb_cur_max_constant[(MB_CUR_MAX > 0) ? 1 : -1];

/**
 * @brief Exercises the focused C99 additions.
 * @return Zero when all checks pass.
 */
int main(void)
{
    struct lconv *conventions = localeconv();

    assert(MB_CUR_MAX == 1);
    if (!isblank(' ') || !isblank('\t')) return 1;
    if (isblank('\n') || isblank('A') || isblank(-1)) return 2;
    if (isblank(0xff)) return 3;
    if (conventions->int_p_cs_precedes != CHAR_MAX) return 4;
    if (conventions->int_n_cs_precedes != CHAR_MAX) return 5;
    if (conventions->int_p_sep_by_space != CHAR_MAX) return 6;
    if (conventions->int_n_sep_by_space != CHAR_MAX) return 7;
    if (conventions->int_p_sign_posn != CHAR_MAX) return 8;
    if (conventions->int_n_sign_posn != CHAR_MAX) return 9;
    return 0;
}
