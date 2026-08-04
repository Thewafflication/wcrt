/** @file type-generic-macros.c @brief Probes TinyCC type-generic selection. */
#define WCRT_TYPE_KIND(value) \
    _Generic((value), float: 1, double: 2, long double: 3, default: 0)
typedef char wcrt_float_dispatch[WCRT_TYPE_KIND(1.0F) == 1 ? 1 : -1];
typedef char wcrt_double_dispatch[WCRT_TYPE_KIND(1.0) == 2 ? 1 : -1];

