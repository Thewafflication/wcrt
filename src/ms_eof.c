/** @file ms_eof.c @brief Implements the physical descriptor EOF query. */
#include <io.h>

/** @brief Compares independent physical queries without changing FILE state. */
int _eof(int descriptor)
{
    wcrt_ms_int64 size = _filelengthi64(descriptor);
    wcrt_ms_int64 position;
    if (size < 0) return -1;
    position = _telli64(descriptor);
    if (position < 0) return -1;
    return position == size;
}
