/** @file native-handle.c @brief Checks handle-adoption signature and width. */
#include <io.h>
#include <io.h>

typedef char handle_width[sizeof(__wcrt_intptr_t) == sizeof(void *) ? 1 : -1];
int (*adopt_native_handle)(__wcrt_intptr_t, int) = _open_osfhandle;
