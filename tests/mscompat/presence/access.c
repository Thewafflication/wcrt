/** @file access.c @brief Verifies the Microsoft access declaration. */
#include <io.h>

typedef int (*access_fn)(const char *, int);
static access_fn access_address = _access;

int main(void)
{
    return access_address == 0;
}
