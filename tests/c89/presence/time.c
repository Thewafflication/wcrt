/* REQ-0015: <time.h> presence. Compile only; do not link. */
#include <time.h>

#ifndef NULL
#error "NULL must be defined"
#endif
#ifndef CLOCKS_PER_SEC
#error "CLOCKS_PER_SEC must be defined"
#endif

typedef clock_t (*wcrt_clock_fn)(void);
typedef double (*wcrt_difftime_fn)(time_t, time_t);
typedef time_t (*wcrt_mktime_fn)(struct tm *);
typedef time_t (*wcrt_time_fn)(time_t *);
typedef char *(*wcrt_asctime_fn)(const struct tm *);
typedef char *(*wcrt_ctime_fn)(const time_t *);
typedef struct tm *(*wcrt_tm_fn)(const time_t *);
typedef size_t (*wcrt_strftime_fn)(char *, size_t, const char *,
                                   const struct tm *);

static wcrt_clock_fn wcrt_clock_address = (clock);
static wcrt_difftime_fn wcrt_difftime_address = (difftime);
static wcrt_mktime_fn wcrt_mktime_address = (mktime);
static wcrt_time_fn wcrt_time_address = (time);
static wcrt_asctime_fn wcrt_asctime_address = (asctime);
static wcrt_ctime_fn wcrt_ctime_address = (ctime);
static wcrt_tm_fn wcrt_tm_functions[] = { (gmtime), (localtime) };
static wcrt_strftime_fn wcrt_strftime_address = (strftime);
static clock_t wcrt_clock_value = CLOCKS_PER_SEC;
static time_t wcrt_time_value;
static size_t wcrt_time_size;
static struct tm wcrt_tm_value;

static void wcrt_tm_members(void)
{
    wcrt_tm_value.tm_sec = 0;
    wcrt_tm_value.tm_min = 0;
    wcrt_tm_value.tm_hour = 0;
    wcrt_tm_value.tm_mday = 0;
    wcrt_tm_value.tm_mon = 0;
    wcrt_tm_value.tm_year = 0;
    wcrt_tm_value.tm_wday = 0;
    wcrt_tm_value.tm_yday = 0;
    wcrt_tm_value.tm_isdst = 0;
}
