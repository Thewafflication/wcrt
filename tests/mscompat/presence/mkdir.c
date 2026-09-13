/** @file mkdir.c @brief Verifies the Microsoft mkdir declaration. */
#include <direct.h>
#include <stdlib.h>

typedef int (*mkdir_fn)(const char *);
static mkdir_fn mkdir_address = _mkdir;
typedef char *(*getcwd_fn)(char *, int);
typedef char *(*fullpath_fn)(char *, const char *, size_t);
static getcwd_fn getcwd_address = _getcwd;
static fullpath_fn fullpath_address = _fullpath;

int main(void)
{
    return mkdir_address == 0 || getcwd_address == 0 ||
        fullpath_address == 0;
}
