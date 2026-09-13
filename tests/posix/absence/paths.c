/** @file paths.c @brief Checks strict isolation of POSIX path names. */
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>

static int access;
static int mkdir;

int main(void)
{
    return access + mkdir;
}
