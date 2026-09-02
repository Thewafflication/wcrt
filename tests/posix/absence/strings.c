/** @file strings.c @brief Checks strict isolation of POSIX strings. */
#include <strings.h>
#include <strings.h>

/** @brief Strict-profile object proving strcasecmp is absent. */
static int strcasecmp;
/** @brief Strict-profile object proving strncasecmp is absent. */
static int strncasecmp;

int main(void)
{
    return strcasecmp + strncasecmp;
}
