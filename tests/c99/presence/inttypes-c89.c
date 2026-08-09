/**
 * @file inttypes-c89.c
 * @brief Verifies that inttypes interfaces are absent in C89 mode.
 */
#include <inttypes.h>

#if defined(PRId8) || defined(PRIdMAX) || defined(PRIdPTR) || \
    defined(SCNd8) || defined(SCNdMAX) || defined(SCNdPTR)
#error C99 integer format macros must not be exposed in C89 mode
#endif

static int intmax_t;
static int uintmax_t;
static int imaxdiv_t;
static int imaxabs;
static int strtoimax;
static int wcstoimax;
static int wcstoumax;
