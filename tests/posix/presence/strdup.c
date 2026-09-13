/** @file strdup.c @brief Checks the selected POSIX strdup declaration. */
#define WCRT_POSIX 1
#include <string.h>
#include <string.h>

#ifdef _POSIX_VERSION
#error A bounded WCRT profile must not advertise _POSIX_VERSION
#endif

/** @brief Selected string-duplication signature. */
typedef char *(*posix_strdup_fn)(const char *);
/** @brief Selected bounded string-duplication signature. */
typedef char *(*posix_strndup_fn)(const char *, size_t);
/** @brief Forces type checking of strdup. */
static posix_strdup_fn posix_strdup_address = strdup;
/** @brief Forces type checking of strndup. */
static posix_strndup_fn posix_strndup_address = strndup;

int main(void)
{
    return posix_strdup_address == 0 || posix_strndup_address == 0;
}
