/**
 * @file path-components.c
 * @brief Verifies Microsoft path declarations and buffer constants.
 */

#include <stdlib.h>
#include <stdlib.h>

typedef void (*make_fn)(char *, const char *, const char *, const char *,
    const char *);
typedef void (*split_fn)(const char *, char *, char *, char *, char *);
static make_fn make_address = _makepath;
static split_fn split_address = _splitpath;
typedef char path_sizes[(_MAX_PATH == 260 && _MAX_DRIVE == 3 &&
    _MAX_DIR == 256 && _MAX_FNAME == 256 && _MAX_EXT == 256) ? 1 : -1];

int main(void)
{
    return make_address == 0 || split_address == 0;
}
