/** @file strings.c @brief Checks selected POSIX string declarations. */
#define WCRT_POSIX 1
#include <stddef.h>
#include <strings.h>
#include <strings.h>

#ifndef WCRT_STRINGS_H
#error <strings.h> did not resolve to the WCRT header
#endif
#ifdef _POSIX_VERSION
#error A bounded WCRT profile must not advertise _POSIX_VERSION
#endif

/** @brief Selected unbounded case-comparison signature. */
typedef int (*posix_strcasecmp_fn)(const char *, const char *);
/** @brief Selected bounded case-comparison signature. */
typedef int (*posix_strncasecmp_fn)(const char *, const char *, size_t);
/** @brief Forces type checking of strcasecmp. */
static posix_strcasecmp_fn posix_strcasecmp_address = strcasecmp;
/** @brief Forces type checking of strncasecmp. */
static posix_strncasecmp_fn posix_strncasecmp_address = strncasecmp;

int main(void)
{
    return posix_strcasecmp_address == 0 ||
        posix_strncasecmp_address == 0;
}
