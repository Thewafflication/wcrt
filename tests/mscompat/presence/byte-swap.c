/** @file byte-swap.c @brief Checks the legacy byte-copy signature. */
#include <stdlib.h>
#include <stdlib.h>
void (__cdecl *copy_byte_pairs)(char *, char *, int) = _swab;
