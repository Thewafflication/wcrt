/** @file unlink.c @brief Checks strict isolation of POSIX unlink. */
#include <unistd.h>
#include <unistd.h>

/** @brief Strict-profile object proving unlink is absent. */
static int unlink;

int main(void)
{
    return unlink;
}
