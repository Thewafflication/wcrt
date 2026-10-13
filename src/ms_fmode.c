/** @file ms_fmode.c @brief Defines default file-translation mode storage. */
#include <stdlib.h>
#include <fcntl.h>

/** @brief Addressable process default, initialized to legacy text mode. */
int _fmode = _O_TEXT;
