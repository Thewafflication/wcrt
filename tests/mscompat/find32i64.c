/** @file find32i64.c @brief Verifies mixed records on owned native searches. */
#include <io.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/** @brief Validates one known filename/size without assuming search order. */
static int identify(const char *name, wcrt_ms_int64 size)
{
    if (strstr(name, "-a.bin") && size == 3) return 1;
    if (strstr(name, "-b.bin") && size == 7) return 2;
    return 0;
}
/** @brief Mixes next record forms and checks exhaustion output retention. */
int main(int argc, char **argv)
{
    struct wcrt_finddata32 narrow, saved_narrow;
    struct guarded {
        unsigned long before;
        struct wcrt_finddata32i64 wide;
        unsigned long after;
    } value;
    struct wcrt_finddata32i64 saved_wide;
    FILE *file;
    __wcrt_intptr_t handle;
    int seen, match;
    if (argc != 4) return 1;
    file = fopen(argv[1], "wb");
    if (!file || fwrite("abc", 1, 3, file) != 3 || fclose(file)) return 2;
    file = fopen(argv[2], "wb");
    if (!file || fwrite("abcdefg", 1, 7, file) != 7 || fclose(file)) return 3;
    value.before = 0x12345678UL;
    value.after = 0xabcdef01UL;
    errno = 123;
    handle = wcrt_findfirst32(argv[3], &narrow);
    if (handle == -1) return 4;
    seen = identify(narrow.name, narrow.size);
    if (handle == -1 || errno != 123 || !seen) return 4;
    if (wcrt_findnext32i64(handle, &value.wide) || errno != 123) return 5;
    match = identify(value.wide.name, value.wide.size);
    if (!match || match == seen) return 6;
    saved_narrow = narrow;
    saved_wide = value.wide;
    if (wcrt_findnext32(handle, &narrow) != -1 || errno != ENOENT ||
        memcmp(&narrow, &saved_narrow, sizeof(narrow)) ||
        wcrt_findnext32i64(handle, &value.wide) != -1 || errno != ENOENT ||
        memcmp(&value.wide, &saved_wide, sizeof(saved_wide)) ||
        wcrt_findclose(handle)) return 7;
    errno = 123;
    if (strlen(argv[3]) >= sizeof(value.wide.name)) return 11;
    strcpy(value.wide.name, argv[3]);
    handle = wcrt_findfirst32i64(value.wide.name, &value.wide);
    if (handle == -1) return 8;
    seen = identify(value.wide.name, value.wide.size);
    if (handle == -1 || errno != 123 || !seen ||
        wcrt_findnext32(handle, &narrow)) return 8;
    match = identify(narrow.name, narrow.size);
    if (!match || match == seen || wcrt_findclose(handle)) return 9;
    if (value.before != 0x12345678UL || value.after != 0xabcdef01UL ||
        remove(argv[1]) || remove(argv[2])) return 10;
    return 0;
}
