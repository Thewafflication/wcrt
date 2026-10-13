/**
 * @file drive-query.c
 * @brief Tests read-only drive queries against native providers.
 */

#include <direct.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int main(void)
{
    char before[_MAX_PATH];
    char after[_MAX_PATH];
    unsigned long expected;
    unsigned long actual;
    int drive;
    int letter;
    if (_getcwd(before, _MAX_PATH) != before) return 1;
    expected = GetLogicalDrives();
    errno = EDOM;
    actual = _getdrives();
    if (actual != expected || errno != EDOM || (actual & 0xfc000000UL))
        return 2;
    letter = (unsigned char)before[0];
    if (letter >= 'a' && letter <= 'z') letter -= 'a' - 'A';
    errno = EDOM;
    drive = _getdrive();
    if (before[1] == ':' && letter >= 'A' && letter <= 'Z') {
        if (drive != letter - 'A' + 1) return 3;
    } else if (drive != 0) return 4;
    if (errno != EDOM || _getcwd(after, _MAX_PATH) != after ||
        strcmp(before, after)) return 5;
    return 0;
}
