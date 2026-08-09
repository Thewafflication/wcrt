/**
 * @file wctype.h
 * @brief Declares C99 wide-character classification and mapping.
 */

#ifndef WCRT_WCTYPE_H
#define WCRT_WCTYPE_H

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L

#include <wchar.h>

/** @brief Stable descriptor for a wide-character class. */
typedef unsigned short wctype_t;
/** @brief Stable descriptor for a wide-character mapping. */
typedef unsigned short wctrans_t;

#ifdef __cplusplus
extern "C" {
#endif

int iswalnum(wint_t character);
int iswalpha(wint_t character);
int iswblank(wint_t character);
int iswcntrl(wint_t character);
int iswdigit(wint_t character);
int iswgraph(wint_t character);
int iswlower(wint_t character);
int iswprint(wint_t character);
int iswpunct(wint_t character);
int iswspace(wint_t character);
int iswupper(wint_t character);
int iswxdigit(wint_t character);
int iswctype(wint_t character, wctype_t descriptor);
wctype_t wctype(const char *name);
wint_t towlower(wint_t character);
wint_t towupper(wint_t character);
wint_t towctrans(wint_t character, wctrans_t descriptor);
wctrans_t wctrans(const char *name);

#ifdef __cplusplus
}
#endif

#endif

#endif
