/** @file stat.c @brief Tests Microsoft 64-bit file status. */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @brief Runs 64-bit file-status checks.
 * @param argc Argument count.
 * @param argv Test path in element one.
 * @return Zero on success.
 */
int main(int argc, char **argv)
{
    struct _stat64 status;
    struct _stat64 descriptor_status;
    FILE *stream;
    if (argc != 2) return 1;
    stream = fopen(argv[1], "wb");
    if (stream == NULL) return 2;
    if (fwrite("stat", 1, 4, stream) != 4) return 3;
    if (_fstat64(_fileno(stream), &descriptor_status) != 0 ||
        descriptor_status.st_size != 4) return 12;
    memset(&status, 0xA5, sizeof(status));
    if (_stat64(argv[1], &status) != 0) return 4;
    if (status.st_size != 4) return 5;
    if ((status.st_mode & _S_IFREG) == 0) return 6;
    if ((status.st_mode & _S_IREAD) == 0) return 7;
    if (status.st_dev != descriptor_status.st_dev ||
        status.st_ino != descriptor_status.st_ino) return 13;
    if (fclose(stream) != 0) return 14;
    if (_chmod(argv[1], _S_IREAD) != 0 ||
        _stat64(argv[1], &status) != 0 ||
        (status.st_mode & _S_IWRITE) != 0) return 15;
    if (_chmod(argv[1], _S_IREAD | _S_IWRITE) != 0 ||
        _stat64(argv[1], &status) != 0 ||
        (status.st_mode & _S_IWRITE) == 0) return 16;
#if defined(__i386__) || defined(_M_IX86)
    memset(&status, 0, sizeof(status));
    if (_stat(argv[1], &status) != 0 || status.st_size != 4) return 8;
#endif
    errno = 0;
    if (_stat64("missing-directory/file", &status) != -1) return 9;
    if (errno == 0) return 10;
    if (remove(argv[1]) != 0) return 11;
    return 0;
}
