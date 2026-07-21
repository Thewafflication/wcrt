/* REQ-0003: <errno.h> presence. Compile only; do not link. */
#include <errno.h>

#ifndef errno
#error "errno must be defined"
#endif
#ifndef EDOM
#error "EDOM must be defined"
#endif
#ifndef ERANGE
#error "ERANGE must be defined"
#endif

enum {
    wcrt_edom_is_positive = 1 / (EDOM > 0),
    wcrt_erange_is_positive = 1 / (ERANGE > 0),
    wcrt_error_values_are_distinct = 1 / (EDOM != ERANGE)
};

static void wcrt_errno_is_modifiable(void)
{
    int value = errno;
    errno = value;
}
