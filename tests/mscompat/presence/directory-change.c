/**
 * @file directory-change.c
 * @brief Verifies Microsoft directory mutation declarations in C89.
 */

#include <direct.h>
#include <direct.h>

static int (*change_address)(const char *) = _chdir;
static int (*remove_address)(const char *) = _rmdir;

int main(void)
{
    return change_address == 0 || remove_address == 0;
}
