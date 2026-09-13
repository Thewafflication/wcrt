/** @file timeval.c @brief Checks strict isolation of POSIX wall-clock names. */
#include <sys/time.h>
#include <sys/time.h>

struct timeval { int strict_member; };
static int gettimeofday;

int main(void)
{
    struct timeval value;
    value.strict_member = 0;
    return value.strict_member + gettimeofday;
}
