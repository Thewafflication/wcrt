/**
 * @file locale.c
 * @brief Exercises C89 locale selection and C-locale conventions.
 */

#include <ctype.h>
#include <limits.h>
#include <locale.h>

/**
 * @brief Compares two null-terminated byte strings.
 * @param left First string.
 * @param right Second string.
 * @return Nonzero when both strings contain the same bytes.
 */
static int strings_equal(const char *left, const char *right)
{
    while (*left != '\0' && *left == *right) {
        ++left;
        ++right;
    }
    return *left == *right;
}

/**
 * @brief Runs locale query, update, failure, and convention checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    static int categories[] = {
        LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME
    };
    struct lconv *conventions;
    int index;

    if (!strings_equal(setlocale(LC_ALL, NULL), "C")) return 1;
    for (index = 0; index < 6; ++index) {
        if (!strings_equal(setlocale(categories[index], "C"), "C")) {
            return 2;
        }
        if (!strings_equal(setlocale(categories[index], NULL), "C")) {
            return 3;
        }
    }
    if (setlocale(LC_ALL, "unsupported") != NULL) return 4;
    if (!isalpha('A') || isalpha('1')) return 5;

    conventions = localeconv();
    if (conventions == NULL) return 6;
    if (!strings_equal(conventions->decimal_point, ".")) return 7;
    if (!strings_equal(conventions->thousands_sep, "")) return 8;
    if (!strings_equal(conventions->grouping, "")) return 9;
    if (!strings_equal(conventions->int_curr_symbol, "")) return 10;
    if (!strings_equal(conventions->currency_symbol, "")) return 11;
    if (!strings_equal(conventions->mon_decimal_point, "")) return 12;
    if (!strings_equal(conventions->mon_thousands_sep, "")) return 13;
    if (!strings_equal(conventions->mon_grouping, "")) return 14;
    if (!strings_equal(conventions->positive_sign, "")) return 15;
    if (!strings_equal(conventions->negative_sign, "")) return 16;
    if (conventions->int_frac_digits != CHAR_MAX) return 17;
    if (conventions->frac_digits != CHAR_MAX) return 18;
    if (conventions->p_cs_precedes != CHAR_MAX) return 19;
    if (conventions->p_sep_by_space != CHAR_MAX) return 20;
    if (conventions->n_cs_precedes != CHAR_MAX) return 21;
    if (conventions->n_sep_by_space != CHAR_MAX) return 22;
    if (conventions->p_sign_posn != CHAR_MAX) return 23;
    if (conventions->n_sign_posn != CHAR_MAX) return 24;
    return 0;
}
