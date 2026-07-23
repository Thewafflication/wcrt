/**
 * @file locale.h
 * @brief Declares C89 localization selection and convention data.
 */

#ifndef WCRT_LOCALE_H
#define WCRT_LOCALE_H

#ifndef NULL
/** @brief Null pointer constant. */
#define NULL ((void *)0)
#endif

/** @name Locale category selectors */
/** @{ */
#define LC_ALL 0
#define LC_COLLATE 1
#define LC_CTYPE 2
#define LC_MONETARY 3
#define LC_NUMERIC 4
#define LC_TIME 5
/** @} */

/** @brief Numeric and monetary formatting conventions. */
struct lconv {
    char *decimal_point;       /**< Numeric decimal separator. */
    char *thousands_sep;       /**< Numeric thousands separator. */
    char *grouping;            /**< Numeric digit grouping. */
    char *int_curr_symbol;     /**< International currency symbol. */
    char *currency_symbol;     /**< Local currency symbol. */
    char *mon_decimal_point;   /**< Monetary decimal separator. */
    char *mon_thousands_sep;   /**< Monetary thousands separator. */
    char *mon_grouping;        /**< Monetary digit grouping. */
    char *positive_sign;       /**< Nonnegative monetary sign. */
    char *negative_sign;       /**< Negative monetary sign. */
    char int_frac_digits;      /**< International fractional digits. */
    char frac_digits;          /**< Local fractional digits. */
    char p_cs_precedes;        /**< Currency position for positive values. */
    char p_sep_by_space;       /**< Currency spacing for positive values. */
    char n_cs_precedes;        /**< Currency position for negative values. */
    char n_sep_by_space;       /**< Currency spacing for negative values. */
    char p_sign_posn;          /**< Sign position for positive values. */
    char n_sign_posn;          /**< Sign position for negative values. */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Selects or queries a locale category.
 * @param category Locale category selector.
 * @param locale Locale name, an empty string, or NULL for a query.
 * @return Selected locale name, or NULL for an invalid request.
 */
char *setlocale(int category, const char *locale);

/**
 * @brief Returns conventions for the active locale.
 * @return Pointer to static convention storage.
 */
struct lconv *localeconv(void);

#ifdef __cplusplus
}
#endif

#endif
