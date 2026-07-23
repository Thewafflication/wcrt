/**
 * @file time.c
 * @brief Exercises C89 clocks, calendar conversion, normalization, and format.
 */

#include <string.h>
#include <time.h>

/**
 * @brief Runs epoch, leap-year, local round-trip, and format checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    time_t epoch = 0;
    time_t now;
    time_t stored;
    struct tm *value;
    struct tm local_copy;
    struct tm normalization;
    char output[256];
    clock_t first_clock;
    clock_t second_clock;
    volatile unsigned long spin;

    value = gmtime(&epoch);
    if (value == NULL || value->tm_year != 70 || value->tm_mon != 0 ||
        value->tm_mday != 1 || value->tm_wday != 4 || value->tm_yday != 0) {
        return 1;
    }
    if (strcmp(asctime(value), "Thu Jan  1 00:00:00 1970\n") != 0) {
        return 2;
    }
    now = time(&stored);
    if (now != stored || now <= 0 || difftime(now, epoch) != (double)now) {
        return 3;
    }
    value = localtime(&now);
    if (value == NULL) {
        return 4;
    }
    local_copy = *value;
    if (mktime(&local_copy) != now) {
        return 5;
    }
    normalization.tm_sec = 0;
    normalization.tm_min = 0;
    normalization.tm_hour = 0;
    normalization.tm_mday = 32;
    normalization.tm_mon = 0;
    normalization.tm_year = 100;
    normalization.tm_wday = 0;
    normalization.tm_yday = 0;
    normalization.tm_isdst = -1;
    if (mktime(&normalization) == (time_t)-1 ||
        normalization.tm_mon != 1 || normalization.tm_mday != 1) {
        return 6;
    }
    value = gmtime(&epoch);
    if (strftime(output, sizeof(output),
        "%a|%A|%b|%B|%c|%d|%H|%I|%j|%m|%M|%p|%S|%U|%w|%W|%x|%X|%y|%Y|%Z|%%",
        value) == 0 || strstr(output, "Thu|Thursday|Jan|January") == NULL ||
        strstr(output, "1970") == NULL) {
        return 7;
    }
    if (strftime(output, 2, "long", value) != 0) {
        return 8;
    }
    if (ctime(&now) == NULL || strlen(ctime(&now)) != 25) {
        return 9;
    }
    first_clock = clock();
    for (spin = 0; spin < 1000000UL; ++spin) {
    }
    second_clock = clock();
    if (first_clock == (clock_t)-1 || second_clock == (clock_t)-1 ||
        second_clock < first_clock) {
        return 10;
    }
    return 0;
}
