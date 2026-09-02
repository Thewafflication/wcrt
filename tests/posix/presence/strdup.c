/** @file strdup.c @brief Checks the selected POSIX strdup declaration. */
#define WCRT_POSIX 1
#include <string.h>
#include <string.h>

#ifdef _POSIX_VERSION
#error A bounded WCRT profile must not advertise _POSIX_VERSION
#endif

/** @brief Selected string-duplication signature. */
typedef char *(*posix_strdup_fn)(const char *);
/** @brief Forces type checking of strdup. */
static posix_strdup_fn posix_strdup_address = strdup;

int main(void)
{
    return posix_strdup_address == 0;
}
