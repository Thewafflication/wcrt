/** @file getopt.c @brief Checks selected option parser declarations. */
#define WCRT_POSIX 1
#include <getopt.h>
#include <getopt.h>
typedef int (*getopt_fn)(int, char *const [], const char *);
typedef int (*getopt_long_fn)(int, char *const [], const char *,
    const struct option *, int *);
static getopt_fn getopt_address = getopt;
static getopt_long_fn getopt_long_address = getopt_long;
int main(void)
{
    return getopt_address == 0 || getopt_long_address == 0 || optind < 0;
}
