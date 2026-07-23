/**
 * @file signal_default.c
 * @brief Exercises the destructive default signal action in a child process.
 */

#include <signal.h>

/**
 * @brief Raises a signal with its default disposition.
 * @return Failure code if default termination unexpectedly returns.
 */
int main(void)
{
    signal(SIGTERM, SIG_DFL);
    raise(SIGTERM);
    return 1;
}
