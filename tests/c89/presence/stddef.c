/* REQ-0011: <stddef.h> presence. Compile only; do not link. */
#include <stddef.h>

#ifndef NULL
#error "NULL must be defined"
#endif
#ifndef offsetof
#error "offsetof must be defined"
#endif

struct wcrt_offset_probe {
    char first;
    int second;
    long third;
};

static ptrdiff_t wcrt_pointer_difference;
static size_t wcrt_size_value;
static wchar_t wcrt_wide_character;

enum {
    wcrt_first_offset = offsetof(struct wcrt_offset_probe, first),
    wcrt_second_offset = offsetof(struct wcrt_offset_probe, second),
    wcrt_third_offset = offsetof(struct wcrt_offset_probe, third)
};

static void *wcrt_null_pointer = NULL;
