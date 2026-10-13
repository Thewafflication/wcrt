/**
 * @file path-components.c
 * @brief Tests lexical Microsoft path components without filesystem access.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/** @brief Input and expected components for one split operation. */
struct path_case {
    const char *path;
    const char *drive;
    const char *directory;
    const char *filename;
    const char *extension;
};

/** @brief Verifies component content and output guard bytes. */
static int check_split(const struct path_case *test)
{
    char drive[_MAX_DRIVE + 1];
    char directory[_MAX_DIR + 1];
    char filename[_MAX_FNAME + 1];
    char extension[_MAX_EXT + 1];

    memset(drive, 0x5a, sizeof(drive));
    memset(directory, 0x5a, sizeof(directory));
    memset(filename, 0x5a, sizeof(filename));
    memset(extension, 0x5a, sizeof(extension));
    errno = EDOM;
    _splitpath(test->path, drive, directory, filename, extension);
    if (errno != EDOM || strcmp(drive, test->drive) != 0 ||
        strcmp(directory, test->directory) != 0 ||
        strcmp(filename, test->filename) != 0 ||
        strcmp(extension, test->extension) != 0) return 1;
    return drive[_MAX_DRIVE] != 0x5a || directory[_MAX_DIR] != 0x5a ||
        filename[_MAX_FNAME] != 0x5a || extension[_MAX_EXT] != 0x5a;
}

int main(void)
{
    static const struct path_case cases[] = {
        {"", "", "", "", ""},
        {"C:", "C:", "", "", ""},
        {"C:relative.txt", "C:", "", "relative", ".txt"},
        {"c:\\dir\\file.tar.gz", "c:", "\\dir\\", "file.tar", ".gz"},
        {"/dir.with.dot/file", "", "/dir.with.dot/", "file", ""},
        {"a/b\\c.txt", "", "a/b\\", "c", ".txt"},
        {"folder/", "", "folder/", "", ""},
        {".profile", "", "", "", ".profile"},
        {"file.", "", "", "file", "."},
        {"..", "", "", ".", "."},
        {"\\\\server\\share\\a.txt", "", "\\\\server\\share\\", "a", ".txt"}
    };
    char path[_MAX_PATH + 1];
    char component[_MAX_FNAME];
    char output[_MAX_FNAME + 1];
    size_t index;

    for (index = 0; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        if (check_split(&cases[index])) return 1 + (int)index;
    }
    memset(path, 0x5a, sizeof(path));
    errno = EDOM;
    _makepath(path, "c:", "\\sample\\crt", "makepath", "c");
    if (strcmp(path, "c:\\sample\\crt\\makepath.c") != 0 ||
        errno != EDOM || path[_MAX_PATH] != 0x5a) return 20;
    _makepath(path, "Dignored", "a/b/", "file", ".txt");
    if (strcmp(path, "D:a/b/file.txt") != 0) return 21;
    _makepath(path, NULL, NULL, NULL, NULL);
    if (*path != '\0') return 22;
    _makepath(path, "", "", "name", "");
    if (strcmp(path, "name") != 0) return 23;
    _makepath(path, NULL, "dir\\", NULL, "ext");
    if (strcmp(path, "dir\\.ext") != 0) return 24;
    _splitpath("x:/dir/name.ext", NULL, NULL, output, NULL);
    if (strcmp(output, "name") != 0) return 25;
    _splitpath("x:/dir/name.ext", NULL, NULL, NULL, NULL);
    memset(component, 'a', sizeof(component) - 1);
    component[sizeof(component) - 1] = '\0';
    memset(output, 0x5a, sizeof(output));
    _splitpath(component, NULL, NULL, output, NULL);
    if (strcmp(component, output) != 0 || output[_MAX_FNAME] != 0x5a)
        return 26;
    _makepath(path, NULL, NULL, component, NULL);
    if (strcmp(path, component) != 0) return 27;
    errno = 0;
    _makepath(NULL, NULL, NULL, NULL, NULL);
    if (errno != EINVAL) return 28;
    errno = 0;
    _splitpath(NULL, path, output, component, NULL);
    if (errno != EINVAL || *path || *output || *component) return 29;
    return 0;
}
