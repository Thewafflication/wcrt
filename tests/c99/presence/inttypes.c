/**
 * @file inttypes.c
 * @brief Verifies the compile-time C99 greatest-width integer interface.
 */
#include <inttypes.h>
#include <inttypes.h>

typedef char wcrt_imax_quot_type[
    sizeof(((imaxdiv_t *)0)->quot) == sizeof(intmax_t) ? 1 : -1];
typedef char wcrt_imax_rem_type[
    sizeof(((imaxdiv_t *)0)->rem) == sizeof(intmax_t) ? 1 : -1];
typedef char wcrt_imax_layout[
    sizeof(imaxdiv_t) == 2 * sizeof(intmax_t) ? 1 : -1];
typedef char wcrt_imaxabs_type[
    sizeof(imaxabs((intmax_t)1)) == sizeof(intmax_t) ? 1 : -1];
typedef char wcrt_strtoimax_type[
    sizeof(strtoimax("1", (char **)0, 10)) == sizeof(intmax_t) ? 1 : -1];
typedef char wcrt_strtoumax_type[
    sizeof(strtoumax("1", (char **)0, 10)) == sizeof(uintmax_t) ? 1 : -1];
typedef char wcrt_wcstoimax_type[
    sizeof(wcstoimax(L"1", (wchar_t **)0, 10)) == sizeof(intmax_t) ? 1 : -1];
typedef char wcrt_wcstoumax_type[
    sizeof(wcstoumax(L"1", (wchar_t **)0, 10)) == sizeof(uintmax_t) ? 1 : -1];

#define WCRT_FORMAT_SET(prefix) \
    prefix##8, prefix##16, prefix##32, prefix##64, \
    prefix##LEAST8, prefix##LEAST16, prefix##LEAST32, prefix##LEAST64, \
    prefix##FAST8, prefix##FAST16, prefix##FAST32, prefix##FAST64, \
    prefix##MAX, prefix##PTR

static const char *wcrt_formats[] = {
    WCRT_FORMAT_SET(PRId), WCRT_FORMAT_SET(PRIi),
    WCRT_FORMAT_SET(PRIo), WCRT_FORMAT_SET(PRIu),
    WCRT_FORMAT_SET(PRIx), WCRT_FORMAT_SET(PRIX),
    WCRT_FORMAT_SET(SCNd), WCRT_FORMAT_SET(SCNi),
    WCRT_FORMAT_SET(SCNo), WCRT_FORMAT_SET(SCNu), WCRT_FORMAT_SET(SCNx)
};
static const char wcrt_concatenated[] = "%" PRIdMAX;
