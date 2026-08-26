/** @file dirent.c @brief Tests selected POSIX directory streams. */
#define WCRT_POSIX 1
#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

/** @brief Finds a named regular entry and validates its Windows mapping. */
static int find_test_file(DIR *directory)
{
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, "test.exe") == 0) {
            if (entry->d_ino != 0 || entry->d_type != DT_REG) return -1;
            return 1;
        }
    }
    return 0;
}

/** @brief Runs selected POSIX directory behavior checks. */
int main(int count, char **arguments)
{
    DIR *directory;
    int found;
    if (count != 2) return 1;
    errno = 0;
    if (opendir("missing-directory") != NULL || errno != ENOENT) return 2;
    directory = opendir(arguments[1]);
    if (directory == NULL) return 3;
    found = find_test_file(directory);
    if (found != 1) return 4;
    rewinddir(directory);
    found = find_test_file(directory);
    if (found != 1) return 5;
    errno = EDOM;
    while (readdir(directory) != NULL) { }
    if (errno != EDOM) return 6;
    if (closedir(directory) != 0) return 7;
    errno = 0;
    if (readdir(NULL) != NULL || errno != EINVAL) return 8;
    return 0;
}
