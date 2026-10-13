/** @file find32i64.c @brief Checks wide-size search declarations and layout. */
#include <stddef.h>
#pragma pack(push, 1)
#include <io.h>
#include <io.h>
struct retained_packing { char byte; long value; };
#pragma pack(pop)
typedef char record_size[sizeof(struct wcrt_finddata32i64) == 288 ? 1 : -1];
typedef char size_offset[
    offsetof(struct wcrt_finddata32i64, size) == 16 ? 1 : -1];
typedef char name_offset[
    offsetof(struct wcrt_finddata32i64, name) == 24 ? 1 : -1];
typedef char restored_packing[sizeof(struct retained_packing) == 5 ? 1 : -1];
typedef char size_type[sizeof(_fsize_t) == 4 ? 1 : -1];
typedef char unsigned_size[(_fsize_t)-1 == 0xffffffffUL ? 1 : -1];
#ifdef _findfirsti64
#error Default wide-size filename aliases must be absent
#endif
__wcrt_intptr_t (*first_wide)(const char *, struct wcrt_finddata32i64 *) =
    wcrt_findfirst32i64;
int (*next_wide)(__wcrt_intptr_t, struct wcrt_finddata32i64 *) =
    wcrt_findnext32i64;
