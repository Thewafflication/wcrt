/**
 * @file signal.h
 * @brief Declares C89 signal handling facilities.
 */

#ifndef WCRT_SIGNAL_H
#define WCRT_SIGNAL_H

/** @brief Integer type safely assignable by a signal handler. */
typedef int sig_atomic_t;

/** @brief Interrupt signal number. */
#define SIGINT 2
/** @brief Illegal-instruction signal number. */
#define SIGILL 4
/** @brief Floating-point exception signal number. */
#define SIGFPE 8
/** @brief Invalid-memory-access signal number. */
#define SIGSEGV 11
/** @brief Termination-request signal number. */
#define SIGTERM 15
/** @brief Abnormal-termination signal number. */
#define SIGABRT 22

/** @brief Selects the implementation-defined default action. */
#define SIG_DFL ((void (*)(int))0)
/** @brief Selects signal suppression. */
#define SIG_IGN ((void (*)(int))1)
/** @brief Reports that signal registration failed. */
#define SIG_ERR ((void (*)(int))-1)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Changes the disposition of a supported signal.
 * @param signal_number Supported signal number.
 * @param handler Default, ignored, or user-defined disposition.
 * @return Previous disposition, or SIG_ERR when the request is invalid.
 */
void (*signal(int signal_number, void (*handler)(int)))(int);

/**
 * @brief Delivers a supported signal synchronously.
 * @param signal_number Supported signal number.
 * @return Zero after successful delivery, or nonzero when unsupported.
 */
int raise(int signal_number);

#ifdef __cplusplus
}
#endif

#endif
