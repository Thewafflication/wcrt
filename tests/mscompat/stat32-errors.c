/**
 * @file stat32-errors.c
 * @brief Tests atomic narrow conversion using deterministic status providers.
 */

#include <errno.h>
#include <string.h>

#define __wcrt_status_path test_status_path
#define __wcrt_status_handle test_status_handle
#include "../../src/ms_stat32.c"

static struct wcrt_file_status source;
static int provider_error;

/** @brief Supplies controlled metadata or a retained provider failure. */
int test_status_path(const char *path, struct wcrt_file_status *result)
{
    (void)path;
    if (provider_error) {
        errno = provider_error;
        return -1;
    }
    *result = source;
    return 0;
}

/** @brief Supplies controlled metadata for a valid WCRT native handle. */
int test_status_handle(void *handle, struct wcrt_file_status *result)
{
    (void)handle;
    return test_status_path("owned", result);
}

/** @brief Checks error and zeroed output for one provider vector. */
static int check_failure(int expected)
{
    struct wcrt_stat32 status;
    unsigned char empty[sizeof(status)];
    memset(&status, 0xa5, sizeof(status));
    memset(empty, 0, sizeof(empty));
    errno = EDOM;
    return wcrt_stat32("owned", &status) != -1 || errno != expected ||
        memcmp(&status, empty, sizeof(status)) != 0;
}

int main(void)
{
    struct wcrt_stat32 status;
    long long *times[3];
    FILE *stream;
    int index;
    memset(&source, 0, sizeof(source));
    source.device = 0x12345678U;
    source.inode = 0x123456789ULL;
    source.mode = _S_IFREG | _S_IREAD;
    source.links = 2;
    source.size = 2147483647LL;
    source.access_time = -2147483647LL - 1LL;
    source.write_time = 2147483647LL;
    source.creation_time = -1;
    errno = EDOM;
    if (wcrt_stat32("owned", &status) != 0 || errno != EDOM ||
        status.st_dev != source.device || status.st_rdev != source.device ||
        status.st_ino != 0x6789U || status.st_mode != source.mode ||
        status.st_nlink != 2 || status.st_uid != 0 || status.st_gid != 0 ||
        status.st_size != 2147483647L ||
        status.st_atime != -2147483647L - 1L ||
        status.st_mtime != 2147483647L || status.st_ctime != -1) return 1;
    source.size = 2147483648LL;
    if (check_failure(ERANGE)) return 2;
    source.size = -1;
    if (check_failure(ERANGE)) return 3;
    source.size = 0;
    source.access_time = source.write_time = source.creation_time = 0;
    times[0] = &source.access_time;
    times[1] = &source.write_time;
    times[2] = &source.creation_time;
    for (index = 0; index < 3; ++index) {
        *times[index] = 2147483648LL;
        if (check_failure(ERANGE)) return 4;
        *times[index] = -2147483649LL;
        if (check_failure(ERANGE)) return 5;
        *times[index] = 0;
    }
    provider_error = EACCES;
    if (check_failure(EACCES)) return 6;
    if (wcrt_fstat32(-1, &status) != -1 || errno != EBADF) return 7;
    if (wcrt_fstat32(0, NULL) != -1 || errno != EINVAL) return 8;
    provider_error = 0;
    if (wcrt_stat32("owned", &status) != 0 || status.st_size != 0 ||
        status.st_atime != 0) return 9;
    stream = tmpfile();
    if (stream == NULL) return 10;
    if (wcrt_fstat32(_fileno(stream), &status) != 0 || status.st_size != 0)
        return 11;
    provider_error = EACCES;
    if (wcrt_fstat32(_fileno(stream), &status) != -1 || errno != EACCES)
        return 12;
    provider_error = 0;
    source.write_time = 2147483648LL;
    if (wcrt_fstat32(_fileno(stream), &status) != -1 || errno != ERANGE)
        return 13;
    if (fclose(stream) != 0) return 14;
    return 0;
}
