/** @file native-handle-errors.c @brief Tests adoption width and failures. */
#define __wcrt_file_handle_type test_handle_type
#define __wcrt_adopt_file_handle test_handle_adopt
#include "../../src/ms_handle.c"

static int provided_type = 1;
static int allocation_failure;
static int adoption_calls;
static void *seen_handle;
static unsigned int seen_flags;
static FILE supplied_stream;

/** @brief Supplies a controlled native type while recording every bit. */
int test_handle_type(void *handle)
{
    seen_handle = handle;
    if (provided_type == 0) errno = EBADF;
    return provided_type;
}

/** @brief Supplies allocation success/failure without owning an OS handle. */
FILE *test_handle_adopt(void *handle, unsigned int flags)
{
    ++adoption_calls;
    seen_handle = handle;
    seen_flags = flags;
    if (allocation_failure) { errno = EMFILE; return NULL; }
    supplied_stream.descriptor = 7;
    supplied_stream.flags = flags | WCRT_FILE_OWNED | WCRT_FILE_BINARY;
    return &supplied_stream;
}

int main(void)
{
    __wcrt_intptr_t native;
#if defined(__x86_64__) || defined(__aarch64__)
    native = (__wcrt_intptr_t)0x8000000100000007ULL;
#else
    native = (__wcrt_intptr_t)0xf0000007UL;
#endif
    errno = EDOM;
    if (_open_osfhandle(native, _O_RDWR | _O_TEXT) != 7 ||
        errno != EDOM || seen_handle != (void *)native ||
        seen_flags != (WCRT_FILE_READ | WCRT_FILE_WRITE) ||
        (supplied_stream.flags & WCRT_FILE_BINARY) != 0 ||
        (supplied_stream.flags & WCRT_FILE_OWNED) == 0) return 1;
    if (_open_osfhandle(native, _O_WRONLY | _O_APPEND) != 7 ||
        seen_flags != (WCRT_FILE_WRITE | WCRT_FILE_APPEND) ||
        (supplied_stream.flags & WCRT_FILE_BINARY) == 0) return 2;
    adoption_calls = 0;
    if (_open_osfhandle(native, 3) != -1 || errno != EINVAL ||
        adoption_calls != 0) return 3;
    provided_type = 0;
    if (_open_osfhandle(native, _O_BINARY) != -1 || errno != EBADF ||
        adoption_calls != 0) return 4;
    provided_type = 3;
    if (_open_osfhandle(native, _O_APPEND) != -1 || errno != EINVAL ||
        adoption_calls != 0) return 5;
    provided_type = 2;
    if (_open_osfhandle(native, _O_TEXT) != 7 ||
        seen_flags != WCRT_FILE_READ) return 6;
    provided_type = 1;
    allocation_failure = 1;
    if (_open_osfhandle(native, _O_RDWR) != -1 || errno != EMFILE ||
        seen_handle != (void *)native) return 7;
    return 0;
}
