/**
 * @file drive-query.c
 * @brief Verifies the Microsoft drive-query declarations.
 */

#include <direct.h>
#include <direct.h>

static int (*drive_address)(void) = _getdrive;
static unsigned long (*mask_address)(void) = _getdrives;

int main(void)
{
    return drive_address == 0 || mask_address == 0;
}
