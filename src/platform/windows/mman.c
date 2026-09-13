/** @file mman.c @brief Implements read-only Windows file mappings. */
#define WCRT_POSIX 1
#include <errno.h>
#include <io.h>
#include <sys/mman.h>

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif
#define WCRT_PAGE_READONLY 0x02UL
#define WCRT_FILE_MAP_READ 0x04UL
__declspec(dllimport) void *WCRT_WINAPI CreateFileMappingA(void *file,
    void *attributes, unsigned long protection, unsigned long maximum_high,
    unsigned long maximum_low, const char *name);
__declspec(dllimport) void *WCRT_WINAPI MapViewOfFile(void *mapping,
    unsigned long access, unsigned long offset_high, unsigned long offset_low,
    size_t bytes);
__declspec(dllimport) int WCRT_WINAPI UnmapViewOfFile(const void *address);
__declspec(dllimport) int WCRT_WINAPI CloseHandle(void *handle);

void *mmap(void *address, size_t length, int protection, int flags,
    int descriptor, off_t offset)
{
    void *file;
    void *mapping;
    void *view;
    unsigned long long position;
    (void)address;
    if (length == 0 || offset < 0 || protection != PROT_READ ||
        (flags & MAP_FIXED) != 0 ||
        ((flags & (MAP_SHARED | MAP_PRIVATE)) != MAP_SHARED &&
        (flags & (MAP_SHARED | MAP_PRIVATE)) != MAP_PRIVATE)) {
        errno = EINVAL;
        return MAP_FAILED;
    }
    file = (void *)_get_osfhandle(descriptor);
    if (file == (void *)-1) {
        errno = EBADF;
        return MAP_FAILED;
    }
    mapping = CreateFileMappingA(file, NULL, WCRT_PAGE_READONLY, 0, 0, NULL);
    if (mapping == NULL) {
        errno = EACCES;
        return MAP_FAILED;
    }
    position = (unsigned long long)offset;
    view = MapViewOfFile(mapping, WCRT_FILE_MAP_READ,
        (unsigned long)(position >> 32), (unsigned long)position, length);
    CloseHandle(mapping);
    if (view == NULL) {
        errno = EINVAL;
        return MAP_FAILED;
    }
    return view;
}

int munmap(void *address, size_t length)
{
    if (address == NULL || address == MAP_FAILED || length == 0) {
        errno = EINVAL;
        return -1;
    }
    if (!UnmapViewOfFile(address)) {
        errno = EINVAL;
        return -1;
    }
    return 0;
}
