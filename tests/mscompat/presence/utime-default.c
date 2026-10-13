/** @file utime-default.c @brief Checks default file-time alias isolation. */
#include <sys/utime.h>
#include <sys/utime.h>
#include <time.h>

#ifdef _utime
#error Generic file-time alias leaked into default headers
#endif
#ifdef _utimbuf
#error Legacy file-time tag alias leaked into default headers
#endif
#ifdef _futime
#error Generic descriptor file-time alias leaked into default headers
#endif
typedef char explicit_narrow_width[sizeof(struct __utimbuf32) == 8 ? 1 : -1];
typedef char explicit_wide_width[sizeof(struct __utimbuf64) == 16 ? 1 : -1];
typedef char default_calendar_width[sizeof(time_t) == 8 ? 1 : -1];

/** @brief Placeholder proving the default generic tag remains absent. */
struct _utimbuf {
    char placeholder; /**< Default-profile proof member. */
};
static int _utime;

/** @brief Uses the default-profile placeholder to prove no function exists. */
int check_default_utime(void)
{
    return _utime;
}
