/** @file unistd.c @brief Checks strict isolation of POSIX process names. */
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>

/** @brief Strict-profile placeholder proving pid_t is absent. */
typedef char pid_t;
/** @brief Strict-profile object proving getpid is absent. */
static int getpid;

int main(void)
{
    pid_t value = 0;
    return value + getpid;
}
