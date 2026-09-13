/** @file lowio.c @brief Checks selected POSIX descriptor declarations. */
#define WCRT_POSIX 1
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

typedef int (*posix_open_fn)(const char *, int, ...);
typedef int (*posix_close_fn)(int);
typedef ssize_t (*posix_read_fn)(int, void *, size_t);
typedef ssize_t (*posix_write_fn)(int, const void *, size_t);
typedef off_t (*posix_lseek_fn)(int, off_t, int);
typedef int (*posix_descriptor_fn)(int);
typedef FILE *(*posix_fdopen_fn)(int, const char *);

static posix_open_fn posix_open_address = open;
static posix_close_fn posix_close_address = close;
static posix_read_fn posix_read_address = read;
static posix_write_fn posix_write_address = write;
static posix_lseek_fn posix_lseek_address = lseek;
static posix_descriptor_fn posix_isatty_address = isatty;
static posix_descriptor_fn posix_fsync_address = fsync;
static posix_fdopen_fn posix_fdopen_address = fdopen;
static posix_descriptor_fn posix_dup_address = dup;
typedef int (*posix_dup2_fn)(int, int);
static posix_dup2_fn posix_dup2_address = dup2;

int main(void)
{
    return posix_open_address == 0 || posix_close_address == 0 ||
        posix_read_address == 0 || posix_write_address == 0 ||
        posix_lseek_address == 0 || posix_isatty_address == 0 ||
        posix_fsync_address == 0 || posix_fdopen_address == 0 ||
        posix_dup_address == 0 || posix_dup2_address == 0;
}
