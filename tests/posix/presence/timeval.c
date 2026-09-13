/** @file timeval.c @brief Checks selected POSIX wall-clock declarations. */
#define WCRT_POSIX 1
#include <sys/time.h>
#include <sys/time.h>

typedef int (*posix_gettimeofday_fn)(struct timeval *, void *);
static posix_gettimeofday_fn posix_gettimeofday_address = gettimeofday;

typedef char posix_timeval_seconds_width[
    sizeof(((struct timeval *)0)->tv_sec) == sizeof(time_t) ? 1 : -1];

int main(void)
{
    return posix_gettimeofday_address == 0;
}
