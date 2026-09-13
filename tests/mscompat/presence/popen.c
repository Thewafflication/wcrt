/** @file popen.c @brief Checks Microsoft command-stream declarations. */
#include <stdio.h>
typedef FILE *(*popen_fn)(const char *, const char *);
typedef int (*pclose_fn)(FILE *);
static popen_fn popen_address = _popen;
static pclose_fn pclose_address = _pclose;
int main(void) { return popen_address == 0 || pclose_address == 0; }
