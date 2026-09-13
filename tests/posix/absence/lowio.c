/** @file lowio.c @brief Checks strict isolation of POSIX descriptor names. */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

static int open;
static int close;
static int read;
static int write;
static int lseek;
static int isatty;
static int fsync;
static int fdopen;
static int dup;
static int dup2;
static int pipe;

int main(void)
{
    return open + close + read + write + lseek + isatty + fsync + fdopen +
        dup + dup2 + pipe;
}
