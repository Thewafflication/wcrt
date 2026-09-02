/**
 * @file string-set.c
 * @brief Verifies Microsoft in-place string-set behavior.
 */

#include <errno.h>
#include <string.h>

int main(void)
{
    char full[7] = "abcde";
    char bounded[7] = "abcde";
    char empty[2] = "";
    if (_strset(full, 'x') != full || strcmp(full, "xxxxx") != 0 ||
        full[6] != 0) return 1;
    if (_strnset(bounded, 'y', 0) != bounded ||
        strcmp(bounded, "abcde") != 0) return 2;
    if (_strnset(bounded, 'y', 2) != bounded ||
        strcmp(bounded, "yycde") != 0) return 3;
    if (_strnset(bounded, 'z', 5) != bounded ||
        strcmp(bounded, "zzzzz") != 0) return 4;
    if (_strnset(bounded, 'q', 20) != bounded ||
        strcmp(bounded, "qqqqq") != 0) return 5;
    if (_strset(empty, 'x') != empty || empty[0] != '\0' ||
        empty[1] != '\0') return 6;
    errno = 0;
    if (_strset(0, 'x') != 0 || errno != EINVAL) return 7;
    errno = 0;
    if (_strnset(0, 'x', 0) != 0 || errno != EINVAL) return 8;
    return 0;
}
