/** @file dirent.c @brief Checks strict isolation of POSIX directories. */
#include <dirent.h>
#include <dirent.h>

#ifdef DT_UNKNOWN
#error Directory entry types leaked into the strict WCRT profile
#endif

/** @brief Strict-profile placeholder proving DIR is absent. */
typedef char DIR;
/** @brief Strict-profile placeholder proving struct dirent is absent. */
struct dirent {
    int strict_member; /**< Placeholder member. */
};

/** @brief Strict-profile object proving opendir is absent. */
static int opendir;
/** @brief Strict-profile object proving readdir is absent. */
static int readdir;
/** @brief Strict-profile object proving rewinddir is absent. */
static int rewinddir;
/** @brief Strict-profile object proving telldir is absent. */
static int telldir;
/** @brief Strict-profile object proving seekdir is absent. */
static int seekdir;
/** @brief Strict-profile object proving closedir is absent. */
static int closedir;
/** @brief Strict-profile object proving scandir is absent. */
static int scandir;
/** @brief Strict-profile object proving alphasort is absent. */
static int alphasort;
