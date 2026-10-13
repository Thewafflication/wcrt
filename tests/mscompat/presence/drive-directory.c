/**
 * @file drive-directory.c
 * @brief Verifies Microsoft drive-directory declarations.
 */

#include <direct.h>
#include <direct.h>

static int (*change_address)(int) = _chdrive;
static char *(*query_address)(int, char *, int) = _getdcwd;

int main(void)
{
    return change_address == 0 || query_address == 0;
}
