/**
 * @file errno.h
 * @brief Declares the C89 error indicator and required error constants.
 */

#ifndef WCRT_ERRNO_H
#define WCRT_ERRNO_H

/** @brief Domain error reported by a mathematical function. */
#define EDOM 33

/** @brief Range error reported by a mathematical or conversion function. */
#define ERANGE 34

/** @brief Process-global C89 error indicator storage. */
extern int wcrt_errno;

/** @brief Modifiable lvalue containing the current error indicator. */
#define errno wcrt_errno

#endif
