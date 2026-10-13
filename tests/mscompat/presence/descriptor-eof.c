/** @file descriptor-eof.c @brief Checks descriptor EOF declaration type. */
#include <io.h>
#include <io.h>

int (*physical_eof_query)(int) = _eof;
