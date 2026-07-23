/**
 * @file time.c
 * @brief Implements C89 calendar conversion and formatting.
 */

#include <string.h>
#include <time.h>

#include "internal/time.h"

/** @brief Shared broken-down result overwritten by conversion functions. */
static struct tm wcrt_broken_down;
/** @brief Shared fixed-format result overwritten by text conversions. */
static char wcrt_time_text[26];

/** @brief Abbreviated C-locale weekday names. */
static const char *wcrt_short_days[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
/** @brief Full C-locale weekday names. */
static const char *wcrt_long_days[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
    "Saturday"
};
/** @brief Abbreviated C-locale month names. */
static const char *wcrt_short_months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
/** @brief Full C-locale month names. */
static const char *wcrt_long_months[] = {
    "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"
};

/** @brief Computes mathematical floor division for a positive divisor. */
static long long wcrt_floor_div(long long value, long long divisor)
{
    long long quotient = value / divisor;
    if (value % divisor < 0) {
        --quotient;
    }
    return quotient;
}

/** @brief Converts a civil date to days relative to the Unix epoch. */
static long long wcrt_days_from_civil(long long year, int month, int day)
{
    long long era;
    unsigned int year_of_era;
    unsigned int day_of_year;
    unsigned int day_of_era;

    year -= month <= 2;
    era = wcrt_floor_div(year, 400);
    year_of_era = (unsigned int)(year - era * 400);
    day_of_year = (153U * (unsigned int)(month + (month > 2 ? -3 : 9)) +
        2U) / 5U + (unsigned int)day - 1U;
    day_of_era = year_of_era * 365U + year_of_era / 4U -
        year_of_era / 100U + day_of_year;
    return era * 146097LL + (long long)day_of_era - 719468LL;
}

/** @brief Converts epoch-relative days to a normalized civil date. */
static void wcrt_civil_from_days(long long days, int *year, int *month,
    int *day)
{
    long long adjusted = days + 719468LL;
    long long era = wcrt_floor_div(adjusted, 146097LL);
    unsigned int day_of_era = (unsigned int)(adjusted - era * 146097LL);
    unsigned int year_of_era = (day_of_era - day_of_era / 1460U +
        day_of_era / 36524U - day_of_era / 146096U) / 365U;
    long long full_year = (long long)year_of_era + era * 400LL;
    unsigned int day_of_year = day_of_era -
        (365U * year_of_era + year_of_era / 4U - year_of_era / 100U);
    unsigned int month_piece = (5U * day_of_year + 2U) / 153U;

    *day = (int)(day_of_year - (153U * month_piece + 2U) / 5U + 1U);
    *month = (int)month_piece + (month_piece < 10U ? 3 : -9);
    full_year += *month <= 2;
    *year = (int)full_year;
}

/** @brief Populates a broken-down UTC-like value from epoch seconds. */
static struct tm *wcrt_break_down(time_t value)
{
    long long days = wcrt_floor_div(value, 86400LL);
    long long seconds = value - days * 86400LL;
    int year;
    int month;
    int day;
    long long year_start;

    wcrt_civil_from_days(days, &year, &month, &day);
    year_start = wcrt_days_from_civil(year, 1, 1);
    wcrt_broken_down.tm_sec = (int)(seconds % 60);
    wcrt_broken_down.tm_min = (int)((seconds / 60) % 60);
    wcrt_broken_down.tm_hour = (int)(seconds / 3600);
    wcrt_broken_down.tm_mday = day;
    wcrt_broken_down.tm_mon = month - 1;
    wcrt_broken_down.tm_year = year - 1900;
    wcrt_broken_down.tm_wday = (int)((days + 4) % 7);
    if (wcrt_broken_down.tm_wday < 0) {
        wcrt_broken_down.tm_wday += 7;
    }
    wcrt_broken_down.tm_yday = (int)(days - year_start);
    wcrt_broken_down.tm_isdst = -1;
    return &wcrt_broken_down;
}

clock_t clock(void)
{
    return __wcrt_processor_clock();
}

double difftime(time_t end, time_t beginning)
{
    return (double)(end - beginning);
}

time_t time(time_t *destination)
{
    time_t value = __wcrt_wall_time();
    if (destination != NULL) {
        *destination = value;
    }
    return value;
}

struct tm *gmtime(const time_t *timer)
{
    if (timer == NULL) {
        return NULL;
    }
    return wcrt_break_down(*timer);
}

struct tm *localtime(const time_t *timer)
{
    if (timer == NULL) {
        return NULL;
    }
    return wcrt_break_down(__wcrt_utc_to_local(*timer));
}

time_t mktime(struct tm *broken_down)
{
    long long total_months;
    long long year;
    int month;
    long long seconds;
    long long days;
    time_t local_value;
    time_t utc_value;
    struct tm normalized;

    if (broken_down == NULL) {
        return (time_t)-1;
    }
    total_months = (long long)(broken_down->tm_year + 1900) * 12LL +
        broken_down->tm_mon;
    year = wcrt_floor_div(total_months, 12LL);
    month = (int)(total_months - year * 12LL) + 1;
    days = wcrt_days_from_civil(year, month, 1) +
        (long long)broken_down->tm_mday - 1LL;
    seconds = (long long)broken_down->tm_hour * 3600LL +
        (long long)broken_down->tm_min * 60LL + broken_down->tm_sec;
    days += wcrt_floor_div(seconds, 86400LL);
    seconds -= wcrt_floor_div(seconds, 86400LL) * 86400LL;
    local_value = (time_t)(days * 86400LL + seconds);
    utc_value = __wcrt_local_to_utc(local_value);
    if (utc_value == (time_t)-1) {
        return (time_t)-1;
    }
    normalized = *localtime(&utc_value);
    *broken_down = normalized;
    return utc_value;
}

/** @brief Writes exactly two decimal digits. */
static void wcrt_two_digits(char *output, int value)
{
    output[0] = (char)('0' + value / 10 % 10);
    output[1] = (char)('0' + value % 10);
}

char *asctime(const struct tm *broken_down)
{
    int year;
    if (broken_down == NULL || broken_down->tm_wday < 0 ||
        broken_down->tm_wday > 6 || broken_down->tm_mon < 0 ||
        broken_down->tm_mon > 11) {
        return NULL;
    }
    memcpy(wcrt_time_text, wcrt_short_days[broken_down->tm_wday], 3);
    wcrt_time_text[3] = ' ';
    memcpy(wcrt_time_text + 4, wcrt_short_months[broken_down->tm_mon], 3);
    wcrt_time_text[7] = ' ';
    wcrt_time_text[8] = broken_down->tm_mday < 10 ? ' ' :
        (char)('0' + broken_down->tm_mday / 10 % 10);
    wcrt_time_text[9] = (char)('0' + broken_down->tm_mday % 10);
    wcrt_time_text[10] = ' ';
    wcrt_two_digits(wcrt_time_text + 11, broken_down->tm_hour);
    wcrt_time_text[13] = ':';
    wcrt_two_digits(wcrt_time_text + 14, broken_down->tm_min);
    wcrt_time_text[16] = ':';
    wcrt_two_digits(wcrt_time_text + 17, broken_down->tm_sec);
    wcrt_time_text[19] = ' ';
    year = broken_down->tm_year + 1900;
    wcrt_time_text[20] = (char)('0' + year / 1000 % 10);
    wcrt_time_text[21] = (char)('0' + year / 100 % 10);
    wcrt_time_text[22] = (char)('0' + year / 10 % 10);
    wcrt_time_text[23] = (char)('0' + year % 10);
    wcrt_time_text[24] = '\n';
    wcrt_time_text[25] = '\0';
    return wcrt_time_text;
}

char *ctime(const time_t *timer)
{
    struct tm *value = localtime(timer);
    return value == NULL ? NULL : asctime(value);
}

/** @brief Appends text to a bounded strftime destination. */
static int wcrt_append_text(char *destination, size_t capacity,
    size_t *length, const char *text)
{
    while (*text != '\0') {
        if (*length + 1 >= capacity) {
            return 0;
        }
        destination[(*length)++] = *text++;
    }
    return 1;
}

/** @brief Appends a zero-padded decimal field. */
static int wcrt_append_number(char *destination, size_t capacity,
    size_t *length, int value, int width)
{
    char digits[12];
    int index;
    for (index = width - 1; index >= 0; --index) {
        digits[index] = (char)('0' + value % 10);
        value /= 10;
    }
    digits[width] = '\0';
    return wcrt_append_text(destination, capacity, length, digits);
}

size_t strftime(char *destination, size_t capacity, const char *format,
    const struct tm *value)
{
    size_t length = 0;
    if (capacity == 0 || destination == NULL || format == NULL ||
        value == NULL) {
        return 0;
    }
    while (*format != '\0') {
        char conversion;
        int number = 0;
        int width = 2;
        const char *text = NULL;
        if (*format != '%') {
            char literal[2] = {*format++, '\0'};
            if (!wcrt_append_text(destination, capacity, &length, literal)) {
                return 0;
            }
            continue;
        }
        conversion = *++format;
        if (conversion == '\0') {
            return 0;
        }
        ++format;
        switch (conversion) {
        case 'a': text = wcrt_short_days[value->tm_wday]; break;
        case 'A': text = wcrt_long_days[value->tm_wday]; break;
        case 'b': text = wcrt_short_months[value->tm_mon]; break;
        case 'B': text = wcrt_long_months[value->tm_mon]; break;
        case 'd': number = value->tm_mday; break;
        case 'H': number = value->tm_hour; break;
        case 'I':
            number = value->tm_hour % 12;
            if (number == 0) number = 12;
            break;
        case 'j': number = value->tm_yday + 1; width = 3; break;
        case 'm': number = value->tm_mon + 1; break;
        case 'M': number = value->tm_min; break;
        case 'p': text = value->tm_hour < 12 ? "AM" : "PM"; break;
        case 'S': number = value->tm_sec; break;
        case 'U': number = (value->tm_yday + 7 - value->tm_wday) / 7; break;
        case 'w': number = value->tm_wday; width = 1; break;
        case 'W':
            number = (value->tm_yday + 7 -
                ((value->tm_wday + 6) % 7)) / 7;
            break;
        case 'y': number = (value->tm_year + 1900) % 100; break;
        case 'Y': number = value->tm_year + 1900; width = 4; break;
        case 'Z': text = value->tm_isdst < 0 ? "" : "Local"; break;
        case '%': text = "%"; break;
        case 'x':
            if (strftime(destination + length, capacity - length,
                "%m/%d/%y", value) == 0) return 0;
            length += strlen(destination + length);
            continue;
        case 'X':
            if (strftime(destination + length, capacity - length,
                "%H:%M:%S", value) == 0) return 0;
            length += strlen(destination + length);
            continue;
        case 'c':
            if (strftime(destination + length, capacity - length,
                "%a %b %d %H:%M:%S %Y", value) == 0) return 0;
            length += strlen(destination + length);
            continue;
        default: return 0;
        }
        if (text != NULL) {
            if (!wcrt_append_text(destination, capacity, &length, text)) {
                return 0;
            }
        } else if (!wcrt_append_number(destination, capacity, &length,
            number, width)) {
            return 0;
        }
    }
    destination[length] = '\0';
    return length;
}
