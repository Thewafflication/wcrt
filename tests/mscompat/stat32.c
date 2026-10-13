/**
 * @file stat32.c
 * @brief Compares narrow status with the existing 64-bit file status.
 */

#include <errno.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/** @brief Places canaries around the narrow metadata record. */
struct guarded_status {
    unsigned char before[8]; /**< Leading canary. */
    struct wcrt_stat32 status; /**< Narrow metadata record. */
    unsigned char after[64]; /**< Tail catches a wider-ABI overwrite. */
};

/** @brief Checks that a failing query completely zeroed its output. */
static int empty_status(const struct wcrt_stat32 *status)
{
    unsigned char empty[sizeof(*status)];
    memset(empty, 0, sizeof(empty));
    return memcmp(status, empty, sizeof(empty)) == 0;
}

int main(int argument_count, char **arguments)
{
    struct guarded_status guarded;
    struct wcrt_stat32 descriptor_status;
    struct _stat64 wide;
    FILE *stream;
    int descriptor;
    int index;
    if (argument_count != 2) return 1;
    stream = fopen(arguments[1], "w+b");
    if (stream == NULL || fwrite("status", 1, 6, stream) != 6 ||
        fflush(stream) != 0) return 2;
    descriptor = _fileno(stream);
    memset(&guarded, 0xa5, sizeof(guarded));
    errno = EDOM;
    if (wcrt_stat32(arguments[1], &guarded.status) != 0 || errno != EDOM ||
        wcrt_fstat32(descriptor, &descriptor_status) != 0 ||
        _stat64(arguments[1], &wide) != 0) return 3;
    if (guarded.status.st_size != 6 || descriptor_status.st_size != 6 ||
        guarded.status.st_dev != wide.st_dev ||
        guarded.status.st_ino != wide.st_ino ||
        guarded.status.st_mode != wide.st_mode ||
        guarded.status.st_atime != wide.st_atime ||
        guarded.status.st_mtime != wide.st_mtime ||
        guarded.status.st_ctime != wide.st_ctime ||
        guarded.status.st_rdev != wide.st_rdev ||
        guarded.status.st_nlink != wide.st_nlink ||
        guarded.status.st_uid != 0 || guarded.status.st_gid != 0 ||
        descriptor_status.st_dev != guarded.status.st_dev ||
        descriptor_status.st_ino != guarded.status.st_ino) return 4;
    for (index = 0; index < 8; ++index) {
        if (guarded.before[index] != 0xa5) return 5;
    }
    for (index = 0; index < 64; ++index)
        if (guarded.after[index] != 0xa5) return 5;
    errno = 0;
    if (wcrt_stat32(NULL, &guarded.status) != -1 || errno != EINVAL ||
        !empty_status(&guarded.status)) return 6;
    errno = 0;
    if (wcrt_stat32(arguments[1], NULL) != -1 || errno != EINVAL) return 7;
    if (fclose(stream) != 0 || remove(arguments[1]) != 0) return 8;
    errno = 0;
    if (wcrt_fstat32(descriptor, &descriptor_status) != -1 || errno != EBADF ||
        !empty_status(&descriptor_status)) return 9;
    memset(&guarded.status, 0xa5, sizeof(guarded.status));
    errno = 0;
    if (wcrt_stat32(arguments[1], &guarded.status) != -1 || errno != ENOENT ||
        !empty_status(&guarded.status)) return 10;
    return 0;
}
