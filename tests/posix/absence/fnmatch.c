/** @file fnmatch.c @brief Checks strict filename-match isolation. */
#include <fnmatch.h>
#include <fnmatch.h>
#ifdef FNM_NOMATCH
#error fnmatch constants leaked into the strict profile
#endif
static int fnmatch;
int main(void) { return fnmatch; }
