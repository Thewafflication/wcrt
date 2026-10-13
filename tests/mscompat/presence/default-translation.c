/** @file default-translation.c @brief Checks the default-mode data type. */
#include <stdlib.h>
#include <stdlib.h>
typedef char default_mode_width[sizeof(_fmode) == sizeof(int) ? 1 : -1];
int *default_mode_address = &_fmode;
