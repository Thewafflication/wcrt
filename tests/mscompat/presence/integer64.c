/**
 * @file integer64.c
 * @brief Verifies Microsoft 64-bit declarations and limit expression types.
 */

#include <stdlib.h>
#include <limits.h>

static char *(*signed_writer)(long long, char *, int) = _i64toa;
static char *(*unsigned_writer)(unsigned long long, char *, int) = _ui64toa;
static long long (*decimal_reader)(const char *) = _atoi64;
static long long (*signed_reader)(const char *, char **, int) = _strtoi64;
static unsigned long long (*unsigned_reader)(const char *, char **, int) =
    _strtoui64;
typedef char signed_width[sizeof(wcrt_ms_int64) == 8 ? 1 : -1];
typedef char unsigned_width[sizeof(wcrt_ms_uint64) == 8 ? 1 : -1];
typedef char signed_limit[_I64_MIN == (-9223372036854775807LL - 1LL) &&
    _I64_MAX == 9223372036854775807LL ? 1 : -1];
typedef char unsigned_limit[_UI64_MAX == 18446744073709551615ULL ? 1 : -1];

int main(void)
{
    return signed_writer == 0 || unsigned_writer == 0 || decimal_reader == 0 ||
        signed_reader == 0 || unsigned_reader == 0;
}
