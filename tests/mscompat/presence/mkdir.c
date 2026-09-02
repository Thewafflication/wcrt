/** @file mkdir.c @brief Verifies the Microsoft mkdir declaration. */
#include <direct.h>

typedef int (*mkdir_fn)(const char *);
static mkdir_fn mkdir_address = _mkdir;

int main(void)
{
    return mkdir_address == 0;
}
