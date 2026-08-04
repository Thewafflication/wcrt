/**
 * @file stdint.c
 * @brief Verifies the compile-time C99 integer interface.
 */
#include <stdint.h>
#include <stdint.h>

typedef char wcrt_int8_width[sizeof(int8_t) == 1 ? 1 : -1];
typedef char wcrt_uint8_width[sizeof(uint8_t) == 1 ? 1 : -1];
typedef char wcrt_int16_width[sizeof(int16_t) == 2 ? 1 : -1];
typedef char wcrt_uint16_width[sizeof(uint16_t) == 2 ? 1 : -1];
typedef char wcrt_int32_width[sizeof(int32_t) == 4 ? 1 : -1];
typedef char wcrt_uint32_width[sizeof(uint32_t) == 4 ? 1 : -1];
typedef char wcrt_int64_width[sizeof(int64_t) == 8 ? 1 : -1];
typedef char wcrt_uint64_width[sizeof(uint64_t) == 8 ? 1 : -1];
typedef char wcrt_pointer_width[
    sizeof(intptr_t) == sizeof(void *) ? 1 : -1];
typedef char wcrt_maximum_width[sizeof(intmax_t) >= sizeof(int64_t) ? 1 : -1];
typedef char wcrt_int8_signed[(int8_t)-1 < 0 ? 1 : -1];
typedef char wcrt_uint8_unsigned[(uint8_t)-1 > 0 ? 1 : -1];
typedef char wcrt_int32_constant_type[
    sizeof(INT32_C(1)) == sizeof(int) ? 1 : -1];
typedef char wcrt_uint32_constant_unsigned[
    UINT32_C(0) - UINT32_C(1) > UINT32_C(0) ? 1 : -1];
typedef char wcrt_int64_constant_type[
    sizeof(INT64_C(1)) == sizeof(long long) ? 1 : -1];
typedef char wcrt_uint64_constant_unsigned[
    UINT64_C(0) - UINT64_C(1) > UINT64_C(0) ? 1 : -1];
typedef char wcrt_size_limit[
    SIZE_MAX == (uintptr_t)-1 ? 1 : -1];

static int_least8_t wcrt_least_value = INT8_C(1);
static uint_fast32_t wcrt_fast_value = UINT32_C(1);
static intmax_t wcrt_max_value = INTMAX_C(1);
static uintmax_t wcrt_umax_value = UINTMAX_C(1);
