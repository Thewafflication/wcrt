/** @file calendar-clock-errors.c @brief Models signed epoch conversion. */
#define WCRT_POSIX 1
#include <errno.h>
#include <string.h>
#undef __declspec
#define __declspec(attribute)
#define GetSystemTimeAsFileTime test_calendar_clock
#define FileTimeToLocalFileTime test_calendar_local
#define LocalFileTimeToFileTime test_calendar_utc
#include "../../src/platform/windows/clock.c"

static unsigned long long clock_ticks = WCRT_EPOCH_TICKS;
static unsigned long long converted_ticks = WCRT_EPOCH_TICKS;

/** @brief Supplies a deterministic clock without changing system time. */
void WCRT_WINAPI test_calendar_clock(struct wcrt_file_time *value)
{
    wcrt_store_ticks(value, clock_ticks);
}

/** @brief Supplies a timezone result without changing configuration. */
int WCRT_WINAPI test_calendar_local(const struct wcrt_file_time *input,
    struct wcrt_file_time *output)
{
    (void)input;
    wcrt_store_ticks(output, converted_ticks);
    return 1;
}

/** @brief Supplies a selected inverse timezone result. */
int WCRT_WINAPI test_calendar_utc(const struct wcrt_file_time *input,
    struct wcrt_file_time *output)
{
    (void)input;
    wcrt_store_ticks(output, converted_ticks);
    return 1;
}

int main(void)
{
    long guarded[3] = { 1234567, 0, 7654321 };
    struct tm input;
    struct tm before;
    clock_ticks = WCRT_EPOCH_TICKS - 1;
    if (__wcrt_wall_time() != -1) return 1;
    clock_ticks = WCRT_EPOCH_TICKS;
    if (__wcrt_wall_time() != 0) return 2;
    converted_ticks = WCRT_EPOCH_TICKS - 216000000000ULL;
    if (__wcrt_utc_to_local(0) != -21600) return 3;
    converted_ticks = WCRT_EPOCH_TICKS - 1;
    if (__wcrt_local_to_utc(0) != -1) return 4;
    converted_ticks = WCRT_EPOCH_TICKS;
    if (__wcrt_local_to_utc(-21600) != 0) return 5;
    clock_ticks = WCRT_EPOCH_TICKS + 21474836479999999ULL;
    errno = EDOM;
    if (wcrt_time32(&guarded[1]) != 2147483647L || errno != EDOM ||
        guarded[1] != 2147483647L || guarded[0] != 1234567 ||
        guarded[2] != 7654321) return 6;
    clock_ticks += 1;
    if (wcrt_time32(&guarded[1]) != -1 || errno != EINVAL ||
        guarded[1] != -1 || guarded[0] != 1234567 || guarded[2] != 7654321)
        return 7;
    clock_ticks = WCRT_EPOCH_TICKS - 1;
    if (wcrt_time32(&guarded[1]) != -1 || errno != EINVAL ||
        guarded[0] != 1234567 || guarded[2] != 7654321) return 8;
    clock_ticks = WCRT_EPOCH_TICKS;
    errno = EDOM;
    if (wcrt_time32(&guarded[1]) != 0 || errno != EDOM) return 9;
    memset(&input, 0, sizeof(input));
    input.tm_year = 70;
    input.tm_mday = 1;
    before = input;
    converted_ticks = WCRT_EPOCH_TICKS + 21474836480000000ULL;
    if (wcrt_mktime32(&input) != -1 || errno != EINVAL ||
        memcmp(&before, &input, sizeof(input))) return 10;
    converted_ticks = WCRT_EPOCH_TICKS - 10000000ULL;
    if (wcrt_mktime32(&input) != -1 || errno != EINVAL ||
        memcmp(&before, &input, sizeof(input))) return 11;
    return 0;
}
