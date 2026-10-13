/** @file vc6-default-mode.c @brief Verifies Microsoft default-mode imports. */
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <share.h>
#include <stdio.h>

typedef char default_mode_width[sizeof(_fmode) == 4 ? 1 : -1];

/** @brief Writes imported storage and observes future DLL open behavior. */
int check_vc6_default_mode(void)
{
    int *storage = &_fmode;
    int original = *storage;
    int descriptor, result = 0;
    char path[L_tmpnam];
    FILE *stream;
    if (original != _O_TEXT ||
        tmpnam(path) != path || remove(path)) return 1;
    *storage = _O_BINARY;
    descriptor = _creat(path, _S_IREAD | _S_IWRITE);
    if (descriptor < 0) result = 2;
    else {
        if (_write(descriptor, "A\n", 2) != 2 ||
            _filelength(descriptor) != 2 ||
            _setmode(descriptor, _O_TEXT) != _O_BINARY) result = 3;
        if (_close(descriptor)) result = 3;
    }
    stream = fopen(path, "w");
    if (!stream) result = 4;
    else {
        if (fputs("B\n", stream) || _filelength(_fileno(stream)) != 2)
            result = 5;
        if (fclose(stream)) result = 5;
    }
    stream = _fsopen(path, "wt", _SH_DENYNO);
    if (!stream) result = 6;
    else {
        if (fputs("C\n", stream) || _filelength(_fileno(stream)) != 3)
            result = 7;
        if (fclose(stream)) result = 7;
    }
    *storage = original;
    if (remove(path)) result = 8;
    return result;
}
