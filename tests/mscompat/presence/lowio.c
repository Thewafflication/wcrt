/** @file lowio.c @brief Checks Microsoft descriptor I/O declarations. */
#include <fcntl.h>
#include <io.h>
#include <fcntl.h>
#include <io.h>

typedef int (*ms_open_fn)(const char *, int, ...);
typedef int (*ms_close_fn)(int);
typedef int (*ms_read_fn)(int, void *, unsigned int);
typedef int (*ms_write_fn)(int, const void *, unsigned int);
typedef long (*ms_lseek_fn)(int, long, int);
typedef long (*ms_tell_fn)(int);
typedef int (*ms_descriptor_fn)(int);
typedef FILE *(*ms_fdopen_fn)(int, const char *);

static ms_open_fn ms_open_address = _open;
static ms_close_fn ms_close_address = _close;
static ms_read_fn ms_read_address = _read;
static ms_write_fn ms_write_address = _write;
static ms_lseek_fn ms_lseek_address = _lseek;
static ms_tell_fn ms_tell_address = _tell;
static ms_descriptor_fn ms_commit_address = _commit;
static ms_descriptor_fn ms_isatty_address = _isatty;
static ms_fdopen_fn ms_fdopen_address = _fdopen;
static ms_descriptor_fn ms_dup_address = _dup;
typedef int (*ms_dup2_fn)(int, int);
static ms_dup2_fn ms_dup2_address = _dup2;
typedef int (*ms_pipe_fn)(int [2], unsigned int, int);
static ms_pipe_fn ms_pipe_address = _pipe;

int main(void)
{
    return ms_open_address == 0 || ms_close_address == 0 ||
        ms_read_address == 0 || ms_write_address == 0 ||
        ms_lseek_address == 0 || ms_tell_address == 0 ||
        ms_commit_address == 0 || ms_isatty_address == 0 ||
        ms_fdopen_address == 0 || ms_dup_address == 0 ||
        ms_dup2_address == 0 || ms_pipe_address == 0;
}
