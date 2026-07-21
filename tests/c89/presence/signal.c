/* REQ-0009: <signal.h> presence. Compile only; do not link. */
#include <signal.h>

#ifndef SIG_DFL
#error "SIG_DFL must be defined"
#endif
#ifndef SIG_ERR
#error "SIG_ERR must be defined"
#endif
#ifndef SIG_IGN
#error "SIG_IGN must be defined"
#endif
#ifndef SIGABRT
#error "SIGABRT must be defined"
#endif
#ifndef SIGFPE
#error "SIGFPE must be defined"
#endif
#ifndef SIGILL
#error "SIGILL must be defined"
#endif
#ifndef SIGINT
#error "SIGINT must be defined"
#endif
#ifndef SIGSEGV
#error "SIGSEGV must be defined"
#endif
#ifndef SIGTERM
#error "SIGTERM must be defined"
#endif

typedef void (*wcrt_signal_handler)(int);
typedef wcrt_signal_handler (*wcrt_signal_fn)(int, wcrt_signal_handler);
typedef int (*wcrt_raise_fn)(int);

static wcrt_signal_fn wcrt_signal_address = (signal);
static wcrt_raise_fn wcrt_raise_address = (raise);
static volatile sig_atomic_t wcrt_signal_value;
static wcrt_signal_handler wcrt_signal_actions[] = {
    SIG_DFL, SIG_ERR, SIG_IGN
};

enum {
    wcrt_signal_constants_present =
        SIGABRT + SIGFPE + SIGILL + SIGINT + SIGSEGV + SIGTERM
};
