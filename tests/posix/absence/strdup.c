/** @file strdup.c @brief Checks strict isolation of POSIX strdup. */
#include <string.h>
#include <string.h>

/** @brief Strict-profile object proving strdup is absent. */
static int strdup;
/** @brief Strict-profile object proving strndup is absent. */
static int strndup;

int main(void)
{
    return strdup + strndup;
}
