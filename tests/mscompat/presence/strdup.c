/**
 * @file strdup.c
 * @brief Verifies the Microsoft string duplication declaration.
 */

#include <string.h>

typedef char *(*duplicate_fn)(const char *);
static duplicate_fn duplicate_address = _strdup;

int main(void)
{
    return duplicate_address == 0;
}
