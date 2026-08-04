/** @file wpm-snprintf.c @brief Tests WPM bounded-formatting needs. */
#include <stdio.h>
#include <string.h>

/** @brief Runs WPM formatting checks. @return Zero on success. */
int main(void)
{
    char buffer[32];
    int result;
    result = snprintf(buffer, sizeof(buffer), "%s-%c-%d-%u-%x",
        "wpm", 'A', -12, 34U, 0x2aU);
    if (result != 15) return 1;
    if (strcmp(buffer, "wpm-A--12-34-2a") != 0) return 2;
    memset(buffer, 'X', sizeof(buffer));
    result = snprintf(buffer, 4, "%s", "abcdef");
    if (result != 6 || memcmp(buffer, "abc\0", 4) != 0) return 3;
    if (snprintf(NULL, 0, "%llu", 42ULL) != 2) return 4;
    return 0;
}
