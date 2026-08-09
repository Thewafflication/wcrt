/**
 * @file wchar.h
 * @brief Declares C99 wide-character utilities.
 */

#ifndef WCRT_WCHAR_H
#define WCRT_WCHAR_H

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <wcrt/restrict.h>

#ifndef WCRT_WINT_T_DEFINED
#define WCRT_WINT_T_DEFINED
/** @brief Promotion-safe type containing wide characters or WEOF. */
typedef unsigned int wint_t;
#endif

#ifndef WCRT_MBSTATE_T_DEFINED
#define WCRT_MBSTATE_T_DEFINED
/** @brief Target-invariant restartable multibyte conversion state. */
typedef struct {
    unsigned long __value;
    unsigned short __bytes;
    unsigned short __state;
} mbstate_t;
#endif

#define WEOF ((wint_t)WINT_MAX)

#ifdef __cplusplus
extern "C" {
#endif

int fwprintf(FILE *WCRT_RESTRICT stream,
    const wchar_t *WCRT_RESTRICT format, ...);
int wprintf(const wchar_t *WCRT_RESTRICT format, ...);
int swprintf(wchar_t *WCRT_RESTRICT destination, size_t capacity,
    const wchar_t *WCRT_RESTRICT format, ...);
int vfwprintf(FILE *WCRT_RESTRICT stream,
    const wchar_t *WCRT_RESTRICT format, va_list arguments);
int vwprintf(const wchar_t *WCRT_RESTRICT format, va_list arguments);
int vswprintf(wchar_t *WCRT_RESTRICT destination, size_t capacity,
    const wchar_t *WCRT_RESTRICT format, va_list arguments);

int fwscanf(FILE *WCRT_RESTRICT stream,
    const wchar_t *WCRT_RESTRICT format, ...);
int wscanf(const wchar_t *WCRT_RESTRICT format, ...);
int swscanf(const wchar_t *WCRT_RESTRICT source,
    const wchar_t *WCRT_RESTRICT format, ...);
int vfwscanf(FILE *WCRT_RESTRICT stream,
    const wchar_t *WCRT_RESTRICT format, va_list arguments);
int vwscanf(const wchar_t *WCRT_RESTRICT format, va_list arguments);
int vswscanf(const wchar_t *WCRT_RESTRICT source,
    const wchar_t *WCRT_RESTRICT format, va_list arguments);

wint_t fgetwc(FILE *stream);
wint_t getwc(FILE *stream);
wint_t getwchar(void);
wint_t fputwc(wchar_t character, FILE *stream);
wint_t putwc(wchar_t character, FILE *stream);
wint_t putwchar(wchar_t character);
wchar_t *fgetws(wchar_t *WCRT_RESTRICT destination, int capacity,
    FILE *WCRT_RESTRICT stream);
int fputws(const wchar_t *WCRT_RESTRICT string,
    FILE *WCRT_RESTRICT stream);
wint_t ungetwc(wint_t character, FILE *stream);
int fwide(FILE *stream, int mode);

double wcstod(const wchar_t *WCRT_RESTRICT string,
    wchar_t **WCRT_RESTRICT end_pointer);
float wcstof(const wchar_t *WCRT_RESTRICT string,
    wchar_t **WCRT_RESTRICT end_pointer);
long double wcstold(const wchar_t *WCRT_RESTRICT string,
    wchar_t **WCRT_RESTRICT end_pointer);
long wcstol(const wchar_t *WCRT_RESTRICT string,
    wchar_t **WCRT_RESTRICT end_pointer, int base);
unsigned long wcstoul(const wchar_t *WCRT_RESTRICT string,
    wchar_t **WCRT_RESTRICT end_pointer, int base);
long long wcstoll(const wchar_t *WCRT_RESTRICT string,
    wchar_t **WCRT_RESTRICT end_pointer, int base);
unsigned long long wcstoull(const wchar_t *WCRT_RESTRICT string,
    wchar_t **WCRT_RESTRICT end_pointer, int base);

wchar_t *wcscpy(wchar_t *WCRT_RESTRICT destination,
    const wchar_t *WCRT_RESTRICT source);
wchar_t *wcsncpy(wchar_t *WCRT_RESTRICT destination,
    const wchar_t *WCRT_RESTRICT source, size_t count);
wchar_t *wcscat(wchar_t *WCRT_RESTRICT destination,
    const wchar_t *WCRT_RESTRICT source);
wchar_t *wcsncat(wchar_t *WCRT_RESTRICT destination,
    const wchar_t *WCRT_RESTRICT source, size_t count);
int wcscmp(const wchar_t *left, const wchar_t *right);
int wcscoll(const wchar_t *left, const wchar_t *right);
int wcsncmp(const wchar_t *left, const wchar_t *right, size_t count);
size_t wcsxfrm(wchar_t *WCRT_RESTRICT destination,
    const wchar_t *WCRT_RESTRICT source, size_t count);
wchar_t *wcschr(const wchar_t *string, wchar_t character);
size_t wcscspn(const wchar_t *string, const wchar_t *reject);
wchar_t *wcspbrk(const wchar_t *string, const wchar_t *accept);
wchar_t *wcsrchr(const wchar_t *string, wchar_t character);
size_t wcsspn(const wchar_t *string, const wchar_t *accept);
wchar_t *wcsstr(const wchar_t *haystack, const wchar_t *needle);
wchar_t *wcstok(wchar_t *WCRT_RESTRICT string,
    const wchar_t *WCRT_RESTRICT separators,
    wchar_t **WCRT_RESTRICT state);
size_t wcslen(const wchar_t *string);
wchar_t *wmemchr(const wchar_t *memory, wchar_t character, size_t count);
int wmemcmp(const wchar_t *left, const wchar_t *right, size_t count);
wchar_t *wmemcpy(wchar_t *WCRT_RESTRICT destination,
    const wchar_t *WCRT_RESTRICT source, size_t count);
wchar_t *wmemmove(wchar_t *destination, const wchar_t *source, size_t count);
wchar_t *wmemset(wchar_t *destination, wchar_t character, size_t count);

size_t wcsftime(wchar_t *WCRT_RESTRICT destination, size_t capacity,
    const wchar_t *WCRT_RESTRICT format,
    const struct tm *WCRT_RESTRICT broken_down);

wint_t btowc(int character);
int wctob(wint_t character);
int mbsinit(const mbstate_t *state);
size_t mbrlen(const char *WCRT_RESTRICT source, size_t count,
    mbstate_t *WCRT_RESTRICT state);
size_t mbrtowc(wchar_t *WCRT_RESTRICT destination,
    const char *WCRT_RESTRICT source, size_t count,
    mbstate_t *WCRT_RESTRICT state);
size_t wcrtomb(char *WCRT_RESTRICT destination, wchar_t character,
    mbstate_t *WCRT_RESTRICT state);
size_t mbsrtowcs(wchar_t *WCRT_RESTRICT destination,
    const char **WCRT_RESTRICT source, size_t count,
    mbstate_t *WCRT_RESTRICT state);
size_t wcsrtombs(char *WCRT_RESTRICT destination,
    const wchar_t **WCRT_RESTRICT source, size_t count,
    mbstate_t *WCRT_RESTRICT state);

#ifdef __cplusplus
}
#endif

#endif

#endif
