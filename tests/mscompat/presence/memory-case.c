/**
 * @file memory-case.c
 * @brief Verifies the Microsoft case-insensitive memory declaration.
 */

#include <stddef.h>
#include <string.h>

typedef int (*memicmp_fn)(const void *, const void *, size_t);
static memicmp_fn memicmp_address = _memicmp;

int main(void)
{
    return memicmp_address == 0;
}
