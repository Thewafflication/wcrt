/** @file timeval.c @brief Verifies selected POSIX wall-clock behavior. */
#define WCRT_POSIX 1
#include <errno.h>
#include <sys/time.h>
#include <time.h>

int main(void)
{
    struct timeval value;
    time_t before = time(NULL) - 1;
    time_t after;
    if (gettimeofday(&value, NULL) != 0) return 1;
    after = time(NULL) + 1;
    if (value.tv_sec < before || value.tv_sec > after) return 2;
    if (value.tv_usec < 0 || value.tv_usec >= 1000000L) return 3;
    errno = 0;
    if (gettimeofday(NULL, NULL) != -1 || errno != EINVAL) return 4;
    return 0;
}
