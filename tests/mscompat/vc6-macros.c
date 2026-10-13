/**
 * @file vc6-macros.c
 * @brief Exercises selected C89 macro expansions through compiler call sites.
 */

#define WCRT_C89 1
#define NDEBUG 1
#include <assert.h>

static int assertion_effect;

/** @brief Disabled assertion expansion must suppress its side effect. */
static int disabled_assertion(void)
{
    assert(++assertion_effect);
    return assertion_effect;
}

#undef NDEBUG
#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief Exercises offsetof expansion with an aligned integer member. */
struct macro_offsets {
    char first; /**< Byte field. */
    long second; /**< Naturally aligned Windows integer. */
};

typedef char macro_model[offsetof(struct macro_offsets, second) == 4 &&
    sizeof(wchar_t) == 2 && CHAR_BIT == 8 && LONG_MAX == 2147483647L &&
    DBL_MANT_DIG == 53 && LDBL_MANT_DIG == 53 && FLT_RADIX == 2 &&
    EOF == -1 && MB_CUR_MAX == 1 ? 1 : -1];

static volatile sig_atomic_t delivered;

/** @brief Records only the synchronously delivered test signal. */
static void macro_signal(int number)
{
    if (number == SIGINT) delivered = 1;
}

/** @brief Exercises the WCRT setjmp expansion with a valid controlling use. */
static int macro_jump(void)
{
    jmp_buf environment;
    volatile int changed = 0;
    switch (setjmp(environment)) {
    case 0:
        changed = 7;
        longjmp(environment, 0);
        return 1;
    case 1:
        return changed != 7;
    default:
        return 2;
    }
}

/** @brief Executes representative C89 macros and their runtime interfaces. */
int check_vc6_macros(void)
{
    void (*previous)(int);
    struct lconv *locale;
    volatile double math_limit = HUGE_VAL;
    volatile double double_limit = DBL_MAX;
    volatile long double long_limit = LDBL_MAX;
    volatile float float_limit = FLT_MAX;
    float maximum = FLT_MAX;
    unsigned int representation;
    if (disabled_assertion() != 0) return 1;
    assert(++assertion_effect == 1);
    if (assertion_effect != 1 || math_limit != double_limit ||
        long_limit != double_limit || float_limit <= 1.0F) return 2;
    memcpy(&representation, &maximum, sizeof(representation));
    if (representation != 0x7f7fffffU) return 8;
    if (!isdigit('7') || toupper('a') != 'A' ||
        setlocale(LC_ALL, NULL) == NULL) return 3;
    locale = localeconv();
    if (locale == NULL || strcmp(locale->decimal_point, ".")) return 4;
    previous = signal(SIGINT, macro_signal);
    if (previous == SIG_ERR || raise(SIGINT) != 0 || delivered != 1)
        return 5;
    if (signal(SIGINT, previous) == SIG_ERR) return 6;
    return macro_jump() ? 7 : 0;
}
