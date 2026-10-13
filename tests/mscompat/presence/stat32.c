/**
 * @file stat32.c
 * @brief Verifies every stat32 offset and caller packing restoration.
 */

#include <stddef.h>
#pragma pack(push, 1)
#include <sys/stat.h>
#include <sys/stat.h>

/** @brief Probes the caller's packing after the public header. */
struct caller_pack {
    char first; /**< Byte before the integer. */
    int second; /**< Integer remains packed by the caller. */
};
#pragma pack(pop)

#define CHECK_OFFSET(member, expected) \
    typedef char offset_##member[offsetof(struct wcrt_stat32, member) == \
        expected ? 1 : -1]
CHECK_OFFSET(st_dev, 0);
CHECK_OFFSET(st_ino, 4);
CHECK_OFFSET(st_mode, 6);
CHECK_OFFSET(st_nlink, 8);
CHECK_OFFSET(st_uid, 10);
CHECK_OFFSET(st_gid, 12);
CHECK_OFFSET(st_rdev, 16);
CHECK_OFFSET(st_size, 20);
CHECK_OFFSET(st_atime, 24);
CHECK_OFFSET(st_mtime, 28);
CHECK_OFFSET(st_ctime, 32);
typedef char stat32_width[sizeof(struct wcrt_stat32) == 36 ? 1 : -1];
typedef char stat64_width[sizeof(struct _stat64) == 56 ? 1 : -1];
typedef char restored_pack[offsetof(struct caller_pack, second) == 1 ? 1 : -1];

static int (__cdecl *path_address)(const char *, struct wcrt_stat32 *) =
    wcrt_stat32;
static int (__cdecl *handle_address)(int, struct wcrt_stat32 *) = wcrt_fstat32;

int main(void)
{
    return path_address == 0 || handle_address == 0;
}
