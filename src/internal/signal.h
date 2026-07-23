/**
 * @file signal.h
 * @brief Declares the Windows default signal action.
 */

#ifndef WCRT_INTERNAL_SIGNAL_H
#define WCRT_INTERNAL_SIGNAL_H

/**
 * @brief Terminates the process for a default signal disposition.
 * @param signal_number Signal responsible for termination.
 * @note This function does not return.
 */
void __wcrt_signal_terminate(int signal_number);

#endif
