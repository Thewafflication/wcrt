/** @file secure-string.c @brief Tests Microsoft secure string copies. */
#include <errno.h>
#include <string.h>

/** @brief Runs secure string-copy checks. @return Zero on success. */
int main(void)
{
    char buffer[8];
    memset(buffer, 'X', sizeof(buffer));
    if (strcpy_s(buffer, sizeof(buffer), "abc") != 0) return 1;
    if (memcmp(buffer, "abc\0X", 5) != 0) return 2;
    if (strncpy_s(buffer, sizeof(buffer), "abcdef", 3) != 0) return 3;
    if (strcmp(buffer, "abc") != 0) return 4;
    memset(buffer, 'X', sizeof(buffer));
    if (strncpy_s(buffer, 4, "abcdef", _TRUNCATE) != STRUNCATE) return 5;
    if (memcmp(buffer, "abc\0", 4) != 0) return 6;
    buffer[0] = 'X';
    if (strcpy_s(buffer, 2, "abc") != ERANGE || buffer[0] != 0) return 7;
    buffer[0] = 'X';
    if (strcpy_s(buffer, sizeof(buffer), NULL) != EINVAL ||
        buffer[0] != 0) return 8;
    return 0;
}
