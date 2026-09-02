/**
 * @file string-transform.c
 * @brief Verifies Microsoft in-place string transformation behavior.
 */

#include <errno.h>
#include <string.h>

int main(void)
{
    char mixed[] = "AbC-123!";
    char odd[] = "abcde";
    char even[] = "abcd";
    char empty[] = "";
    char one[] = "x";

    if (_strlwr(mixed) != mixed || strcmp(mixed, "abc-123!") != 0) return 1;
    if (_strupr(mixed) != mixed || strcmp(mixed, "ABC-123!") != 0) return 2;
    if (_strrev(mixed) != mixed || strcmp(mixed, "!321-CBA") != 0) return 3;
    if (_strrev(odd) != odd || strcmp(odd, "edcba") != 0) return 4;
    if (_strrev(even) != even || strcmp(even, "dcba") != 0) return 5;
    if (_strlwr(empty) != empty || empty[0] != '\0') return 6;
    if (_strrev(one) != one || strcmp(one, "x") != 0) return 7;
    errno = 0;
    if (_strlwr(0) != 0 || errno != EINVAL) return 8;
    errno = 0;
    if (_strupr(0) != 0 || errno != EINVAL) return 9;
    errno = 0;
    if (_strrev(0) != 0 || errno != EINVAL) return 10;
    return 0;
}
