/**
 * @file memory-case.c
 * @brief Verifies Microsoft case-insensitive memory comparison.
 */

#include <errno.h>
#include <string.h>

int main(void)
{
    const unsigned char left[] = { 'A', 0, 'Z', 0x80 };
    const unsigned char equal[] = { 'a', 0, 'z', 0x80 };
    const unsigned char lower[] = { 'a', 0, 'y', 0x80 };

    if (_memicmp(left, equal, sizeof(left)) != 0) return 1;
    if (_memicmp(left, lower, sizeof(left)) <= 0) return 2;
    if (_memicmp(lower, left, sizeof(left)) >= 0) return 3;
    if (_memicmp(left, lower, 2) != 0) return 4;
    errno = 0;
    if (_memicmp(0, 0, 0) != 0 || errno != 0) return 5;
    errno = 0;
    if (_memicmp(0, equal, 1) != _NLSCMPERROR || errno != EINVAL)
        return 6;
    errno = 0;
    if (_memicmp(left, 0, 1) != _NLSCMPERROR || errno != EINVAL)
        return 7;
    return 0;
}
