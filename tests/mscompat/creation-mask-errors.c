/** @file creation-mask-errors.c @brief Provides creation-mask snapshots. */
int fixture_mask_snapshot(void);
#define __wcrt_file_creation_mask fixture_mask_snapshot
#define main check_shared_creation_baseline
#include "shared-descriptor-errors.c"
#undef main

static int supplied_mask, snapshots;
/** @brief Mutates the next snapshot to prove each creation reads only once. */
int fixture_mask_snapshot(void)
{
    int current = supplied_mask;
    supplied_mask = 0;
    ++snapshots;
    return current;
}
/** @brief Verifies snapshot ordering and new/existing/error partitions. */
int main(void)
{
    FILE stream;
    int flags = _O_CREAT | _O_RDWR | _O_BINARY;
    if (check_shared_creation_baseline()) return 1;
    memset(&stream, 0, sizeof(stream));
    open_ok = attribute_ok = 1;
    attributes = 0x20UL;
    open_calls = attribute_calls = close_calls = snapshots = 0;
    error = 0;
    supplied_mask = _S_IWRITE;
    errno = 123;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD | _S_IWRITE) || snapshots != 1 ||
        attribute_calls != 1 || applied != 0x21UL || errno != 123 ||
        !(stream.flags & WCRT_FILE_READ) ||
        !(stream.flags & WCRT_FILE_WRITE)) return 2;
    supplied_mask = _S_IWRITE;
    error = WCRT_ERROR_ALREADY_EXISTS;
    snapshots = attribute_calls = 0;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD | _S_IWRITE) || snapshots != 1 ||
        attribute_calls) return 3;
    supplied_mask = _S_IWRITE;
    snapshots = 0;
    if (__wcrt_file_open_shared_flags(&stream, "owned", _O_RDWR | _O_BINARY,
        _SH_DENYNO, _S_IREAD | _S_IWRITE) || snapshots || attribute_calls)
        return 4;
    open_calls = 0;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IEXEC) != -1 || errno != EINVAL || snapshots ||
        open_calls) return 5;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        -1, _S_IREAD) != -1 || errno != EINVAL || snapshots || open_calls)
        return 6;
    open_ok = 0;
    error = WCRT_ERROR_NOT_ENOUGH_MEMORY;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD | _S_IWRITE) != -1 || errno != ENOMEM ||
        snapshots != 1 || close_calls || attribute_calls) return 7;
    open_ok = 1;
    error = WCRT_ERROR_ACCESS_DENIED;
    attribute_ok = 0;
    supplied_mask = _S_IWRITE;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD | _S_IWRITE) != -1 || errno != EACCES ||
        close_calls != 1 || snapshots != 2 || stream.handle) return 8;
    return 0;
}
