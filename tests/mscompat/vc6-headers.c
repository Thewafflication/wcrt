/**
 * @file vc6-headers.c
 * @brief Exercises selected VC6 x86 C header spellings and stack varargs.
 */

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <io.h>
#include <sys/locking.h>
#include <direct.h>
#include <fcntl.h>
#include <share.h>
#include <stddef.h>
#include <stdio.h>

int check_vc6_stat32(void);
int check_vc6_macros(void);
int check_system_types(void);
int check_vc6_utime32(void);
int check_vc6_c89_calls(void);
int check_vc6_ms_errors(void);
int check_vc6_calendar_time32(void);
int check_vc6_find32(void);
int check_byte_swap(void);
int check_vc6_default_mode(void);
int check_sentinel_copy(void);

/** @brief Structure exercising offsets and by-value argument slots. */
struct vc6_pair {
    int first;  /**< First integer. */
    int second; /**< Second integer. */
};

typedef char size_width[sizeof(size_t) == 4 ? 1 : -1];
typedef char diff_width[sizeof(ptrdiff_t) == 4 ? 1 : -1];
typedef char wide_width[sizeof(wchar_t) == 2 ? 1 : -1];
typedef char position_width[sizeof(fpos_t) == 8 ? 1 : -1];
typedef char time_width[sizeof(time_t) == 8 ? 1 : -1];
typedef char pair_offset[offsetof(struct vc6_pair, second) == 4 ? 1 : -1];
typedef char stat_width[sizeof(struct _stat64) == 56 ? 1 : -1];
typedef char stamp_width[sizeof(struct __utimbuf64) == 16 ? 1 : -1];

/** @brief Tests scalar promotion, 64-bit slots and aggregate traversal. */
static int check_arguments(int marker, ...)
{
    va_list arguments;
    int character;
    int short_value;
    double floating;
    wcrt_ms_int64 integer;
    const char *text;
    struct vc6_pair pair;
    int trailing;

    va_start(arguments, marker);
    character = va_arg(arguments, int);
    short_value = va_arg(arguments, int);
    floating = va_arg(arguments, double);
    integer = va_arg(arguments, wcrt_ms_int64);
    text = va_arg(arguments, const char *);
    pair = va_arg(arguments, struct vc6_pair);
    trailing = va_arg(arguments, int);
    va_end(arguments);
    return marker != 7 || character != 'A' || short_value != -123 ||
        floating != 1.25 || integer != _I64_MAX || strcmp(text, "text") != 0 ||
        pair.first != 11 || pair.second != 19 || trailing != 42;
}

/** @brief Forwards the selected va_list ABI into a WCRT runtime call. */
static int forward_format(char *buffer, const char *format, ...)
{
    va_list arguments;
    int result;
    va_start(arguments, format);
    result = vsprintf(buffer, format, arguments);
    va_end(arguments);
    return result;
}

/** @brief Calls the added Microsoft interfaces across the x86 DLL ABI. */
static int check_interfaces(void)
{
    char path[_MAX_PATH];
    char number[32];
    char drive[_MAX_DRIVE];
    char directory[_MAX_DIR];
    char name[_MAX_FNAME];
    char extension[_MAX_EXT];
    char bytes[4];
    FILE *stream;
    int descriptor;
    int result;
    int old_mask;
    struct _stat64 created_status;
    char *saved;
    int (*resize_call)(int, long) = _chsize;
    int (*lock_call)(int, int, long) = _locking;
    int (*create_call)(const char *, int) = _creat;
    int (*mask_call)(int) = _umask;

    if (_open_osfhandle(-1, _O_BINARY) != -1 || errno != EBADF ||
        _open_osfhandle(0, _O_TEXT | _O_BINARY) != -1 || errno != EINVAL)
        return 10;
    _makepath(path, "C", "owned", "file", "txt");
    _splitpath(path, drive, directory, name, extension);
    if (strcmp(path, "C:owned\\file.txt") || strcmp(drive, "C:") ||
        strcmp(directory, "owned\\") || strcmp(name, "file") ||
        strcmp(extension, ".txt")) return 1;
    if (_i64toa(_I64_MIN, number, 10) != number ||
        strcmp(number, "-9223372036854775808") ||
        _strtoi64(number, NULL, 10) != _I64_MIN ||
        _atoi64(number) != _I64_MIN) return 2;
    if (_ui64toa(_UI64_MAX, number, 10) != number ||
        strcmp(number, "18446744073709551615") ||
        _strtoui64(number, NULL, 10) != _UI64_MAX) return 3;
    if (tmpnam(path) != path || remove(path) != 0) return 4;
    descriptor = _sopen(path, _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY,
        _SH_DENYNO, _S_IREAD | _S_IWRITE);
    if (descriptor < 0) return 5;
    result = _setmode(descriptor, _O_TEXT) != _O_BINARY ||
        _write(descriptor, "V\n", 2) != 2;
    if (_filelength(descriptor) != 3 || _filelengthi64(descriptor) != 3 ||
        _eof(descriptor) != 1 ||
        _telli64(descriptor) != 3 ||
        _lseeki64(descriptor, (_I64_MAX >> 30), SEEK_SET) !=
        (_I64_MAX >> 30) || _telli64(descriptor) != (_I64_MAX >> 30) ||
        _filelengthi64(descriptor) != 3 || _eof(descriptor) != 0 ||
        _lseeki64(descriptor, 0, SEEK_END) != 3) result = 1;
    errno = EDOM;
    if (resize_call(descriptor, 4) || errno != EDOM ||
        _filelength(descriptor) != 4 || _telli64(descriptor) != 3 ||
        _setmode(descriptor, _O_BINARY) != _O_TEXT ||
        _read(descriptor, bytes, 1) != 1 || bytes[0] != 0 ||
        resize_call(descriptor, 3) || _telli64(descriptor) != 4) result = 1;
    errno = EDOM;
    if (lock_call(descriptor, _LK_NBLCK, 1) || errno != EDOM ||
        _telli64(descriptor) != 4 || _filelength(descriptor) != 3 ||
        lock_call(descriptor, _LK_UNLCK, 1)) result = 1;
    if (_close(descriptor) != 0) result = 1;
    stream = _fsopen(path, "rb", _SH_DENYWR);
    if (stream == NULL) result = 1;
    else {
        if (fread(bytes, 1, 3, stream) != 3 ||
            memcmp(bytes, "V\r\n", 3)) result = 1;
        if (fclose(stream) != 0) result = 1;
    }
    if (remove(path) != 0 || result) return 6;
    descriptor = create_call(path, _S_IREAD | _S_IWRITE);
    if (descriptor < 0 || _write(descriptor, "C", 1) != 1 ||
        _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 1) != 1 || bytes[0] != 'C' ||
        _close(descriptor) || remove(path)) return 11;
    old_mask = mask_call(_S_IWRITE | 0x40000000);
    descriptor = create_call(path, _S_IREAD | _S_IWRITE);
    result = descriptor < 0;
    if (descriptor >= 0 &&
        (_write(descriptor, "M", 1) != 1 || _close(descriptor))) result = 1;
    if (mask_call(old_mask) != _S_IWRITE || _stat64(path, &created_status) ||
        (created_status.st_mode & _S_IWRITE) ||
        _chmod(path, _S_IREAD | _S_IWRITE) || remove(path) || result) return 12;

    saved = _getcwd(NULL, 0);
    if (saved == NULL) return 7;
    descriptor = _getdrive();
    if (descriptor < 0 || descriptor > 26 || _getdrives() == 0 ||
        (descriptor > 0 &&
        !(_getdrives() & (1UL << (descriptor - 1))))) {
        free(saved);
        return 10;
    }
    if (tmpnam(path) != path || remove(path) != 0 || _mkdir(path) != 0) {
        free(saved);
        return 8;
    }
    result = _chdir(path) != 0;
    if (!result) {
        char current[_MAX_PATH];
        if (_getcwd(current, _MAX_PATH) == NULL ||
            _stricmp(current, path)) result = 1;
        if (_getdcwd(0, current, _MAX_PATH) != current ||
            _stricmp(current, path)) result = 1;
        descriptor = _getdrive();
        if (descriptor != 0 && _chdrive(descriptor) != 0) result = 1;
    }
    if (_chdir(saved) != 0) result = 1;
    free(saved);
    if (_rmdir(path) != 0 || result) return 9;
    return 0;
}

int main(void)
{
    struct vc6_pair pair;
    char buffer[80];
    int index;
    pair.first = 11;
    pair.second = 19;
    for (index = 0; index < 100; ++index) {
        if (check_arguments(7, (char)'A', (short)-123, (float)1.25,
            _I64_MAX, "text", pair, 42)) return 1;
    }
    if (forward_format(buffer, "%I64d/%d/%.2f", _I64_MAX, 42, 1.25) != 27 ||
        strcmp(buffer, "9223372036854775807/42/1.25") != 0) return 2;
    errno = 0;
    if (fflush(stdout) != 0 || ferror(stderr) || feof(stdin)) return 3;
    if (check_interfaces()) return 4;
    if (check_vc6_stat32()) return 5;
    if (check_vc6_macros()) return 6;
    if (check_system_types()) return 7;
    if (check_vc6_utime32()) return 8;
    if (check_vc6_c89_calls()) return 9;
    if (check_vc6_ms_errors()) return 10;
    if (check_vc6_calendar_time32()) return 11;
    if (check_vc6_find32()) return 12;
    if (check_byte_swap()) return 13;
    if (check_vc6_default_mode()) return 14;
    if (check_sentinel_copy()) return 15;
    return 0;
}
