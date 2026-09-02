/** @file strdup.c @brief Checks strict isolation of POSIX strdup. */
#include <string.h>
#include <string.h>

/** @brief Strict-profile object proving strdup is absent. */
static int strdup;

int main(void)
{
    return strdup;
}
