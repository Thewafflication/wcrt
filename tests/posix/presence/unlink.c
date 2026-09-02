/** @file unlink.c @brief Checks the selected POSIX unlink declaration. */
#define WCRT_POSIX 1
#include <unistd.h>
#include <unistd.h>

/** @brief Selected unlink signature. */
typedef int (*posix_unlink_fn)(const char *);
/** @brief Forces type checking of unlink. */
static posix_unlink_fn posix_unlink_address = unlink;

int main(void)
{
    return posix_unlink_address == 0;
}
