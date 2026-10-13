/** @file default-translation-errors.c @brief Tests default-mode snapshots. */
#define WCRT_POSIX 1
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#undef __declspec
#define __declspec(attribute)
#define CreateFileA fixture_open
#define GetLastError fixture_error
#include "../../src/platform/windows/file.c"

static int opens;
/** @brief Changes the default during open to prove prior mode selection. */
void *WCRT_WINAPI fixture_open(const char *path, unsigned long access,
    unsigned long sharing, void *security, unsigned long creation,
    unsigned long attributes, void *template_file)
{
    (void)path; (void)access; (void)sharing; (void)security;
    (void)creation; (void)attributes; (void)template_file;
    ++opens;
    _fmode = _O_TEXT;
    return (void *)1;
}
/** @brief Supplies successful new-file classification. */
unsigned long WCRT_WINAPI fixture_error(void) { return 0; }
/** @brief Verifies defaults, overrides and validation before native opens. */
int main(void)
{
    FILE stream;
    int flags = _O_CREAT | _O_RDWR;
    int original = _fmode;
    memset(&stream, 0, sizeof(stream));
    _fmode = _O_BINARY;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD | _S_IWRITE) || opens != 1 ||
        !(stream.flags & WCRT_FILE_BINARY)) return 1;
    _fmode = _O_BINARY;
    if (__wcrt_file_open_shared(&stream, "owned", "w", _SH_DENYNO) ||
        opens != 2 || !(stream.flags & WCRT_FILE_BINARY)) return 2;
    _fmode = 12345;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD | _S_IWRITE) != -1 || errno != EINVAL ||
        opens != 2 || __wcrt_file_open_shared(&stream, "owned", "w",
        _SH_DENYNO) != -1 || errno != EINVAL || opens != 2) return 3;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags | _O_BINARY,
        _SH_DENYNO, _S_IREAD | _S_IWRITE) || opens != 3 ||
        !(stream.flags & WCRT_FILE_BINARY)) return 4;
    _fmode = 12345;
    if (__wcrt_file_open_shared(&stream, "owned", "wt", _SH_DENYNO) ||
        opens != 4 || (stream.flags & WCRT_FILE_BINARY)) return 5;
    _fmode = _O_BINARY;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags | _O_TEXT,
        _SH_DENYNO, _S_IREAD | _S_IWRITE) || opens != 5 ||
        (stream.flags & WCRT_FILE_BINARY)) return 6;
    _fmode = original;
    return 0;
}
