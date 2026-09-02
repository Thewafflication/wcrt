/** @file mkdir.c @brief Verifies Microsoft directory creation behavior. */
#include <direct.h>
#include <errno.h>

int main(int argument_count, char **arguments)
{
    if (argument_count != 3) return 1;
    if (_mkdir(arguments[1]) != 0) return 2;
    errno = 0;
    if (_mkdir(arguments[1]) != -1 || errno != EEXIST) return 3;
    errno = 0;
    if (_mkdir(arguments[2]) != -1 || errno != ENOENT) return 4;
    errno = 0;
    if (_mkdir(0) != -1 || errno != EINVAL) return 5;
    return 0;
}
