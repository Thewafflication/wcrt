/** @file fnmatch.c @brief Verifies selected filename matching. */
#define WCRT_POSIX 1
#include <fnmatch.h>

int main(void)
{
    if (fnmatch("*.c", "file.c", 0) != 0) return 1;
    if (fnmatch("*.c", "file.h", 0) != FNM_NOMATCH) return 2;
    if (fnmatch("src/*.c", "src/lib/file.c", FNM_PATHNAME) != FNM_NOMATCH)
        return 3;
    if (fnmatch("src/*.c", "src/file.c", FNM_PATHNAME) != 0) return 4;
    if (fnmatch("[a-c]?.c", "b7.c", 0) != 0) return 5;
    if (fnmatch("[!a-c]*", "zebra", 0) != 0) return 6;
    if (fnmatch("\\*.c", "*.c", 0) != 0) return 7;
    if (fnmatch("FILE.C", "file.c", FNM_CASEFOLD) != 0) return 8;
    if (fnmatch("*", ".hidden", FNM_PERIOD) != FNM_NOMATCH) return 9;
    if (fnmatch(".*", ".hidden", FNM_PERIOD) != 0) return 10;
    if (fnmatch("a/*", "a/.x", FNM_PATHNAME | FNM_PERIOD) != FNM_NOMATCH)
        return 11;
    return 0;
}
