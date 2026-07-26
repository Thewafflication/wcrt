/**
 * @file stdio.c
 * @brief Exercises C99 bounded formatted output.
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/** @brief Calls vsnprintf for direct va_list coverage. */
static int call_vsnprintf(char *output, size_t size, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vsnprintf(output, size, format, arguments);
    va_end(arguments);
    return result;
}

/** @brief Runs C99 bounded-output boundary checks. */
int main(void)
{
    char buffer[8];
    int result;

    memset(buffer, 'X', sizeof(buffer));
    result = snprintf(buffer, sizeof(buffer), "%s", "abc");
    if (result != 3 || strcmp(buffer, "abc") != 0 || buffer[4] != 'X') {
        return 1;
    }
    memset(buffer, 'X', sizeof(buffer));
    result = snprintf(buffer, 4, "%s", "abcdef");
    if (result != 6 || memcmp(buffer, "abc\0X", 5) != 0) return 2;
    memset(buffer, 'X', sizeof(buffer));
    result = snprintf(buffer, 1, "%s", "abcdef");
    if (result != 6 || buffer[0] != '\0' || buffer[1] != 'X') return 3;
    result = snprintf(buffer, 0, "%s", "abcdef");
    if (result != 6 || buffer[0] != '\0' || buffer[1] != 'X') return 4;
    if (snprintf(NULL, 0, "%s-%d", "abc", 12) != 6) return 5;
    memset(buffer, 'X', sizeof(buffer));
    result = call_vsnprintf(buffer, 5, "%s-%d", "abc", 12);
    if (result != 6 || memcmp(buffer, "abc-\0X", 6) != 0) return 6;
    return 0;
}
