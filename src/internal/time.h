/**
 * @file time.h
 * @brief Declares Windows clock and local-time adaptation.
 */

#ifndef WCRT_INTERNAL_TIME_H
#define WCRT_INTERNAL_TIME_H

#include <time.h>

time_t __wcrt_wall_time(void);
clock_t __wcrt_processor_clock(void);
time_t __wcrt_utc_to_local(time_t utc);
time_t __wcrt_local_to_utc(time_t local);

#endif
