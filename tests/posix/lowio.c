/** @file lowio.c @brief Verifies shared descriptor I/O behavior. */
#define WCRT_POSIX 1
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int test_microsoft(const char *path)
{
    char buffer[8] = {0};
    int descriptor = _open(path, _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY,
        0600);
    FILE *stream;
    if (descriptor < 3) return 1;
    if (_get_osfhandle(descriptor) == (__wcrt_intptr_t)-1) return 2;
    if (_write(descriptor, "shared", 6) != 6 || _tell(descriptor) != 6)
        return 3;
    if (_lseek(descriptor, 0, SEEK_SET) != 0) return 4;
    if (_read(descriptor, buffer, 6) != 6 || strcmp(buffer, "shared") != 0)
        return 5;
    if (_commit(descriptor) != 0 || _isatty(descriptor) != 0) return 6;
    stream = _fdopen(descriptor, "r+b");
    if (stream == NULL || _fileno(stream) != descriptor) return 7;
    if (fseek(stream, 0, SEEK_END) != 0 || _tell(descriptor) != 6) return 8;
    if (_close(descriptor) != 0) return 9;
    errno = 0;
    if (_read(descriptor, buffer, 1) != -1 || errno != EBADF) return 10;
    errno = 0;
    if (_close(descriptor) != -1 || errno != EBADF) return 11;
    errno = 0;
    if (_open(path, _O_CREAT | _O_EXCL | _O_WRONLY, 0600) != -1 ||
        errno != EEXIST) return 12;
    return 0;
}

static int test_posix(const char *path)
{
    char buffer[8] = {0};
    int descriptor = open(path, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, 0600);
    FILE *stream;
    if (descriptor < 3) return 20;
    if (write(descriptor, "portable", 8) != 8) return 21;
    if (lseek(descriptor, 0, SEEK_SET) != 0) return 22;
    if (read(descriptor, buffer, 8) != 8 ||
        memcmp(buffer, "portable", 8) != 0) return 23;
    if (fsync(descriptor) != 0 || isatty(descriptor) != 0) return 24;
    stream = fdopen(descriptor, "r+b");
    if (stream == NULL || fileno(stream) != descriptor) return 25;
    if (fclose(stream) != 0) return 26;
    errno = 0;
    if (close(descriptor) != -1 || errno != EBADF) return 27;
    return 0;
}

int main(int argument_count, char **arguments)
{
    int result;
    if (argument_count != 3) return 30;
    result = test_microsoft(arguments[1]);
    if (result != 0) return result;
    result = test_posix(arguments[2]);
    remove(arguments[1]);
    remove(arguments[2]);
    return result;
}
