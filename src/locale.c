/**
 * @file locale.c
 * @brief Implements the mandatory C locale and its conventions.
 */

#include <limits.h>
#include <locale.h>

#include "internal/locale.h"

/** @brief Empty convention string shared by C-locale fields. */
static char wcrt_empty[] = "";

/** @brief C-locale decimal-point string. */
static char wcrt_decimal_point[] = ".";

/** @brief Static C-locale convention record. */
static struct lconv wcrt_c_conventions = {
    wcrt_decimal_point,
    wcrt_empty,
    wcrt_empty,
    wcrt_empty,
    wcrt_empty,
    wcrt_empty,
    wcrt_empty,
    wcrt_empty,
    wcrt_empty,
    wcrt_empty,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX,
    CHAR_MAX
};

/** @brief Implements the internal category validation contract. */
int __wcrt_locale_category_valid(int category)
{
    return category >= LC_ALL && category <= LC_TIME;
}

/** @brief Implements the internal locale-name validation contract. */
int __wcrt_locale_name_supported(const char *locale)
{
    return locale[0] == '\0' ||
        (locale[0] == 'C' && locale[1] == '\0');
}

/** @brief Implements the public setlocale contract. */
char *setlocale(int category, const char *locale)
{
    static char c_locale[] = "C";

    if (!__wcrt_locale_category_valid(category)) {
        return NULL;
    }
    if (locale == NULL) {
        return c_locale;
    }
    if (!__wcrt_locale_name_supported(locale)) {
        return NULL;
    }
    return c_locale;
}

/** @brief Implements the public localeconv contract. */
struct lconv *localeconv(void)
{
    return &wcrt_c_conventions;
}
