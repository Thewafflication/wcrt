/** @file type-generic-macros.c @brief Probes TinyCC type-generic selection. */
#define WCRT_TYPE_KIND(value) \
    _Generic((value), float: 1, double: 2, long double: 3, default: 0)
typedef char wcrt_float_dispatch[WCRT_TYPE_KIND(1.0F) == 1 ? 1 : -1];
typedef char wcrt_double_dispatch[WCRT_TYPE_KIND(1.0) == 2 ? 1 : -1];
typedef char wcrt_long_double_dispatch[
    WCRT_TYPE_KIND(1.0L) == 3 ? 1 : -1];
typedef char wcrt_integer_dispatch[WCRT_TYPE_KIND(1) == 0 ? 1 : -1];
static const float wcrt_qualified_float = 1.0F;
typedef char wcrt_qualified_dispatch[
    WCRT_TYPE_KIND(wcrt_qualified_float) == 1 ? 1 : -1];

#define WCRT_SELECT(value) _Generic((value), \
    float: wcrt_float_target, \
    long double: wcrt_long_target, \
    default: wcrt_double_target)(value)

static int wcrt_count;
static float wcrt_float_target(float value) { return value; }
static double wcrt_double_target(double value) { return value; }
static long double wcrt_long_target(long double value) { return value; }
static double wcrt_value(void) { ++wcrt_count; return 1.0; }

int wcrt_type_generic_single_evaluation(void)
{
    (void)WCRT_SELECT(wcrt_value());
    return wcrt_count == 1 ? 0 : 1;
}
