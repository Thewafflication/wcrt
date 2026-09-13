/** @file getopt.c @brief Checks strict option parser isolation. */
#include <getopt.h>
#include <getopt.h>
#ifdef no_argument
#error getopt constants leaked into the strict profile
#endif
static int getopt;
static int getopt_long;
int main(void) { return getopt + getopt_long; }
