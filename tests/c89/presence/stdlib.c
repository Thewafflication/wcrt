/* REQ-0013: <stdlib.h> presence. Compile only; do not link. */
#include <stdlib.h>

#ifndef NULL
#error "NULL must be defined"
#endif
#ifndef EXIT_FAILURE
#error "EXIT_FAILURE must be defined"
#endif
#ifndef EXIT_SUCCESS
#error "EXIT_SUCCESS must be defined"
#endif
#ifndef RAND_MAX
#error "RAND_MAX must be defined"
#endif
#ifndef MB_CUR_MAX
#error "MB_CUR_MAX must be defined"
#endif

typedef double (*wcrt_atof_fn)(const char *);
typedef int (*wcrt_atoi_fn)(const char *);
typedef long (*wcrt_atol_fn)(const char *);
typedef double (*wcrt_strtod_fn)(const char *, char **);
typedef long (*wcrt_strtol_fn)(const char *, char **, int);
typedef unsigned long (*wcrt_strtoul_fn)(const char *, char **, int);
typedef int (*wcrt_rand_fn)(void);
typedef void (*wcrt_srand_fn)(unsigned int);
typedef void *(*wcrt_calloc_fn)(size_t, size_t);
typedef void (*wcrt_free_fn)(void *);
typedef void *(*wcrt_malloc_fn)(size_t);
typedef void *(*wcrt_realloc_fn)(void *, size_t);
typedef void (*wcrt_void_fn)(void);
typedef int (*wcrt_atexit_fn)(void (*)(void));
typedef void (*wcrt_exit_fn)(int);
typedef char *(*wcrt_getenv_fn)(const char *);
typedef int (*wcrt_system_fn)(const char *);
typedef void *(*wcrt_bsearch_fn)(const void *, const void *, size_t, size_t,
                                 int (*)(const void *, const void *));
typedef void (*wcrt_qsort_fn)(void *, size_t, size_t,
                              int (*)(const void *, const void *));
typedef int (*wcrt_abs_fn)(int);
typedef long (*wcrt_labs_fn)(long);
typedef div_t (*wcrt_div_fn)(int, int);
typedef ldiv_t (*wcrt_ldiv_fn)(long, long);
typedef int (*wcrt_mblen_fn)(const char *, size_t);
typedef int (*wcrt_mbtowc_fn)(wchar_t *, const char *, size_t);
typedef int (*wcrt_wctomb_fn)(char *, wchar_t);
typedef size_t (*wcrt_mbstowcs_fn)(wchar_t *, const char *, size_t);
typedef size_t (*wcrt_wcstombs_fn)(char *, const wchar_t *, size_t);

static wcrt_atof_fn wcrt_atof_address = (atof);
static wcrt_atoi_fn wcrt_atoi_address = (atoi);
static wcrt_atol_fn wcrt_atol_address = (atol);
static wcrt_strtod_fn wcrt_strtod_address = (strtod);
static wcrt_strtol_fn wcrt_strtol_address = (strtol);
static wcrt_strtoul_fn wcrt_strtoul_address = (strtoul);
static wcrt_rand_fn wcrt_rand_address = (rand);
static wcrt_srand_fn wcrt_srand_address = (srand);
static wcrt_calloc_fn wcrt_calloc_address = (calloc);
static wcrt_free_fn wcrt_free_address = (free);
static wcrt_malloc_fn wcrt_malloc_address = (malloc);
static wcrt_realloc_fn wcrt_realloc_address = (realloc);
static wcrt_void_fn wcrt_abort_address = (abort);
static wcrt_atexit_fn wcrt_atexit_address = (atexit);
static wcrt_exit_fn wcrt_exit_address = (exit);
static wcrt_getenv_fn wcrt_getenv_address = (getenv);
static wcrt_system_fn wcrt_system_address = (system);
static wcrt_bsearch_fn wcrt_bsearch_address = (bsearch);
static wcrt_qsort_fn wcrt_qsort_address = (qsort);
static wcrt_abs_fn wcrt_abs_address = (abs);
static wcrt_labs_fn wcrt_labs_address = (labs);
static wcrt_div_fn wcrt_div_address = (div);
static wcrt_ldiv_fn wcrt_ldiv_address = (ldiv);
static wcrt_mblen_fn wcrt_mblen_address = (mblen);
static wcrt_mbtowc_fn wcrt_mbtowc_address = (mbtowc);
static wcrt_wctomb_fn wcrt_wctomb_address = (wctomb);
static wcrt_mbstowcs_fn wcrt_mbstowcs_address = (mbstowcs);
static wcrt_wcstombs_fn wcrt_wcstombs_address = (wcstombs);

static div_t wcrt_div_value;
static ldiv_t wcrt_ldiv_value;
static size_t wcrt_stdlib_size;
static wchar_t wcrt_stdlib_wchar;

static void wcrt_use_stdlib_macros(void)
{
    size_t values[] = {
        (size_t)EXIT_FAILURE, (size_t)EXIT_SUCCESS,
        (size_t)RAND_MAX, (size_t)MB_CUR_MAX
    };
    (void)values;
}
