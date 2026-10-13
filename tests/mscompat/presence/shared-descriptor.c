/**
 * @file shared-descriptor.c
 * @brief Verifies the Microsoft shared-descriptor declaration.
 */

#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <sys/stat.h>
#include <io.h>

static int (*open_address)(const char *, int, int, ...) = _sopen;

int main(void)
{
    return open_address == 0;
}
