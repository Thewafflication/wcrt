/**
 * @file wchar.c
 * @brief Verifies the complete compile-time C99 wide-character interface.
 */
#include <wchar.h>
#include <wchar.h>
#include <inttypes.h>
#include <stdint.h>

typedef char wcrt_wchar_width[sizeof(wchar_t) == 2 ? 1 : -1];
typedef char wcrt_wint_width[sizeof(wint_t) == 4 ? 1 : -1];
typedef char wcrt_wint_unsigned[((wint_t)-1 > (wint_t)0) ? 1 : -1];
typedef char wcrt_mbstate_width[sizeof(mbstate_t) == 8 ? 1 : -1];
typedef char wcrt_weof_distinct[
    WEOF > (wint_t)(wchar_t)-1 ? 1 : -1];
typedef char wcrt_wint_limit[
    (WINT_MIN == 0 && WINT_MAX == UINT32_MAX) ? 1 : -1];
typedef char wcrt_wchar_limit[
    (WCHAR_MIN == 0 && WCHAR_MAX == UINT16_MAX) ? 1 : -1];

static int (*wcrt_fwprintf)(FILE *, const wchar_t *, ...) = fwprintf;
static int (*wcrt_wprintf)(const wchar_t *, ...) = wprintf;
static int (*wcrt_swprintf)(wchar_t *, size_t, const wchar_t *, ...) = swprintf;
static int (*wcrt_vfwprintf)(FILE *, const wchar_t *, va_list) = vfwprintf;
static int (*wcrt_vwprintf)(const wchar_t *, va_list) = vwprintf;
static int (*wcrt_vswprintf)(wchar_t *, size_t, const wchar_t *, va_list) =
    vswprintf;
static int (*wcrt_fwscanf)(FILE *, const wchar_t *, ...) = fwscanf;
static int (*wcrt_wscanf)(const wchar_t *, ...) = wscanf;
static int (*wcrt_swscanf)(const wchar_t *, const wchar_t *, ...) = swscanf;
static int (*wcrt_vfwscanf)(FILE *, const wchar_t *, va_list) = vfwscanf;
static int (*wcrt_vwscanf)(const wchar_t *, va_list) = vwscanf;
static int (*wcrt_vswscanf)(const wchar_t *, const wchar_t *, va_list) =
    vswscanf;

static wint_t (*wcrt_fgetwc)(FILE *) = fgetwc;
static wint_t (*wcrt_getwc)(FILE *) = getwc;
static wint_t (*wcrt_getwchar)(void) = getwchar;
static wint_t (*wcrt_fputwc)(wchar_t, FILE *) = fputwc;
static wint_t (*wcrt_putwc)(wchar_t, FILE *) = putwc;
static wint_t (*wcrt_putwchar)(wchar_t) = putwchar;
static wchar_t *(*wcrt_fgetws)(wchar_t *, int, FILE *) = fgetws;
static int (*wcrt_fputws)(const wchar_t *, FILE *) = fputws;
static wint_t (*wcrt_ungetwc)(wint_t, FILE *) = ungetwc;
static int (*wcrt_fwide)(FILE *, int) = fwide;

static double (*wcrt_wcstod)(const wchar_t *, wchar_t **) = wcstod;
static float (*wcrt_wcstof)(const wchar_t *, wchar_t **) = wcstof;
static long double (*wcrt_wcstold)(const wchar_t *, wchar_t **) = wcstold;
static long (*wcrt_wcstol)(const wchar_t *, wchar_t **, int) = wcstol;
static unsigned long (*wcrt_wcstoul)(const wchar_t *, wchar_t **, int) =
    wcstoul;
static long long (*wcrt_wcstoll)(const wchar_t *, wchar_t **, int) = wcstoll;
static unsigned long long (*wcrt_wcstoull)(const wchar_t *, wchar_t **, int) =
    wcstoull;
static intmax_t (*wcrt_wcstoimax)(const wchar_t *, wchar_t **, int) =
    wcstoimax;
static uintmax_t (*wcrt_wcstoumax)(const wchar_t *, wchar_t **, int) =
    wcstoumax;

static wchar_t *(*wcrt_wcscpy)(wchar_t *, const wchar_t *) = wcscpy;
static wchar_t *(*wcrt_wcsncpy)(wchar_t *, const wchar_t *, size_t) = wcsncpy;
static wchar_t *(*wcrt_wcscat)(wchar_t *, const wchar_t *) = wcscat;
static wchar_t *(*wcrt_wcsncat)(wchar_t *, const wchar_t *, size_t) = wcsncat;
static int (*wcrt_wcscmp)(const wchar_t *, const wchar_t *) = wcscmp;
static int (*wcrt_wcscoll)(const wchar_t *, const wchar_t *) = wcscoll;
static int (*wcrt_wcsncmp)(const wchar_t *, const wchar_t *, size_t) = wcsncmp;
static size_t (*wcrt_wcsxfrm)(wchar_t *, const wchar_t *, size_t) = wcsxfrm;
static wchar_t *(*wcrt_wcschr)(const wchar_t *, wchar_t) = wcschr;
static size_t (*wcrt_wcscspn)(const wchar_t *, const wchar_t *) = wcscspn;
static wchar_t *(*wcrt_wcspbrk)(const wchar_t *, const wchar_t *) = wcspbrk;
static wchar_t *(*wcrt_wcsrchr)(const wchar_t *, wchar_t) = wcsrchr;
static size_t (*wcrt_wcsspn)(const wchar_t *, const wchar_t *) = wcsspn;
static wchar_t *(*wcrt_wcsstr)(const wchar_t *, const wchar_t *) = wcsstr;
static wchar_t *(*wcrt_wcstok)(wchar_t *, const wchar_t *, wchar_t **) = wcstok;
static size_t (*wcrt_wcslen)(const wchar_t *) = wcslen;
static wchar_t *(*wcrt_wmemchr)(const wchar_t *, wchar_t, size_t) = wmemchr;
static int (*wcrt_wmemcmp)(const wchar_t *, const wchar_t *, size_t) = wmemcmp;
static wchar_t *(*wcrt_wmemcpy)(wchar_t *, const wchar_t *, size_t) = wmemcpy;
static wchar_t *(*wcrt_wmemmove)(wchar_t *, const wchar_t *, size_t) = wmemmove;
static wchar_t *(*wcrt_wmemset)(wchar_t *, wchar_t, size_t) = wmemset;

static size_t (*wcrt_wcsftime)(wchar_t *, size_t, const wchar_t *,
    const struct tm *) = wcsftime;
static wint_t (*wcrt_btowc)(int) = btowc;
static int (*wcrt_wctob)(wint_t) = wctob;
static int (*wcrt_mbsinit)(const mbstate_t *) = mbsinit;
static size_t (*wcrt_mbrlen)(const char *, size_t, mbstate_t *) = mbrlen;
static size_t (*wcrt_mbrtowc)(wchar_t *, const char *, size_t, mbstate_t *) =
    mbrtowc;
static size_t (*wcrt_wcrtomb)(char *, wchar_t, mbstate_t *) = wcrtomb;
static size_t (*wcrt_mbsrtowcs)(wchar_t *, const char **, size_t, mbstate_t *) =
    mbsrtowcs;
static size_t (*wcrt_wcsrtombs)(char *, const wchar_t **, size_t, mbstate_t *) =
    wcsrtombs;
