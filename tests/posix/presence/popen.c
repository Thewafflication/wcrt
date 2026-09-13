/** @file popen.c @brief Checks selected command-stream declarations. */
#define WCRT_POSIX 1
#include <stdio.h>
typedef FILE *(*popen_fn)(const char *, const char *);
typedef int (*pclose_fn)(FILE *);
static popen_fn popen_address = popen;
static pclose_fn pclose_address = pclose;
int main(void) { return popen_address == 0 || pclose_address == 0; }
