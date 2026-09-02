#include <stddef.h>
#include <string.h>

typedef char *(*set_fn)(char *, int);
typedef char *(*nset_fn)(char *, int, size_t);
static set_fn set_address = _strset;
static nset_fn nset_address = _strnset;

int main(void)
{
    return set_address == 0 || nset_address == 0;
}
