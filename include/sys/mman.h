/** @file sys/mman.h @brief Declares selected POSIX file mapping interfaces. */
#ifndef WCRT_SYS_MMAN_H
#define WCRT_SYS_MMAN_H
#if defined(WCRT_POSIX)
#include <stddef.h>
#include <sys/types.h>
#define PROT_NONE 0x0
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define MAP_SHARED 0x1
#define MAP_PRIVATE 0x2
#define MAP_FIXED 0x10
#define MAP_FAILED ((void *)-1)
#ifdef __cplusplus
extern "C" {
#endif
void *mmap(void *address, size_t length, int protection, int flags,
    int descriptor, off_t offset);
int munmap(void *address, size_t length);
#ifdef __cplusplus
}
#endif
#endif
#endif
