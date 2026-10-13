/** @file system-types.c @brief Checks system-type identity and isolation. */

#define WCRT_MS_TYPES_ALIASES 1
#if defined(WCRT_TYPES_STAT_FIRST)
#include <sys/stat.h>
#endif
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>

typedef char device_width[sizeof(_dev_t) == 4 ? 1 : -1];
typedef char inode_width[sizeof(_ino_t) == 2 ? 1 : -1];
typedef char offset_width[sizeof(_off_t) == 4 ? 1 : -1];
typedef char device_unsigned[(_dev_t)-1 > 0 ? 1 : -1];
typedef char inode_unsigned[(_ino_t)-1 > 0 ? 1 : -1];
typedef char offset_signed[(_off_t)-1 < 0 ? 1 : -1];
typedef char narrow_status[sizeof(struct wcrt_stat32) == 36 ? 1 : -1];
typedef char wide_status[sizeof(struct _stat64) == 56 ? 1 : -1];
#if defined(WCRT_POSIX)
typedef char posix_inode[sizeof(ino_t) == 8 ? 1 : -1];
typedef char posix_offset[sizeof(off_t) == 8 ? 1 : -1];
#else
typedef char alias_device[sizeof(dev_t) == 4 ? 1 : -1];
typedef char alias_inode[sizeof(ino_t) == 2 ? 1 : -1];
typedef char alias_offset[sizeof(off_t) == 4 ? 1 : -1];
#endif

/** @brief Uses compatible pointers to verify typedef identity. */
int check_system_types(void)
{
    _dev_t device = 7;
    _ino_t inode = 9;
    _off_t offset = -11;
    unsigned int *device_pointer = &device;
    unsigned short *inode_pointer = &inode;
    long *offset_pointer = &offset;
#if !defined(WCRT_POSIX)
    dev_t *device_alias = device_pointer;
    ino_t *inode_alias = inode_pointer;
    off_t *offset_alias = offset_pointer;
    if (*device_alias != 7 || *inode_alias != 9 || *offset_alias != -11)
        return 1;
#endif
    return *device_pointer != 7 || *inode_pointer != 9 ||
        *offset_pointer != -11;
}
