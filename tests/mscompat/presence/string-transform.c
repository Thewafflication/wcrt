/**
 * @file string-transform.c
 * @brief Verifies Microsoft string transformation declarations.
 */

#include <string.h>

typedef char *(*transform_fn)(char *);
static transform_fn lower_address = _strlwr;
static transform_fn upper_address = _strupr;
static transform_fn reverse_address = _strrev;

int main(void)
{
    return lower_address == 0 || upper_address == 0 || reverse_address == 0;
}
