/**
 * @file wctype-c89.c
 * @brief Verifies that wide classification is absent in selected C89 mode.
 */
#include <wctype.h>

#if defined(WEOF)
#error C99 wide constants must not be exposed in C89 mode
#endif

static int wint_t;
static int wctype_t;
static int wctrans_t;
static int iswalnum;
static int wctype;
static int towlower;
static int wctrans;
