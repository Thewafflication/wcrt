/** @file vc6-utime32.c @brief Exercises an explicit narrow file-time route. */
#define WCRT_VC6_UTIME32 1
#define WCRT_VC6_STAT32 1
#include <sys/utime.h>
#include <sys/utime.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

typedef char legacy_times_width[sizeof(struct _utimbuf) == 8 ? 1 : -1];
typedef char legacy_access_offset[offsetof(struct _utimbuf, actime) == 0 ?
    1 : -1];
typedef char legacy_write_offset[offsetof(struct _utimbuf, modtime) == 4 ?
    1 : -1];
typedef char retained_wide_times[sizeof(struct __utimbuf64) == 16 ? 1 : -1];
typedef char retained_calendar[sizeof(time_t) == 8 ? 1 : -1];

/** @brief Guards both sides of the selected narrow input. */
struct guarded_times {
    unsigned char before[8]; /**< Leading guard. */
    struct _utimbuf times; /**< Selected input. */
    unsigned char after[8]; /**< Trailing guard. */
};

/** @brief Checks selected and explicit time/status calls in one consumer. */
int check_vc6_utime32(void)
{
    char path[L_tmpnam];
    struct guarded_times input;
    unsigned char saved[sizeof(input)];
    struct _stat narrow;
    struct _stat64 wide;
    struct __utimbuf64 explicit_times;
    time_t before;
    time_t after;
    FILE *stream;
    int result = 0;
    int (__cdecl *selected_call)(const char *, struct _utimbuf *) = _utime;
    int (__cdecl *selected_descriptor)(int, struct _utimbuf *) = _futime;
    if (tmpnam(path) != path) return 1;
    stream = fopen(path, "wb");
    if (stream == NULL) { remove(path); return 2; }
    if (fwrite("old", 1, 3, stream) != 3) result = 3;
    if (fclose(stream) != 0) result = 3;
    memset(&input, 0xa5, sizeof(input));
    memset(&narrow, 0, sizeof(narrow));
    input.times.actime = 946684800L;
    input.times.modtime = 2147483647L;
    memcpy(saved, &input, sizeof(input));
    errno = EDOM;
    if (!result && (selected_call(path, &input.times) != 0 ||
        errno != EDOM || memcmp(saved, &input, sizeof(input)) ||
        _stat(path, &narrow) != 0 || _stat64(path, &wide) != 0 ||
        narrow.st_size != 3 || narrow.st_atime != input.times.actime ||
        narrow.st_mtime != input.times.modtime ||
        wide.st_mtime != input.times.modtime)) result = 4;
    before = time(NULL);
    if (!result && (selected_call(path, NULL) != 0 ||
        _stat(path, &narrow) != 0)) result = 5;
    after = time(NULL);
    if (!result && (narrow.st_mtime < before || narrow.st_mtime > after))
        result = 6;
    explicit_times.actime = (wcrt_ms_int64)2147483647L + 1;
    explicit_times.modtime = explicit_times.actime;
    if (!result && (_utime64(path, &explicit_times) != 0 ||
        _stat64(path, &wide) != 0 ||
        wide.st_mtime != explicit_times.modtime ||
        _stat(path, &narrow) != -1 || errno != ERANGE)) result = 7;
    stream = fopen(path, "r+b");
    if (stream == NULL) result = 12;
    else {
        errno = EDOM;
        if (!result && (_futime64(_fileno(stream), &explicit_times) != 0 ||
            errno != EDOM || _fstat64(_fileno(stream), &wide) != 0 ||
            wide.st_mtime != explicit_times.modtime || ftell(stream) != 0))
            result = 13;
        if (fclose(stream) != 0) result = 14;
    }
    stream = fopen(path, "r+b");
    if (stream == NULL) result = 8;
    else {
        errno = EDOM;
        if (!result && (selected_descriptor(_fileno(stream), &input.times) ||
            errno != EDOM || memcmp(saved, &input, sizeof(input)) ||
            _stat(path, &narrow) != 0 ||
            narrow.st_mtime != input.times.modtime)) result = 9;
        if (fclose(stream) != 0) result = 10;
    }
    if (remove(path) != 0) result = 11;
    return result;
}
