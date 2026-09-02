/** @file strings.c @brief Verifies selected POSIX string behavior. */
#define WCRT_POSIX 1
#include <strings.h>

int main(void)
{
    if (strcasecmp("Wcrt", "wCRT") != 0) return 1;
    if (strcasecmp("abc", "abd") >= 0) return 2;
    if (strcasecmp("abe", "abd") <= 0) return 3;
    if (strncasecmp("prefix-A", "PREFIX-b", 7) != 0) return 4;
    if (strncasecmp("abc", "ABd", 3) >= 0) return 5;
    if (strncasecmp(0, 0, 0) != 0) return 6;
    return 0;
}
