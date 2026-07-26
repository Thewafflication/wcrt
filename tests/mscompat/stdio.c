/**
 * @file stdio.c
 * @brief Exercises Microsoft legacy bounded formatted output.
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/** @brief Calls _vsnprintf for direct va_list coverage. */
static int call_vsnprintf(char *output, size_t size, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = _vsnprintf(output, size, format, arguments);
    va_end(arguments);
    return result;
}

/** @brief Runs Microsoft compatibility boundary checks. */
int main(void)
{
    char buffer[8];
    int result;

    memset(buffer, 'X', sizeof(buffer));
    result = _snprintf(buffer, 3, "%s", "abc");
    if (result != 3 || memcmp(buffer, "abcX", 4) != 0) return 1;
    memset(buffer, 'X', sizeof(buffer));
    result = _snprintf(buffer, 5, "%s", "abc");
    if (result != 3 || memcmp(buffer, "abc\0X", 5) != 0) return 2;
    memset(buffer, 'X', sizeof(buffer));
    result = _snprintf(buffer, 3, "%s", "abcdef");
    if (result >= 0 || memcmp(buffer, "abcX", 4) != 0) return 3;
    memset(buffer, 'X', sizeof(buffer));
    result = _snprintf(buffer, 0, "%s", "abc");
    if (result >= 0 || buffer[0] != 'X') return 4;
    if (_snprintf(NULL, 0, "%s-%d", "abc", 12) != 6) return 5;
    memset(buffer, 'X', sizeof(buffer));
    result = call_vsnprintf(buffer, 4, "%s%c", "abc", '-');
    if (result != 4 || memcmp(buffer, "abc-X", 5) != 0) return 6;
    memset(buffer, 'X', sizeof(buffer));
    result = call_vsnprintf(buffer, 3, "%s-%d", "abc", 12);
    if (result >= 0 || memcmp(buffer, "abcX", 4) != 0) return 7;
    return 0;
}
