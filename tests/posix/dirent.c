/** @file dirent.c @brief Tests selected POSIX directory streams. */
#define WCRT_POSIX 1
#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/** @brief Finds a named regular entry and validates its Windows mapping. */
static int find_test_file(DIR *directory)
{
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, "test.exe") == 0) {
            if (entry->d_ino != 0 || entry->d_type != DT_REG) return -1;
            if (entry->d_namlen != strlen(entry->d_name) ||
                entry->d_reclen != sizeof(*entry)) return -1;
            return 1;
        }
    }
    return 0;
}

/** @brief Runs selected POSIX directory behavior checks. */
int main(int count, char **arguments)
{
    DIR *directory;
    struct dirent **entries;
    struct dirent *entry;
    char second_name[WCRT_DIRENT_NAME_MAX + 1];
    long position;
    int entry_count;
    int index;
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
    rewinddir(directory);
    if (readdir(directory) == NULL) return 14;
    position = telldir(directory);
    entry = readdir(directory);
    if (position < 1 || entry == NULL) return 15;
    strcpy(second_name, entry->d_name);
    seekdir(directory, position);
    entry = readdir(directory);
    if (entry == NULL || strcmp(entry->d_name, second_name) != 0) return 16;
    errno = 0;
    seekdir(directory, -1);
    if (errno != EINVAL) return 17;
    errno = EDOM;
    while (readdir(directory) != NULL) { }
    if (errno != EDOM) return 6;
    if (closedir(directory) != 0) return 7;
    errno = 0;
    if (readdir(NULL) != NULL || errno != EINVAL) return 8;
    entry_count = scandir(arguments[1], &entries, NULL, alphasort);
    if (entry_count < 1) return 9;
    found = 0;
    for (index = 0; index < entry_count; ++index) {
        if (index != 0 && strcmp(entries[index - 1]->d_name,
            entries[index]->d_name) > 0) return 10;
        if (strcmp(entries[index]->d_name, "test.exe") == 0) found = 1;
    }
    for (index = 0; index < entry_count; ++index) free(entries[index]);
    free(entries);
    if (!found) return 11;
    errno = 0;
    if (scandir("missing-directory", &entries, NULL, alphasort) != -1 ||
        errno != ENOENT) return 12;
    errno = 0;
    if (scandir(arguments[1], NULL, NULL, alphasort) != -1 ||
        errno != EINVAL) return 13;
    return 0;
}
