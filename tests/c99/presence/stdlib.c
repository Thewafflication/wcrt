/**
 * @file stdlib.c
 * @brief Verifies C99 general-utility declarations and expression types.
 */
#include <stdlib.h>
#include <stdlib.h>

typedef char wcrt_lldiv_quot_type[
    sizeof(((lldiv_t *)0)->quot) == sizeof(long long) ? 1 : -1];
typedef char wcrt_lldiv_rem_type[
    sizeof(((lldiv_t *)0)->rem) == sizeof(long long) ? 1 : -1];
typedef char wcrt_lldiv_layout[
    sizeof(lldiv_t) == 2 * sizeof(long long) ? 1 : -1];
typedef char wcrt_atoll_type[
    sizeof(atoll("1")) == sizeof(long long) ? 1 : -1];
typedef char wcrt_llabs_type[
    sizeof(llabs(1)) == sizeof(long long) ? 1 : -1];
typedef char wcrt_strtof_type[
    sizeof(strtof("1", (char **)0)) == sizeof(float) ? 1 : -1];
typedef char wcrt_strtold_type[
    sizeof(strtold("1", (char **)0)) == sizeof(long double) ? 1 : -1];
typedef char wcrt_strtoll_type[
    sizeof(strtoll("1", (char **)0, 10)) == sizeof(long long) ? 1 : -1];
typedef char wcrt_strtoull_type[
    sizeof(strtoull("1", (char **)0, 10)) ==
        sizeof(unsigned long long) ? 1 : -1];
