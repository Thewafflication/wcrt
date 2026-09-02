/**
 * @file integer-string.c
 * @brief Verifies Microsoft integer-string conversion declarations.
 */

#include <stdlib.h>

typedef char *(*itoa_fn)(int, char *, int);
typedef char *(*ltoa_fn)(long, char *, int);
typedef char *(*ultoa_fn)(unsigned long, char *, int);
static itoa_fn itoa_address = _itoa;
static ltoa_fn ltoa_address = _ltoa;
static ultoa_fn ultoa_address = _ultoa;

int main(void)
{
    return itoa_address == 0 || ltoa_address == 0 || ultoa_address == 0;
}
