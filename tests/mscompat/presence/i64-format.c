/**
 * @file i64-format.c
 * @brief Verifies C89 declarations used by Microsoft I64 format callers.
 */

#include <stdio.h>
#include <stdarg.h>

static int (*output_address)(char *, size_t, const char *, ...) = _snprintf;
static int (*voutput_address)(char *, size_t, const char *, va_list) =
    _vsnprintf;
static int (*input_address)(const char *, const char *, ...) = sscanf;

int main(void)
{
    return output_address == 0 || voutput_address == 0 || input_address == 0;
}
