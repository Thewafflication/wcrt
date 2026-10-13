/** @file descriptor-resize-errors.c @brief Models resize errors and limits. */
#define WCRT_POSIX 1
#include <errno.h>
#include <string.h>
#undef __declspec
#define __declspec(attribute)
#define SetFilePointer test_resize_pointer
#define GetFileType test_resize_type
#define GetLastError test_resize_error
#define SetEndOfFile test_resize_end
#define WriteFile test_resize_write
#define _fstat64 test_resize_status
#include "../../src/platform/windows/file.c"

static long long physical_position;
static long long physical_size;
static unsigned long native_error;
static int seek_calls;
static int write_calls;
static int end_calls;
static int fail_seek_call;
static int fail_end;
static int fail_write_call;
static int zero_write;
static int oversized_write;
static unsigned long short_write;
static int metadata_failure;
static unsigned short metadata_mode;
static int contract_error;

/** @brief Models a signed 64-bit position and selected seek failures. */
unsigned long WCRT_WINAPI test_resize_pointer(void *handle, long low,
    long *high, unsigned long origin)
{
    long long offset = (long long)(((unsigned long long)(unsigned long)*high
        << 32) | (unsigned long)low);
    if (handle != (void *)7 || origin > 2) contract_error = 1;
    ++seek_calls;
    if (seek_calls == fail_seek_call) {
        native_error = WCRT_ERROR_ACCESS_DENIED;
        return WCRT_INVALID_POSITION;
    }
    if (origin == SEEK_CUR) offset += physical_position;
    if (origin == SEEK_END) offset += physical_size;
    physical_position = offset;
    *high = (long)(offset >> 32);
    native_error = 0;
    return (unsigned long)offset;
}

/** @brief Supplies a disk handle without dereferencing the fake pointer. */
unsigned long WCRT_WINAPI test_resize_type(void *handle)
{
    if (handle != (void *)7) contract_error = 1;
    return 1;
}

/** @brief Supplies the latest provider failure. */
unsigned long WCRT_WINAPI test_resize_error(void) { return native_error; }

/** @brief Records truncation or a denied end-of-file operation. */
int WCRT_WINAPI test_resize_end(void *handle)
{
    if (handle != (void *)7) contract_error = 1;
    ++end_calls;
    if (fail_end) { native_error = WCRT_ERROR_DISK_FULL; return 0; }
    physical_size = physical_position;
    return 1;
}

/** @brief Models partial/failed writes while checking raw zero bytes. */
int WCRT_WINAPI test_resize_write(void *handle, const void *buffer,
    unsigned long count, unsigned long *amount, void *overlapped)
{
    unsigned long index;
    if (handle != (void *)7 || overlapped != NULL || count == 0 ||
        count > 512) contract_error = 1;
    for (index = 0; index < count; ++index) {
        if (((const unsigned char *)buffer)[index]) contract_error = 1;
    }
    ++write_calls;
    if (write_calls == fail_write_call) {
        *amount = 0;
        native_error = WCRT_ERROR_DISK_FULL;
        return 0;
    }
    *amount = zero_write ? 0 : count;
    if (short_write && *amount > short_write) *amount = short_write;
    if (oversized_write) *amount = count + 1;
    physical_position += *amount;
    if (physical_position > physical_size) physical_size = physical_position;
    return 1;
}

/** @brief Supplies size/type or a query failure for the fake descriptor. */
int __cdecl test_resize_status(int descriptor, struct _stat64 *status)
{
    if (descriptor != 7) contract_error = 1;
    if (metadata_failure) { errno = EACCES; return -1; }
    memset(status, 0, sizeof(*status));
    status->st_mode = metadata_mode;
    status->st_size = physical_size;
    return 0;
}

/** @brief Resets the owned provider model with distinctive stream state. */
static void reset(FILE *stream)
{
    memset(stream, 0, sizeof(*stream));
    stream->handle = (void *)7;
    stream->descriptor = 7;
    stream->flags = WCRT_FILE_WRITE | WCRT_FILE_APPEND;
    stream->end_of_file = stream->error = 1;
    stream->pushback = 'X';
    physical_position = 8589934591LL;
    physical_size = 3;
    native_error = 0;
    seek_calls = write_calls = end_calls = 0;
    fail_seek_call = fail_end = fail_write_call = 0;
    zero_write = oversized_write = metadata_failure = contract_error = 0;
    short_write = 0;
    metadata_mode = S_IFREG;
    errno = EDOM;
}

int main(void)
{
    FILE stream;
    FILE before;
    reset(&stream);
    before = stream;
    short_write = 7;
    if (__wcrt_file_resize(&stream, 1025) || errno != EDOM ||
        physical_size != 1025 || physical_position != 8589934591LL ||
        write_calls < 2 || end_calls || contract_error ||
        memcmp(&before, &stream, sizeof(stream))) return 1;
    reset(&stream);
    physical_size = 2147483646LL;
    if (__wcrt_file_resize(&stream, 2147483647L) || write_calls != 1 ||
        physical_size != 2147483647LL || physical_position != 8589934591LL)
        return 2;
    reset(&stream);
    physical_size = 9223372036854775807LL;
    if (__wcrt_file_resize(&stream, 2147483647L) || end_calls != 1 ||
        write_calls || physical_size != 2147483647LL ||
        physical_position != 8589934591LL) return 3;
    reset(&stream);
    fail_write_call = 2;
    if (__wcrt_file_resize(&stream, 1025) != -1 || errno != ENOSPC ||
        physical_size != 515 || physical_position != 8589934591LL) return 4;
    reset(&stream);
    zero_write = 1;
    if (__wcrt_file_resize(&stream, 5) != -1 || errno != ENOSPC ||
        write_calls != 1 || physical_position != 8589934591LL) return 5;
    reset(&stream);
    oversized_write = 1;
    if (__wcrt_file_resize(&stream, 5) != -1 || errno != EIO ||
        physical_position != 8589934591LL) return 6;
    reset(&stream);
    fail_end = 1;
    fail_seek_call = 3;
    if (__wcrt_file_resize(&stream, 1) != -1 || errno != ENOSPC ||
        physical_size != 3 || physical_position != 1) return 7;
    reset(&stream);
    fail_seek_call = 3;
    if (__wcrt_file_resize(&stream, 1) != -1 || errno != EACCES ||
        physical_size != 1 || physical_position != 1) return 8;
    reset(&stream);
    metadata_mode = S_IFDIR;
    if (__wcrt_file_resize(&stream, 1) != -1 || errno != EINVAL ||
        seek_calls || write_calls || end_calls) return 9;
    reset(&stream);
    stream.flags = WCRT_FILE_READ;
    if (__wcrt_file_resize(&stream, 1) != -1 || errno != EACCES || seek_calls)
        return 10;
    reset(&stream);
    metadata_failure = 1;
    if (__wcrt_file_resize(&stream, 1) != -1 || errno != EACCES || seek_calls)
        return 11;
    reset(&stream);
    fail_seek_call = 1;
    if (__wcrt_file_resize(&stream, 1) != -1 || errno != EACCES ||
        write_calls || end_calls) return 12;
    reset(&stream);
    if (__wcrt_file_resize(&stream, -1) != -1 || errno != EINVAL || seek_calls)
        return 13;
    reset(&stream);
    fail_seek_call = 2;
    if (__wcrt_file_resize(&stream, 1) != -1 || errno != EACCES ||
        physical_position != 8589934591LL || physical_size != 3 ||
        seek_calls != 2 || write_calls || end_calls) return 14;
    return 0;
}
