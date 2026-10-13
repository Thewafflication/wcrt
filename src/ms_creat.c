/** @file ms_creat.c @brief Implements legacy descriptor create/truncate. */
#include <io.h>
#include <fcntl.h>
#include <share.h>

/** @brief Reuses controlled shared-open permission and error behavior. */
int _creat(const char *path, int permissions)
{
    return _sopen(path, _O_CREAT | _O_TRUNC | _O_RDWR,
        _SH_DENYNO, permissions);
}
