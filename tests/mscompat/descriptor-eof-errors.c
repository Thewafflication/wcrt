/** @file descriptor-eof-errors.c @brief Injects physical EOF query failures. */
#define WCRT_POSIX 1
#define _filelengthi64 test_eof_size
#define _telli64 test_eof_position
#define _eof test_eof
#include "../../src/ms_eof.c"
#include <errno.h>

static wcrt_ms_int64 provided_size;
static wcrt_ms_int64 provided_position;
static int size_error;
static int position_error;
static int position_calls;

/** @brief Supplies a size result or provider failure. */
wcrt_ms_int64 test_eof_size(int descriptor)
{
    (void)descriptor;
    if (size_error) { errno = size_error; return -1; }
    return provided_size;
}

/** @brief Supplies a position result or provider failure. */
wcrt_ms_int64 test_eof_position(int descriptor)
{
    (void)descriptor;
    ++position_calls;
    if (position_error) { errno = position_error; return -1; }
    return provided_position;
}

int main(void)
{
    provided_size = 9223372036854775807LL;
    provided_position = provided_size;
    errno = EDOM;
    if (test_eof(7) != 1 || errno != EDOM) return 1;
    provided_position = 0;
    if (test_eof(7) != 0 || errno != EDOM) return 2;
    size_error = EACCES;
    position_calls = 0;
    if (test_eof(7) != -1 || errno != EACCES || position_calls != 0)
        return 3;
    size_error = 0;
    position_error = EIO;
    if (test_eof(7) != -1 || errno != EIO || position_calls != 1) return 4;
    return 0;
}
