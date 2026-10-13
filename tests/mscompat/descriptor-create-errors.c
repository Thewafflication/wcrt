/** @file descriptor-create-errors.c @brief Checks creation forwarding. */
#include <io.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
int fixture_open(const char *path, int flags, int sharing, ...);
#define _sopen fixture_open
#include "../../src/ms_creat.c"

static const char *expected_path;
static int expected_permissions, result_value, error_value, calls, bad_call;

/** @brief Verifies exactly one forwarding call, including the variadic mode. */
int fixture_open(const char *path, int flags, int sharing, ...)
{
    va_list arguments;
    int permissions;
    va_start(arguments, sharing);
    permissions = va_arg(arguments, int);
    va_end(arguments);
    if (path != expected_path || flags !=
        (_O_CREAT | _O_TRUNC | _O_RDWR) ||
        sharing != _SH_DENYNO || permissions != expected_permissions)
        ++bad_call;
    ++calls;
    if (result_value < 0) errno = error_value;
    return result_value;
}
/** @brief Exercises success and unchanged delegated error/argument results. */
int main(void)
{
    expected_path = "owned";
    expected_permissions = _S_IREAD | _S_IWRITE;
    result_value = 17;
    errno = 123;
    if (_creat(expected_path, expected_permissions) != 17 || errno != 123 ||
        calls != 1 || bad_call) return 1;
    result_value = -1;
    error_value = EMFILE;
    if (_creat(expected_path, expected_permissions) != -1 || errno != EMFILE ||
        calls != 2 || bad_call) return 2;
    expected_path = NULL;
    expected_permissions = -1;
    error_value = EINVAL;
    if (_creat(expected_path, expected_permissions) != -1 || errno != EINVAL ||
        calls != 3 || bad_call) return 3;
    return 0;
}
