/** @file sys/param.h @brief Declares selected POSIX system parameters. */

#ifndef WCRT_SYS_PARAM_H
#define WCRT_SYS_PARAM_H

#if defined(WCRT_POSIX)
#include <limits.h>
/** @brief Compatibility spelling for the selected maximum path length. */
#define MAXPATHLEN PATH_MAX
#endif

#endif
