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

/** @brief Invalid argument reported by Microsoft compatibility interfaces. */
#define EINVAL 22

/** @brief Illegal byte sequence reported by character conversion. */
#define EILSEQ 42

/** @brief Insufficient storage reported by an allocating implementation. */
#define ENOMEM 12

/** @brief Permission denied by a filesystem operation. */
#define EACCES 13

/** @brief File or path component was not found. */
#define ENOENT 2

/** @brief Process file-table capacity was exhausted. */
#define EMFILE 24

/** @brief Microsoft secure-CRT error-code type. */
typedef int errno_t;

/** @brief Process-global C89 error indicator storage. */
extern int wcrt_errno;

/** @brief Modifiable lvalue containing the current error indicator. */
#define errno wcrt_errno

#endif
