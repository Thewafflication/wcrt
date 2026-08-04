/** @file long-long.c @brief Probes C99 long-long parsing and arithmetic. */
typedef char wcrt_long_long_size[sizeof(long long) == 8 ? 1 : -1];
static unsigned long long wcrt_long_long_value = 18446744073709551615ULL;

