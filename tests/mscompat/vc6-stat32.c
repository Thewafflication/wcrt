/**
 * @file vc6-stat32.c
 * @brief Calls the selected stat32 route from a separate x86 translation unit.
 */

#define WCRT_VC6_STAT32 1
#include <errno.h>
#include <io.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

typedef char selected_width[sizeof(struct _stat) == 36 ? 1 : -1];
typedef char explicit_width[sizeof(struct _stat64) == 56 ? 1 : -1];
typedef char selected_time_offset[offsetof(struct _stat, st_atime) == 24 ?
    1 : -1];

/** @brief Guarded selected record with a tail for an old-ABI overwrite. */
struct selected_status {
    unsigned char before[8]; /**< Leading guard. */
    struct _stat status; /**< Explicitly selected narrow record. */
    unsigned char after[64]; /**< Large trailing guard. */
};

/** @brief Verifies selected tags, function pointers and explicit wide calls. */
int check_vc6_stat32(void)
{
    struct selected_status selected;
    struct _stat64 wide;
    FILE *stream;
    int index;
    int result = 0;
    int (__cdecl *path_address)(const char *, struct _stat *) = _stat;
    int (__cdecl *handle_address)(int, struct _stat *) = _fstat;
    memset(&selected, 0xa5, sizeof(selected));
    stream = tmpfile();
    if (stream == NULL) return 1;
    errno = EDOM;
    if (fwrite("legacy", 1, 6, stream) != 6 || fflush(stream) != 0 ||
        handle_address(_fileno(stream), &selected.status) != 0 ||
        _fstat64(_fileno(stream), &wide) != 0 || errno != EDOM ||
        selected.status.st_size != 6 || wide.st_size != 6 ||
        selected.status.st_mode != wide.st_mode) result = 2;
    if (fclose(stream) != 0) result = 3;
    errno = 0;
    if (path_address(NULL, &selected.status) != -1 || errno != EINVAL)
        result = 4;
    for (index = 0; index < 8; ++index)
        if (selected.before[index] != 0xa5) result = 5;
    for (index = 0; index < 64; ++index)
        if (selected.after[index] != 0xa5) result = 5;
    return result;
}
