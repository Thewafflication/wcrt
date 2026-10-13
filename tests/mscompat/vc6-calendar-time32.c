/** @file vc6-calendar-time32.c @brief Exercises narrow generic time routing. */
#define WCRT_VC6_TIME32 1
#define WCRT_VC6_STAT32 1
#define WCRT_VC6_UTIME32 1
#include <wchar.h>
#include <time.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include "calendar-time32.c"

typedef char selected_time_width[sizeof(time_t) == 4 ? 1 : -1];
typedef char selected_clock_width[sizeof(clock_t) == 4 ? 1 : -1];
typedef char retained_fpos_width[sizeof(fpos_t) == 8 ? 1 : -1];
typedef char retained_time_record[sizeof(struct __utimbuf64) == 16 ? 1 : -1];
typedef char retained_tm_record[sizeof(struct tm) == 36 ? 1 : -1];

/** @brief Calls all generic spellings through narrow pointer declarations. */
int check_vc6_calendar_time32(void)
{
    time_t (__cdecl *current)(time_t *) = time;
    time_t (__cdecl *make)(struct tm *) = mktime;
    double (__cdecl *difference)(time_t, time_t) = difftime;
    struct tm *(__cdecl *utc)(const time_t *) = gmtime;
    struct tm *(__cdecl *local)(const time_t *) = localtime;
    char *(__cdecl *text)(const time_t *) = ctime;
    struct calendar_guard guarded;
    struct tm copy;
    long value;
    int result = check_calendar32();
    if (result) return result;
    memset(&guarded, 0xa5, sizeof(guarded));
    guarded.value = 0;
    if (utc(&guarded.value) == NULL || local(&guarded.value) == NULL)
        return 14;
    copy = *local(&guarded.value);
    if (make(&copy) != 0 || difference(LONG_MAX, LONG_MIN) != 4294967295.0 ||
        text(&guarded.value) == NULL || !calendar_guards(&guarded)) return 15;
    errno = EDOM;
    value = current(&guarded.value);
    if (value < 0 || guarded.value != value || errno != EDOM ||
        !calendar_guards(&guarded)) return 16;
    return 0;
}
