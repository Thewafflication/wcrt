/** @file variadic-macros.c @brief Probes C99 variadic preprocessing. */
#define WCRT_VARIADIC_SUM(first, ...) ((first) + (__VA_ARGS__))
typedef char wcrt_variadic_macro[
    WCRT_VARIADIC_SUM(1, 2) == 3 ? 1 : -1];

