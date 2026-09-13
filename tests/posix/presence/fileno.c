/** @file fileno.c @brief Checks the selected POSIX fileno declaration. */
#define WCRT_POSIX 1
#include <stdio.h>
#include <stdio.h>

typedef int (*posix_fileno_fn)(FILE *);
static posix_fileno_fn posix_fileno_address = fileno;

int main(void)
{
    return posix_fileno_address == 0;
}
