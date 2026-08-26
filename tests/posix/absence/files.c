/** @file files.c @brief Checks strict isolation of POSIX file names. */
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>

#ifdef S_IFMT
#error POSIX mode constants leaked into the strict WCRT profile
#endif
#ifdef S_ISREG
#error POSIX mode tests leaked into the strict WCRT profile
#endif

/** @brief Strict-profile placeholder proving dev_t is absent. */
typedef char dev_t;
/** @brief Strict-profile placeholder proving ino_t is absent. */
typedef char ino_t;
/** @brief Strict-profile placeholder proving mode_t is absent. */
typedef char mode_t;
/** @brief Strict-profile placeholder proving nlink_t is absent. */
typedef char nlink_t;
/** @brief Strict-profile placeholder proving uid_t is absent. */
typedef char uid_t;
/** @brief Strict-profile placeholder proving gid_t is absent. */
typedef char gid_t;
/** @brief Strict-profile placeholder proving off_t is absent. */
typedef char off_t;

/** @brief Strict-profile placeholder proving struct timespec is absent. */
struct timespec {
    int strict_member; /**< Placeholder member. */
};
/** @brief Strict-profile placeholder proving struct stat is absent. */
struct stat {
    int strict_member; /**< Placeholder member. */
};
/** @brief Strict-profile placeholder proving struct utimbuf is absent. */
struct utimbuf {
    int strict_member; /**< Placeholder member. */
};

/** @brief Strict-profile object proving the stat function is absent. */
static int stat;
/** @brief Strict-profile object proving the utime function is absent. */
static int utime;
