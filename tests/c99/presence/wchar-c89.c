/**
 * @file wchar-c89.c
 * @brief Verifies that the C99 wide library is absent in selected C89 mode.
 */
#include <wchar.h>
#include <inttypes.h>
#include <stdint.h>

#if defined(WEOF) || defined(WINT_MIN) || defined(WINT_MAX)
#error C99 wide constants must not be exposed in C89 mode
#endif

static int wint_t;
static int mbstate_t;
static int fwprintf;
static int fwide;
static int wcstod;
static int wcscpy;
static int wcsftime;
static int mbrtowc;
static int wcstoimax;
