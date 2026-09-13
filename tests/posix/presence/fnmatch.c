/** @file fnmatch.c @brief Checks selected filename-match declarations. */
#define WCRT_POSIX 1
#include <fnmatch.h>
#include <fnmatch.h>

typedef int (*posix_fnmatch_fn)(const char *, const char *, int);
static posix_fnmatch_fn posix_fnmatch_address = fnmatch;
int main(void) { return posix_fnmatch_address == 0; }
