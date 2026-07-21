/* REQ-0006: <locale.h> presence. Compile only; do not link. */
#include <locale.h>

#ifndef NULL
#error "NULL must be defined"
#endif
#ifndef LC_ALL
#error "LC_ALL must be defined"
#endif
#ifndef LC_COLLATE
#error "LC_COLLATE must be defined"
#endif
#ifndef LC_CTYPE
#error "LC_CTYPE must be defined"
#endif
#ifndef LC_MONETARY
#error "LC_MONETARY must be defined"
#endif
#ifndef LC_NUMERIC
#error "LC_NUMERIC must be defined"
#endif
#ifndef LC_TIME
#error "LC_TIME must be defined"
#endif

typedef char *(*wcrt_setlocale_fn)(int, const char *);
typedef struct lconv *(*wcrt_localeconv_fn)(void);

static wcrt_setlocale_fn wcrt_setlocale_address = (setlocale);
static wcrt_localeconv_fn wcrt_localeconv_address = (localeconv);
static struct lconv wcrt_lconv_value;

static void wcrt_lconv_members(void)
{
    wcrt_lconv_value.decimal_point = (char *)0;
    wcrt_lconv_value.thousands_sep = (char *)0;
    wcrt_lconv_value.grouping = (char *)0;
    wcrt_lconv_value.int_curr_symbol = (char *)0;
    wcrt_lconv_value.currency_symbol = (char *)0;
    wcrt_lconv_value.mon_decimal_point = (char *)0;
    wcrt_lconv_value.mon_thousands_sep = (char *)0;
    wcrt_lconv_value.mon_grouping = (char *)0;
    wcrt_lconv_value.positive_sign = (char *)0;
    wcrt_lconv_value.negative_sign = (char *)0;
    wcrt_lconv_value.int_frac_digits = 0;
    wcrt_lconv_value.frac_digits = 0;
    wcrt_lconv_value.p_cs_precedes = 0;
    wcrt_lconv_value.p_sep_by_space = 0;
    wcrt_lconv_value.n_cs_precedes = 0;
    wcrt_lconv_value.n_sep_by_space = 0;
    wcrt_lconv_value.p_sign_posn = 0;
    wcrt_lconv_value.n_sign_posn = 0;
}

enum {
    wcrt_locale_constants_present =
        LC_ALL + LC_COLLATE + LC_CTYPE + LC_MONETARY + LC_NUMERIC + LC_TIME
};
