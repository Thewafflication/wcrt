/** @file unlink.c @brief Verifies the Microsoft unlink declaration. */
#include <io.h>

typedef int (*unlink_fn)(const char *);
static unlink_fn unlink_address = _unlink;

int main(void)
{
    return unlink_address == 0;
}
