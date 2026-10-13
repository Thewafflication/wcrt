/**
 * @file descriptor-mode.c
 * @brief Verifies the Microsoft descriptor translation declaration.
 */

#include <fcntl.h>
#include <io.h>
#include <io.h>

static int (*mode_address)(int, int) = _setmode;

int main(void)
{
    return mode_address == 0;
}
