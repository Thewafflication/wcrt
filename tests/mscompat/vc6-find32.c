/** @file vc6-find32.c @brief Checks the selected VC6 search source ABI. */
#define WCRT_VC6_FIND32 1
#define WCRT_VC6_TIME32 1
#define WCRT_VC6_STAT32 1
#define WCRT_VC6_UTIME32 1
#include <time.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <io.h>
#include <io.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
typedef char narrow_record[sizeof(struct _finddata_t) == 280 ? 1 : -1];
typedef char narrow_name[offsetof(struct _finddata_t, name) == 20 ? 1 : -1];
typedef char wide_record[sizeof(struct _finddatai64_t) == 288 ? 1 : -1];
typedef char wide_name[offsetof(struct _finddatai64_t, name) == 24 ? 1 : -1];
typedef char size_alias[sizeof(_fsize_t) == 4 ? 1 : -1];
/** @brief Exercises all source function pointers without host CRT calls. */
int check_vc6_find32(void)
{
    __wcrt_intptr_t (*first)(const char *, struct _finddata_t *) = _findfirst;
    int (*next)(__wcrt_intptr_t, struct _finddata_t *) = _findnext;
    int (*close)(__wcrt_intptr_t) = _findclose;
    __wcrt_intptr_t (*first_wide)(const char *, struct _finddatai64_t *) =
        _findfirsti64;
    int (*next_wide)(__wcrt_intptr_t, struct _finddatai64_t *) = _findnexti64;
    struct _finddata_t value;
    struct _finddata_t saved;
    struct _finddatai64_t wide, saved_wide;
    char name[L_tmpnam];
    FILE *file;
    __wcrt_intptr_t handle;
    if (first(NULL, &value) != -1 || errno != EINVAL || value.name[0])
        return 1;
    if (next(-1, &value) != -1 || errno != EINVAL) return 2;
    if (close(-1) != -1 || errno != EINVAL) return 3;
    if (!tmpnam(name)) return 4;
    file = fopen(name, "wb");
    if (!file) return 5;
    if (fwrite("abc", 1, 3, file) != 3 || fclose(file)) return 6;
    errno = 123;
    handle = first(name, &value);
    if (handle == -1 || errno != 123 || value.size != 3 || !value.name[0])
        return 7;
    saved = value;
    if (next(handle, &value) != -1 || errno != ENOENT ||
        memcmp(&value, &saved, sizeof(saved)))
        return 8;
    if (close(handle)) return 9;
    handle = first_wide(name, &wide);
    if (handle == -1 || wide.size != 3 || strcmp(wide.name, saved.name))
        return 10;
    saved_wide = wide;
    if (next_wide(handle, &wide) != -1 || errno != ENOENT ||
        memcmp(&wide, &saved_wide, sizeof(wide)) || close(handle) ||
        remove(name)) return 11;
    return 0;
}
