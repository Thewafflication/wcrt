/**
 * @file clock.c
 * @brief Implements wall and process clocks with Windows FILETIME APIs.
 */

#include "../../internal/time.h"

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_EPOCH_TICKS 116444736000000000ULL

/** @brief Windows 100-nanosecond timestamp halves. */
struct wcrt_file_time {
    unsigned long low;
    unsigned long high;
};

__declspec(dllimport) void WCRT_WINAPI GetSystemTimeAsFileTime(
    struct wcrt_file_time *value);
__declspec(dllimport) void *WCRT_WINAPI GetCurrentProcess(void);
__declspec(dllimport) int WCRT_WINAPI GetProcessTimes(void *process,
    struct wcrt_file_time *creation, struct wcrt_file_time *exit_time,
    struct wcrt_file_time *kernel, struct wcrt_file_time *user);
__declspec(dllimport) int WCRT_WINAPI FileTimeToLocalFileTime(
    const struct wcrt_file_time *utc, struct wcrt_file_time *local);
__declspec(dllimport) int WCRT_WINAPI LocalFileTimeToFileTime(
    const struct wcrt_file_time *local, struct wcrt_file_time *utc);

/** @brief Combines a Windows FILETIME into an unsigned tick count. */
static unsigned long long wcrt_file_ticks(const struct wcrt_file_time *value)
{
    return ((unsigned long long)value->high << 32) | value->low;
}

/** @brief Splits an unsigned tick count into a Windows FILETIME. */
static void wcrt_store_ticks(struct wcrt_file_time *value,
    unsigned long long ticks)
{
    value->low = (unsigned long)ticks;
    value->high = (unsigned long)(ticks >> 32);
}

time_t __wcrt_wall_time(void)
{
    struct wcrt_file_time value;
    GetSystemTimeAsFileTime(&value);
    return (time_t)((wcrt_file_ticks(&value) - WCRT_EPOCH_TICKS) /
        10000000ULL);
}

clock_t __wcrt_processor_clock(void)
{
    struct wcrt_file_time creation;
    struct wcrt_file_time exit_time;
    struct wcrt_file_time kernel;
    struct wcrt_file_time user;
    unsigned long long ticks;

    if (!GetProcessTimes(GetCurrentProcess(), &creation, &exit_time, &kernel,
        &user)) {
        return (clock_t)-1;
    }
    ticks = wcrt_file_ticks(&kernel) + wcrt_file_ticks(&user);
    return (clock_t)(ticks / 10000ULL);
}

time_t __wcrt_utc_to_local(time_t utc)
{
    struct wcrt_file_time input;
    struct wcrt_file_time output;
    unsigned long long ticks = (unsigned long long)utc * 10000000ULL +
        WCRT_EPOCH_TICKS;
    wcrt_store_ticks(&input, ticks);
    if (!FileTimeToLocalFileTime(&input, &output)) {
        return utc;
    }
    return (time_t)((wcrt_file_ticks(&output) - WCRT_EPOCH_TICKS) /
        10000000ULL);
}

time_t __wcrt_local_to_utc(time_t local)
{
    struct wcrt_file_time input;
    struct wcrt_file_time output;
    unsigned long long ticks = (unsigned long long)local * 10000000ULL +
        WCRT_EPOCH_TICKS;
    wcrt_store_ticks(&input, ticks);
    if (!LocalFileTimeToFileTime(&input, &output)) {
        return (time_t)-1;
    }
    return (time_t)((wcrt_file_ticks(&output) - WCRT_EPOCH_TICKS) /
        10000000ULL);
}
