/** @file ms_umask.c @brief Implements the process low-level creation mask. */
#include <io.h>
#include <sys/stat.h>
#include "internal/file.h"

static volatile int wcrt_creation_mask;

/** @brief Silently ignores unsupported bits while preserving errno. */
int _umask(int permissions)
{
    int previous = wcrt_creation_mask;
    wcrt_creation_mask = permissions & (_S_IREAD | _S_IWRITE);
    return previous;
}
/** @brief Supplies one current mask snapshot to the creation backend. */
int __wcrt_file_creation_mask(void) { return wcrt_creation_mask; }
