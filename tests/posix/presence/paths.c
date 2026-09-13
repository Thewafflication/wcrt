/** @file paths.c @brief Checks selected POSIX path declarations. */
#define WCRT_POSIX 1
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>

/** @brief Selected path-access signature. */
typedef int (*posix_access_fn)(const char *, int);
/** @brief Selected directory-creation signature. */
typedef int (*posix_mkdir_fn)(const char *, mode_t);

static posix_access_fn posix_access_address = access;
static posix_mkdir_fn posix_mkdir_address = mkdir;

int main(void)
{
    return posix_access_address == 0 || posix_mkdir_address == 0;
}
