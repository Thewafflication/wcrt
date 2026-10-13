/** @file sentinel-copy.c @brief Checks memory/string include order. */
#include <string.h>
#include <memory.h>
#include <string.h>
#include <memory.h>
/** @brief Retains the public function-pointer signature. */
void *(*sentinel_copy_pointer)(void *, const void *, int, size_t) = _memccpy;
