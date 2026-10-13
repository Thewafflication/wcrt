/** @file descriptor-create.c @brief Tests owned descriptor creation. */
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/** @brief Checks permissions, translation, sharing and truncation. */
int main(int argc, char **argv)
{
    int (*create)(const char *, int) = _creat;
    int descriptor, other;
    char bytes[4];
    struct _stat64 status;
    if (argc != 2) return 1;
    errno = 123;
    descriptor = create(argv[1], _S_IREAD | _S_IWRITE);
    if (descriptor < 0 || errno != 123 || _filelength(descriptor) != 0 ||
        _write(descriptor, "A\n", 2) != 2 || _filelength(descriptor) != 3)
        return 2;
    if (_setmode(descriptor, _O_BINARY) != _O_TEXT ||
        _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 3) != 3 || memcmp(bytes, "A\r\n", 3)) return 3;
    other = _sopen(argv[1], _O_RDWR | _O_BINARY, _SH_DENYNO);
    if (other < 0 || _close(other) || _close(descriptor)) return 4;
    descriptor = create(argv[1], _S_IREAD);
    if (descriptor < 0 || _filelength(descriptor) != 0 ||
        _write(descriptor, "B", 1) != 1 || _close(descriptor)) return 5;
    if (_stat64(argv[1], &status) || !(status.st_mode & _S_IWRITE) ||
        status.st_size != 1 || remove(argv[1])) return 6;
    descriptor = create(argv[1], _S_IREAD);
    if (descriptor < 0 || _write(descriptor, "C", 1) != 1 ||
        _close(descriptor) || _stat64(argv[1], &status) ||
        (status.st_mode & _S_IWRITE)) return 7;
    if (create(argv[1], _S_IREAD | _S_IWRITE) != -1 || errno != EACCES ||
        _stat64(argv[1], &status) || status.st_size != 1) return 8;
    descriptor = _open(argv[1], _O_RDONLY | _O_BINARY);
    if (descriptor < 0 || _read(descriptor, bytes, 1) != 1 || bytes[0] != 'C' ||
        _close(descriptor) || _chmod(argv[1], _S_IREAD | _S_IWRITE) ||
        remove(argv[1])) return 9;
    if (create(NULL, _S_IWRITE) != -1 || errno != EINVAL ||
        create("", _S_IWRITE) != -1 || errno != EINVAL ||
        create(argv[1], -1) != -1 || errno != EINVAL ||
        _access(argv[1], 0) != -1 || errno != ENOENT) return 10;
    descriptor = create(argv[1], _S_IWRITE);
    if (descriptor < 0 || _write(descriptor, "D", 1) != 1 ||
        _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 1) != 1 || bytes[0] != 'D' ||
        _close(descriptor) || remove(argv[1])) return 11;
    return 0;
}
