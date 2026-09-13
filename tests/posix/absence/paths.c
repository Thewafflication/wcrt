/** @file paths.c @brief Checks strict isolation of POSIX path names. */
#include <sys/stat.h>
#include <limits.h>
#include <sys/param.h>
#include <unistd.h>

#ifndef MAX_PATH
#error The Microsoft path limit is missing
#endif
#ifdef PATH_MAX
#error PATH_MAX leaked into the strict profile
#endif
#ifdef MAXPATHLEN
#error MAXPATHLEN leaked into the strict profile
#endif
#include <sys/stat.h>
#include <unistd.h>

static int access;
static int mkdir;

int main(void)
{
    return access + mkdir;
}
