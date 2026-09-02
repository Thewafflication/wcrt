/** @file unistd.c @brief Checks selected POSIX process declarations. */
#define WCRT_POSIX 1
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>

#ifndef WCRT_UNISTD_H
#error <unistd.h> did not resolve to the WCRT header
#endif
#ifdef _POSIX_VERSION
#error A bounded WCRT profile must not advertise _POSIX_VERSION
#endif

/** @brief Verifies that pid_t covers every unsigned 32-bit process ID. */
typedef char posix_pid_width[sizeof(pid_t) >= 8 ? 1 : -1];
/** @brief Selected process-identifier signature. */
typedef pid_t (*posix_getpid_fn)(void);
/** @brief Forces type checking of getpid. */
static posix_getpid_fn posix_getpid_address = getpid;

int main(void)
{
    return posix_getpid_address == 0;
}
