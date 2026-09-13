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

/** @brief A filesystem object already exists. */
#define EEXIST 17

/** @brief A file descriptor is invalid or not open. */
#define EBADF 9

/** @brief A resource is busy. */
#define EBUSY 16

/** @brief A path component is not a directory. */
#define ENOTDIR 20

/** @brief An operation requiring a non-directory received a directory. */
#define EISDIR 21

/** @brief Process file-table capacity was exhausted. */
#define EMFILE 24

/** @brief A device has no space available. */
#define ENOSPC 28

/** @brief A seek was requested on a pipe. */
#define ESPIPE 29

/** @brief A write was requested on a pipe without a reader. */
#define EPIPE 32

/** @brief A directory cannot be removed because it is not empty. */
#define ENOTEMPTY 41

#if defined(WCRT_POSIX)
/** @brief Operation was not permitted. */
#define EPERM 1

/** @brief An operation was interrupted. */
#define EINTR 4

/** @brief A low-level input/output error occurred. */
#define EIO 5

/** @brief The requested device or address was unavailable. */
#define ENXIO 6

/** @brief The operation should be attempted again. */
#define EAGAIN 11

/** @brief The file or requested file size was too large. */
#define EFBIG 27

/** @brief The requested operation is not implemented. */
#define ENOSYS 40
#endif

/** @brief Microsoft secure-CRT error-code type. */
typedef int errno_t;

/** @brief Process-global C89 error indicator storage. */
extern int wcrt_errno;

/** @brief Modifiable lvalue containing the current error indicator. */
#define errno wcrt_errno

#endif
