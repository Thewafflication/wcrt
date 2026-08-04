/** @file string-case.c @brief Tests Microsoft string comparisons. */
#include <errno.h>
#include <string.h>

/** @brief Runs case-insensitive comparison checks. @return Zero on success. */
int main(void)
{
    if (_stricmp("Wcrt", "wCRT") != 0) return 1;
    if (_stricmp("abc", "abd") >= 0) return 2;
    if (_stricmp("abe", "abd") <= 0) return 3;
    if (_strnicmp("prefix-A", "PREFIX-b", 7) != 0) return 4;
    if (_strnicmp("a", "B", 0) != 0) return 5;
    if (_strnicmp("abc", "ABd", 3) >= 0) return 6;
    errno = 0;
    if (_stricmp(NULL, "x") != _NLSCMPERROR || errno != EINVAL) return 7;
    return 0;
}
