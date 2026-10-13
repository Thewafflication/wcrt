/** @file ms_handle.c @brief Implements Microsoft native-handle adoption. */
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include "internal/file.h"

/** @brief Validates flags/handle before transferring native ownership. */
int _open_osfhandle(__wcrt_intptr_t handle, int flags)
{
    unsigned int access;
    FILE *stream;
    int type;
    if ((flags & ~(_O_WRONLY | _O_RDWR | _O_APPEND | _O_TEXT |
        _O_BINARY)) != 0 || (flags & 3) == 3 ||
        (flags & (_O_TEXT | _O_BINARY)) == (_O_TEXT | _O_BINARY)) {
        errno = EINVAL;
        return -1;
    }
    type = __wcrt_file_handle_type((void *)handle);
    if (type == 0) return -1;
    if ((flags & _O_APPEND) && type != 1) {
        errno = EINVAL;
        return -1;
    }
    access = (flags & 3) == _O_WRONLY ? WCRT_FILE_WRITE : WCRT_FILE_READ;
    if ((flags & 3) == _O_RDWR) access |= WCRT_FILE_WRITE;
    if (flags & _O_APPEND) access |= WCRT_FILE_APPEND;
    stream = __wcrt_adopt_file_handle((void *)handle, access);
    if (stream == NULL) return -1;
    if (flags & _O_TEXT) stream->flags &= ~WCRT_FILE_BINARY;
    return stream->descriptor;
}
