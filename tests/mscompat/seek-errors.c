/**
 * @file seek-errors.c
 * @brief Verifies Windows seek error partitions without OS mutations.
 */

#define WCRT_POSIX 1
#include <errno.h>
#include <string.h>

#undef __declspec
#define __declspec(attribute)
#define SetFilePointer test_set_pointer
#define GetFileType test_file_type
#define GetLastError test_last_error
#include "../../src/platform/windows/file.c"

static unsigned long error;
static unsigned long type = 1;
static unsigned long result = WCRT_INVALID_POSITION;
static long result_high;
static int seek_calls;

/** @brief Supplies a deterministic pointer result and high word. */
unsigned long WCRT_WINAPI test_set_pointer(void *handle, long low,
    long *high, unsigned long origin)
{
    (void)handle;
    (void)low;
    (void)origin;
    ++seek_calls;
    *high = result_high;
    return result;
}

/** @brief Supplies a deterministic native handle type. */
unsigned long WCRT_WINAPI test_file_type(void *handle)
{
    (void)handle;
    return type;
}

/** @brief Supplies a deterministic native error. */
unsigned long WCRT_WINAPI test_last_error(void)
{
    return error;
}

int main(void)
{
    FILE stream;
    long long position = 17;
    memset(&stream, 0, sizeof(stream));
    stream.handle = (void *)1;
    stream.end_of_file = 1;
    stream.error = 1;
    stream.pushback = 'X';
    errno = EDOM;
    if (__wcrt_file_seek(&stream, 0, SEEK_SET, &position) != 0 ||
        position != 4294967295LL || errno != EDOM) return 1;
    result_high = 1;
    if (__wcrt_file_seek(&stream, 0, SEEK_SET, &position) != 0 ||
        position != 8589934591LL) return 2;
    position = 17;
    error = WCRT_ERROR_NEGATIVE_SEEK;
    result_high = -1;
    if (__wcrt_file_seek(&stream, -1, SEEK_SET, &position) != -1 ||
        errno != EINVAL || position != 17) return 3;
    result_high = 0;
    error = WCRT_ERROR_INVALID_PARAMETER;
    if (__wcrt_file_seek(&stream, 0, SEEK_SET, &position) != -1 ||
        errno != EINVAL || position != 17) return 4;
    error = WCRT_ERROR_ACCESS_DENIED;
    if (__wcrt_file_seek(&stream, 0, SEEK_SET, &position) != -1 ||
        errno != EACCES) return 5;
    seek_calls = 0;
    if (__wcrt_file_seek(&stream, 0, 99, &position) != -1 ||
        errno != EINVAL || seek_calls != 0) return 6;
    type = 3;
    if (__wcrt_file_seek(&stream, 0, SEEK_CUR, &position) != -1 ||
        errno != ESPIPE || seek_calls != 0) return 7;
    type = 2;
    if (__wcrt_file_seek(&stream, 0, SEEK_CUR, &position) != -1 ||
        errno != ESPIPE || seek_calls != 0) return 8;
    type = 0;
    error = WCRT_ERROR_INVALID_HANDLE;
    if (__wcrt_file_seek(&stream, 0, SEEK_CUR, &position) != -1 ||
        errno != EBADF || seek_calls != 0) return 9;
    if (!stream.end_of_file || !stream.error || stream.pushback != 'X' ||
        position != 17) return 10;
    return 0;
}
