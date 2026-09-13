/** @file paths.c @brief Checks selected POSIX path declarations. */
#define WCRT_POSIX 1
#include <limits.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

#if MAX_PATH != 260 || PATH_MAX != MAX_PATH || MAXPATHLEN != PATH_MAX
#error Selected Windows and POSIX path limits are inconsistent
#endif
#include <sys/stat.h>
#include <unistd.h>

/** @brief Selected path-access signature. */
typedef int (*posix_access_fn)(const char *, int);
/** @brief Selected directory-creation signature. */
typedef int (*posix_mkdir_fn)(const char *, mode_t);
typedef char *(*posix_getcwd_fn)(char *, size_t);
typedef char *(*posix_realpath_fn)(const char *, char *);

static posix_access_fn posix_access_address = access;
static posix_mkdir_fn posix_mkdir_address = mkdir;
static posix_getcwd_fn posix_getcwd_address = getcwd;
static posix_realpath_fn posix_realpath_address = realpath;

int main(void)
{
    return posix_access_address == 0 || posix_mkdir_address == 0 ||
        posix_getcwd_address == 0 || posix_realpath_address == 0;
}
