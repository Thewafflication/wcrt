/** @file descriptor-resize.c @brief Checks the selected resize signature. */
#include <io.h>
#include <io.h>
int (*resize_descriptor)(int, long) = _chsize;
