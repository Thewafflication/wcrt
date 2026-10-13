/** @file ms-errors.c @brief Uses selected MS errors with a small owned file. */
#include "vc6-ms-errors.c"
#include <fcntl.h>
#include <limits.h>

int main(int argument_count, char **arguments)
{
    int descriptor;
    wcrt_ms_int64 far_position = _I64_MAX >> 30;
    if (check_vc6_ms_errors()) return 1;
    if (argument_count != 2) return 2;
    descriptor = _open(arguments[1], _O_CREAT | _O_TRUNC | _O_RDWR |
        _O_BINARY, 0600);
    if (descriptor < 0) return 3;
    if (_lseeki64(descriptor, far_position, SEEK_SET) != far_position ||
        _tell(descriptor) != -1 || errno != EFBIG ||
        _telli64(descriptor) != far_position ||
        _filelength(descriptor) != 0 || errno != EFBIG ||
        _close(descriptor) != 0) return 4;
    return 0;
}
