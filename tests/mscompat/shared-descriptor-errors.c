/**
 * @file shared-descriptor-errors.c
 * @brief Tests creation classification and attribute failure cleanup.
 */

#define WCRT_POSIX 1
#include <errno.h>
#include <string.h>

#undef __declspec
#define __declspec(attribute)
#define CreateFileA test_create_file
#define CloseHandle test_close_handle
#define GetLastError test_last_error
#define GetFileAttributesA test_attributes
#define SetFileAttributesA test_set_attributes
#include "../../src/platform/windows/file.c"

static unsigned long error;
static unsigned long attributes = WCRT_NORMAL_ATTRIBUTE;
static unsigned long applied;
static int attribute_ok = 1;
static int open_ok = 1;
static int open_calls;
static int close_calls;
static int attribute_calls;

/** @brief Supplies a fake open handle without filesystem access. */
void *WCRT_WINAPI test_create_file(const char *path, unsigned long access,
    unsigned long sharing, void *security, unsigned long creation,
    unsigned long flags, void *template_file)
{
    (void)path;
    (void)access;
    (void)sharing;
    (void)security;
    (void)creation;
    (void)flags;
    (void)template_file;
    ++open_calls;
    return open_ok ? (void *)1 : WCRT_INVALID_HANDLE;
}

/** @brief Records cleanup of a fake handle and changes the OS error. */
int WCRT_WINAPI test_close_handle(void *handle)
{
    (void)handle;
    ++close_calls;
    error = WCRT_ERROR_INVALID_HANDLE;
    return 1;
}

/** @brief Supplies the deterministic Windows last-error value. */
unsigned long WCRT_WINAPI test_last_error(void)
{
    return error;
}

/** @brief Supplies configurable existing file attributes. */
unsigned long WCRT_WINAPI test_attributes(const char *path)
{
    (void)path;
    return attributes;
}

/** @brief Records permissions without changing any filesystem path. */
int WCRT_WINAPI test_set_attributes(const char *path, unsigned long flags)
{
    (void)path;
    ++attribute_calls;
    applied = flags;
    return attribute_ok;
}

int main(void)
{
    FILE stream;
    int flags = _O_CREAT | _O_RDWR | _O_BINARY;
    memset(&stream, 0, sizeof(stream));
    error = WCRT_ERROR_ALREADY_EXISTS;
    errno = EDOM;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD) != 0 || attribute_calls != 0 ||
        errno != EDOM) return 1;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags | _O_EXCL,
        _SH_DENYNO, _S_IREAD) != 0 || attribute_calls != 1 ||
        applied != (attributes | WCRT_READONLY_ATTRIBUTE)) return 2;
    error = 0;
    attributes = 0x20UL;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD) != 0 || attribute_calls != 2 ||
        applied != 0x21UL) return 3;
    attribute_ok = 0;
    error = WCRT_ERROR_ACCESS_DENIED;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD) != -1 || stream.handle != NULL ||
        close_calls != 1 || errno != EACCES) return 4;
    attributes = WCRT_INVALID_ATTRIBUTES;
    error = WCRT_ERROR_FILE_NOT_FOUND;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IREAD) != -1 || stream.handle != NULL ||
        close_calls != 2 || attribute_calls != 3 || errno != ENOENT)
        return 5;
    open_ok = 0;
    error = WCRT_ERROR_NOT_ENOUGH_MEMORY;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IWRITE) != -1 || errno != ENOMEM ||
        close_calls != 2) return 6;
    open_calls = 0;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        _SH_DENYNO, _S_IEXEC) != -1 || errno != EINVAL || open_calls)
        return 7;
    if (__wcrt_file_open_shared_flags(&stream, "owned", flags,
        -1, _S_IREAD) != -1 || errno != EINVAL || open_calls) return 8;
    return 0;
}
