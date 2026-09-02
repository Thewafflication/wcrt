/** @file unistd.c @brief Verifies selected POSIX getpid behavior. */
#define WCRT_POSIX 1
#include <unistd.h>

int main(void)
{
    pid_t first = getpid();
    pid_t second = getpid();

    if (first <= 0) return 1;
    if (first != second) return 2;
    return 0;
}
