/** @file creation-mask.c @brief Tests owned files under process mask modes. */
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

/** @brief Maps the four partitions to Windows read/write permission bits. */
static int mode_bits(int index)
{
    return ((index & 1) ? _S_IREAD : 0) | ((index & 2) ? _S_IWRITE : 0);
}
/** @brief Verifies previous-mask values and all new-file permission pairs. */
int main(int argc, char **argv)
{
    int (*change)(int) = _umask;
    int original, previous = 0, mask_index, mode_index, mask, mode, descriptor;
    char byte;
    struct _stat64 status;
    if (argc != 2) return 1;
    errno = 123;
    original = change(0);
    if (original || errno != 123 || change(_S_IREAD) != 0 ||
        change(_S_IREAD | 0x40000000) != _S_IREAD ||
        change(-1) != _S_IREAD ||
        change(INT_MIN) != (_S_IREAD | _S_IWRITE) || errno != 123) return 2;
    for (mask_index = 0; mask_index < 4; ++mask_index) {
        mask = mode_bits(mask_index);
        errno = 123;
        if (change(mask) != previous || errno != 123) return 3;
        previous = mask;
        for (mode_index = 0; mode_index < 4; ++mode_index) {
            mode = mode_bits(mode_index);
            descriptor = _sopen(argv[1], _O_CREAT | _O_EXCL | _O_RDWR |
                _O_BINARY, _SH_DENYNO, mode);
            if (descriptor < 0 || _write(descriptor, "Z", 1) != 1 ||
                _lseek(descriptor, 0, SEEK_SET) != 0 ||
                _read(descriptor, &byte, 1) != 1 || byte != 'Z' ||
                _close(descriptor)) return 4;
            if (_stat64(argv[1], &status) || !(status.st_mode & _S_IREAD) ||
                !!(status.st_mode & _S_IWRITE) != !!(mode & ~mask & _S_IWRITE))
                return 5;
            descriptor = _open(argv[1], _O_RDONLY | _O_BINARY);
            if (descriptor < 0 || _read(descriptor, &byte, 1) != 1 ||
                byte != 'Z' || _close(descriptor) ||
                _chmod(argv[1], _S_IREAD | _S_IWRITE) || remove(argv[1]))
                return 6;
        }
    }
    change(0);
    descriptor = _creat(argv[1], _S_IREAD | _S_IWRITE);
    if (descriptor < 0 || _write(descriptor, "Q", 1) != 1 ||
        _close(descriptor)) return 7;
    change(_S_IWRITE);
    descriptor = _creat(argv[1], _S_IREAD | _S_IWRITE);
    if (descriptor < 0 || _filelength(descriptor) ||
        _write(descriptor, "R", 1) != 1 || _close(descriptor) ||
        _stat64(argv[1], &status) || !(status.st_mode & _S_IWRITE) ||
        remove(argv[1])) return 8;
    descriptor = _creat(argv[1], _S_IREAD | _S_IWRITE);
    if (descriptor < 0 || _write(descriptor, "S", 1) != 1 ||
        _close(descriptor) || _stat64(argv[1], &status) ||
        (status.st_mode & _S_IWRITE)) return 9;
    change(0);
    if (_creat(argv[1], _S_IREAD | _S_IWRITE) != -1 || errno != EACCES ||
        _stat64(argv[1], &status) || status.st_size != 1 ||
        _chmod(argv[1], _S_IREAD | _S_IWRITE) || remove(argv[1])) return 10;
    change(_S_IWRITE);
    descriptor = _open(argv[1], _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY);
    if (descriptor < 0 || _write(descriptor, "T", 1) != 1 ||
        _close(descriptor) || _stat64(argv[1], &status) ||
        (status.st_mode & _S_IWRITE) ||
        _chmod(argv[1], _S_IREAD | _S_IWRITE) || remove(argv[1])) return 11;
    errno = 123;
    if (change(original) != _S_IWRITE || errno != 123) return 12;
    return 0;
}
