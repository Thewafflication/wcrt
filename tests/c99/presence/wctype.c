/**
 * @file wctype.c
 * @brief Verifies the complete compile-time C99 wide-classification surface.
 */
#include <wctype.h>
#include <wctype.h>

typedef char wcrt_wctype_wint[sizeof(wint_t) == 4 ? 1 : -1];

static int (*wcrt_iswalnum)(wint_t) = iswalnum;
static int (*wcrt_iswalpha)(wint_t) = iswalpha;
static int (*wcrt_iswblank)(wint_t) = iswblank;
static int (*wcrt_iswcntrl)(wint_t) = iswcntrl;
static int (*wcrt_iswdigit)(wint_t) = iswdigit;
static int (*wcrt_iswgraph)(wint_t) = iswgraph;
static int (*wcrt_iswlower)(wint_t) = iswlower;
static int (*wcrt_iswprint)(wint_t) = iswprint;
static int (*wcrt_iswpunct)(wint_t) = iswpunct;
static int (*wcrt_iswspace)(wint_t) = iswspace;
static int (*wcrt_iswupper)(wint_t) = iswupper;
static int (*wcrt_iswxdigit)(wint_t) = iswxdigit;
static int (*wcrt_iswctype)(wint_t, wctype_t) = iswctype;
static wctype_t (*wcrt_wctype)(const char *) = wctype;
static wint_t (*wcrt_towlower)(wint_t) = towlower;
static wint_t (*wcrt_towupper)(wint_t) = towupper;
static wint_t (*wcrt_towctrans)(wint_t, wctrans_t) = towctrans;
static wctrans_t (*wcrt_wctrans)(const char *) = wctrans;
