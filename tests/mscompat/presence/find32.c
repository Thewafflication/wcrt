/** @file find32.c @brief Checks default narrow search declarations. */
#include <stddef.h>
#pragma pack(push, 1)
#include <io.h>
#include <io.h>
struct packing_restored { char byte; long value; };
#pragma pack(pop)
typedef char record_size[sizeof(struct wcrt_finddata32) == 280 ? 1 : -1];
typedef char name_offset[offsetof(struct wcrt_finddata32, name) == 20 ? 1 : -1];
typedef char packing_size[sizeof(struct packing_restored) == 5 ? 1 : -1];
#ifdef _findfirst
#error Default filename search aliases must be absent
#endif
__wcrt_intptr_t (*first_search)(const char *, struct wcrt_finddata32 *) =
    wcrt_findfirst32;
int (*next_search)(__wcrt_intptr_t, struct wcrt_finddata32 *) = wcrt_findnext32;
int (*close_search)(__wcrt_intptr_t) = wcrt_findclose;
