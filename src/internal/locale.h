/**
 * @file locale.h
 * @brief Declares private helpers for the mandatory C locale.
 */

#ifndef WCRT_INTERNAL_LOCALE_H
#define WCRT_INTERNAL_LOCALE_H

/**
 * @brief Tests whether a locale category selector is valid.
 * @param category Category value to test.
 * @return Nonzero for a valid C89 category.
 */
int __wcrt_locale_category_valid(int category);

/**
 * @brief Tests whether a locale name selects the supported C locale.
 * @param locale Non-null locale name.
 * @return Nonzero for `C` or the implementation-native empty name.
 */
int __wcrt_locale_name_supported(const char *locale);

#endif
