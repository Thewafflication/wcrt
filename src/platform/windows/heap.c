/**
 * @file heap.c
 * @brief Implements allocation with the Windows process heap.
 */

#include "../../internal/stdlib.h"

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_HEAP_ZERO_MEMORY 0x00000008UL

__declspec(dllimport) void *WCRT_WINAPI GetProcessHeap(void);
__declspec(dllimport) void *WCRT_WINAPI HeapAlloc(void *heap,
    unsigned long flags, size_t size);
__declspec(dllimport) int WCRT_WINAPI HeapFree(void *heap,
    unsigned long flags, void *memory);
__declspec(dllimport) void *WCRT_WINAPI HeapReAlloc(void *heap,
    unsigned long flags, void *memory, size_t size);

void *__wcrt_heap_allocate(size_t size, int zeroed)
{
    unsigned long flags = zeroed ? WCRT_HEAP_ZERO_MEMORY : 0;
    return HeapAlloc(GetProcessHeap(), flags, size == 0 ? 1 : size);
}

void *__wcrt_heap_reallocate(void *memory, size_t size)
{
    if (memory == NULL) {
        return __wcrt_heap_allocate(size, 0);
    }
    return HeapReAlloc(GetProcessHeap(), 0, memory, size == 0 ? 1 : size);
}

void __wcrt_heap_free(void *memory)
{
    if (memory != NULL) {
        HeapFree(GetProcessHeap(), 0, memory);
    }
}
