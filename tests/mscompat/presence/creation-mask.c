/** @file creation-mask.c @brief Checks the legacy mask setter signature. */
#include <io.h>
#include <io.h>
int (*change_creation_mask)(int) = _umask;
