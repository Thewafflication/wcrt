/**
 * @file stdlib.h
 * @brief Declares C89 general utility facilities.
 */

#ifndef WCRT_STDLIB_H
#define WCRT_STDLIB_H

#include <stddef.h>
#include <wcrt/restrict.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define RAND_MAX 32767
#define MB_CUR_MAX ((size_t)1)

/** @brief Quotient and remainder from integer division. */
typedef struct { int quot; int rem; } div_t;
/** @brief Quotient and remainder from long integer division. */
typedef struct { long quot; long rem; } ldiv_t;

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
/** @brief Quotient and remainder from long-long integer division. */
typedef struct { long long quot; long long rem; } lldiv_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

double atof(const char *string);
int atoi(const char *string);
long atol(const char *string);
double strtod(const char *WCRT_RESTRICT string,
    char **WCRT_RESTRICT end_pointer);
long strtol(const char *WCRT_RESTRICT string,
    char **WCRT_RESTRICT end_pointer, int base);
unsigned long strtoul(const char *WCRT_RESTRICT string,
    char **WCRT_RESTRICT end_pointer, int base);
int rand(void);
void srand(unsigned int seed);
void *calloc(size_t count, size_t size);
void free(void *memory);
void *malloc(size_t size);
void *realloc(void *memory, size_t size);
void abort(void);
int atexit(void (*function)(void));
void exit(int status);
char *getenv(const char *name);
int system(const char *command);
void *bsearch(const void *key, const void *base, size_t count, size_t size,
    int (*compare)(const void *, const void *));
void qsort(void *base, size_t count, size_t size,
    int (*compare)(const void *, const void *));
int abs(int value);
long labs(long value);
div_t div(int numerator, int denominator);
ldiv_t ldiv(long numerator, long denominator);
int mblen(const char *string, size_t count);
int mbtowc(wchar_t *WCRT_RESTRICT wide,
    const char *WCRT_RESTRICT string, size_t count);
int wctomb(char *string, wchar_t wide);
size_t mbstowcs(wchar_t *WCRT_RESTRICT destination,
    const char *WCRT_RESTRICT source, size_t count);
size_t wcstombs(char *WCRT_RESTRICT destination,
    const wchar_t *WCRT_RESTRICT source, size_t count);

#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
void _Exit(int status);
long long atoll(const char *string);
long long llabs(long long value);
lldiv_t lldiv(long long numerator, long long denominator);
float strtof(const char *WCRT_RESTRICT string,
    char **WCRT_RESTRICT end_pointer);
long double strtold(const char *WCRT_RESTRICT string,
    char **WCRT_RESTRICT end_pointer);
long long strtoll(const char *WCRT_RESTRICT string,
    char **WCRT_RESTRICT end_pointer, int base);
unsigned long long strtoull(const char *WCRT_RESTRICT string,
    char **WCRT_RESTRICT end_pointer, int base);
#endif

#ifdef __cplusplus
}
#endif

#endif
