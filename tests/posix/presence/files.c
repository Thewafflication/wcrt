/** @file files.c @brief Checks selected POSIX file declarations. */
#define WCRT_POSIX 1
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>

#ifndef WCRT_SYS_TYPES_H
#error <sys/types.h> did not resolve to the WCRT header
#endif
#ifndef WCRT_SYS_STAT_H
#error <sys/stat.h> did not resolve to the WCRT header
#endif
#ifndef WCRT_UTIME_H
#error <utime.h> did not resolve to the WCRT header
#endif
#ifdef _POSIX_VERSION
#error A bounded WCRT profile must not advertise _POSIX_VERSION
#endif
#if S_IFMT != 0xF000U || S_IFDIR != 0x4000U || S_IFREG != 0x8000U
#error POSIX file type constants do not match the selected Windows mapping
#endif
#if S_IRUSR != 0x0100U || S_IWUSR != 0x0080U || S_IXUSR != 0x0040U
#error POSIX owner permission constants do not match the Windows mapping
#endif
#if S_IRWXU != (S_IRUSR | S_IWUSR | S_IXUSR)
#error POSIX combined owner permissions are inconsistent
#endif

/** @brief Verifies the selected device identifier width. */
typedef char posix_dev_width[sizeof(dev_t) == 4 ? 1 : -1];
/** @brief Verifies the selected inode identifier width. */
typedef char posix_inode_width[sizeof(ino_t) == 8 ? 1 : -1];
/** @brief Verifies the selected mode width. */
typedef char posix_mode_width[sizeof(mode_t) == 4 ? 1 : -1];
/** @brief Verifies the selected link-count width. */
typedef char posix_link_width[sizeof(nlink_t) == 4 ? 1 : -1];
/** @brief Verifies the selected owner identifier width. */
typedef char posix_owner_width[sizeof(uid_t) == 4 ? 1 : -1];
/** @brief Verifies the selected group identifier width. */
typedef char posix_group_width[sizeof(gid_t) == 4 ? 1 : -1];
/** @brief Verifies the selected offset width. */
typedef char posix_offset_width[sizeof(off_t) == 8 ? 1 : -1];
/** @brief Verifies the selected timestamp seconds width. */
typedef char posix_time_width[sizeof(time_t) == 8 ? 1 : -1];
/** @brief Verifies nanoseconds follow the seconds member. */
typedef char posix_nanosecond_order[
    offsetof(struct timespec, tv_nsec) >
    offsetof(struct timespec, tv_sec) ? 1 : -1];
/** @brief Verifies file size follows device metadata. */
typedef char posix_stat_order[
    offsetof(struct stat, st_size) >
    offsetof(struct stat, st_rdev) ? 1 : -1];
/** @brief Verifies the selected directory mode test. */
typedef char posix_directory_test[
    S_ISDIR(S_IFDIR) && !S_ISDIR(S_IFREG) ? 1 : -1];
/** @brief Verifies the selected regular-file mode test. */
typedef char posix_regular_test[
    S_ISREG(S_IFREG) && !S_ISREG(S_IFDIR) ? 1 : -1];

/** @brief Selected stat function signature. */
typedef int (*posix_stat_fn)(const char *, struct stat *);
/** @brief Selected utime function signature. */
typedef int (*posix_utime_fn)(const char *, const struct utimbuf *);
/** @brief Forces type checking of stat. */
static posix_stat_fn posix_stat_address = stat;
/** @brief Forces type checking of utime. */
static posix_utime_fn posix_utime_address = utime;
