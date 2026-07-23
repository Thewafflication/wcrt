/**
 * @file signal.c
 * @brief Exercises C89 signal dispositions and synchronous delivery.
 */

#include <signal.h>

/** @brief Signal number observed by the test handler. */
static volatile sig_atomic_t observed_signal;

/**
 * @brief Records a delivered signal using handler-safe assignment.
 * @param signal_number Delivered signal number.
 */
static void record_signal(int signal_number)
{
    observed_signal = signal_number;
}

/**
 * @brief Tests registration, delivery, reset, ignore, and invalid requests.
 * @param argument_count Number of command-line arguments.
 * @param arguments Command-line arguments; one extra requires reset-to-default.
 * @return Zero on success or a test-specific failure code.
 */
int main(int argument_count, char **arguments)
{
    static const int signals[] = {
        SIGTERM, SIGINT, SIGABRT, SIGFPE, SIGILL, SIGSEGV
    };
    int signal_count = argument_count > 1 ? 6 : 1;
    int index;

    (void)arguments;

    for (index = 0; index < signal_count; ++index) {
        int signal_number = signals[index];
        observed_signal = 0;
        if (signal(signal_number, record_signal) != SIG_DFL) {
            return 1;
        }
        if (signal(signal_number, record_signal) != record_signal) {
            return 2;
        }
        if (raise(signal_number) != 0 || observed_signal != signal_number) {
            return 3;
        }
        {
            void (*previous)(int) = signal(signal_number, SIG_IGN);
            if (previous == SIG_ERR && argument_count == 1) {
                signal(signal_number, SIG_DFL);
                continue;
            }
            if (argument_count > 1 && previous != SIG_DFL) {
                return 4;
            }
        }
        if (raise(signal_number) != 0) {
            return 5;
        }
        if (signal(signal_number, SIG_DFL) != SIG_IGN) {
            return 6;
        }
    }
    if (argument_count > 1) {
        if (signal(999, record_signal) != SIG_ERR || raise(999) == 0) {
            return 7;
        }
        if (signal(SIGTERM, SIG_ERR) != SIG_ERR) {
            return 8;
        }
        if (signal(SIGTERM, SIG_IGN) != SIG_DFL) {
            return 9;
        }
    }
    return 0;
}
