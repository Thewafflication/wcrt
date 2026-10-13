/** @file calendar-time32.c @brief Checks wide/default and narrow time ABI. */
#include <time.h>
#include <time.h>
#if defined(time) || defined(mktime) || defined(difftime) || \
    defined(gmtime) || defined(localtime) || defined(ctime)
#error Narrow calendar aliases must require explicit selection
#endif
typedef char default_wide_calendar[sizeof(time_t) == 8 ? 1 : -1];
typedef char narrow_calendar_long[sizeof(long) == 4 ? 1 : -1];
time_t (*default_calendar_call)(time_t *) = time;
long (__cdecl *narrow_calendar_call)(long *) = wcrt_time32;
long (__cdecl *narrow_make_call)(struct tm *) = wcrt_mktime32;
double (__cdecl *narrow_difference_call)(long, long) = wcrt_difftime32;
struct tm *(__cdecl *narrow_utc_call)(const long *) = wcrt_gmtime32;
struct tm *(__cdecl *narrow_local_call)(const long *) = wcrt_localtime32;
char *(__cdecl *narrow_text_call)(const long *) = wcrt_ctime32;
