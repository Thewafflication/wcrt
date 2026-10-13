/**
 * @file shared-stream.c
 * @brief Verifies Microsoft shared-stream declarations and constant values.
 */

#include <share.h>
#include <stdio.h>
#include <share.h>

static FILE *(*open_address)(const char *, const char *, int) = _fsopen;
typedef char share_values[_SH_DENYRW == 0x10 && _SH_DENYWR == 0x20 &&
    _SH_DENYRD == 0x30 && _SH_DENYNO == 0x40 ? 1 : -1];

int main(void)
{
    return open_address == 0;
}
