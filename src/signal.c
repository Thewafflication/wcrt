/**
 * @file signal.c
 * @brief Implements C89 signal registration and synchronous delivery.
 */

#include <signal.h>

#include "internal/signal.h"

/** @brief Number of signal dispositions maintained by WCRT. */
#define WCRT_SIGNAL_COUNT 6

/** @brief Process-wide dispositions in signal-index order. */
static void (*wcrt_handlers[WCRT_SIGNAL_COUNT])(int);

/**
 * @brief Maps a public signal number to the disposition table.
 * @param signal_number Candidate public signal number.
 * @return Table index, or -1 when unsupported.
 */
static int wcrt_signal_index(int signal_number)
{
    switch (signal_number) {
    case SIGABRT:
        return 0;
    case SIGFPE:
        return 1;
    case SIGILL:
        return 2;
    case SIGINT:
        return 3;
    case SIGSEGV:
        return 4;
    case SIGTERM:
        return 5;
    default:
        return -1;
    }
}

void (*signal(int signal_number, void (*handler)(int)))(int)
{
    int index = wcrt_signal_index(signal_number);
    void (*previous)(int);

    if (index < 0 || handler == SIG_ERR) {
        return SIG_ERR;
    }
    previous = wcrt_handlers[index];
    wcrt_handlers[index] = handler;
    return previous;
}

int raise(int signal_number)
{
    int index = wcrt_signal_index(signal_number);
    void (*handler)(int);

    if (index < 0) {
        return 1;
    }
    handler = wcrt_handlers[index];
    if (handler == SIG_IGN) {
        return 0;
    }
    if (handler == SIG_DFL) {
        __wcrt_signal_terminate(signal_number);
        return 1;
    }
    wcrt_handlers[index] = SIG_DFL;
    handler(signal_number);
    return 0;
}
