/**
 * @file ms_path.c
 * @brief Implements lexical Microsoft narrow path construction and splitting.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/** @brief Copies one component range when its output is requested. */
static void wcrt_path_component(char *output, const char *start, size_t size)
{
    if (output != NULL) {
        memcpy(output, start, size);
        output[size] = '\0';
    }
}

void _makepath(char *path, const char *drive, const char *directory,
    const char *filename, const char *extension)
{
    char *cursor = path;
    const char *start;

    if (path == NULL) {
        errno = EINVAL;
        return;
    }
    if (drive != NULL && *drive != '\0') {
        *cursor++ = *drive;
        *cursor++ = ':';
    }
    if (directory != NULL && *directory != '\0') {
        start = directory;
        while (*directory != '\0') *cursor++ = *directory++;
        if (directory > start && directory[-1] != '/' &&
            directory[-1] != '\\') *cursor++ = '\\';
    }
    if (filename != NULL) {
        while (*filename != '\0') *cursor++ = *filename++;
    }
    if (extension != NULL && *extension != '\0') {
        if (*extension != '.') *cursor++ = '.';
        while (*extension != '\0') *cursor++ = *extension++;
    }
    *cursor = '\0';
}

void _splitpath(const char *path, char *drive, char *directory,
    char *filename, char *extension)
{
    const char *start;
    const char *basename;
    const char *dot = NULL;
    const char *cursor;

    if (path == NULL) {
        if (drive != NULL) *drive = '\0';
        if (directory != NULL) *directory = '\0';
        if (filename != NULL) *filename = '\0';
        if (extension != NULL) *extension = '\0';
        errno = EINVAL;
        return;
    }
    start = path;
    if (path[0] != '\0' && path[1] == ':') start += 2;
    wcrt_path_component(drive, path, (size_t)(start - path));
    basename = start;
    for (cursor = start; *cursor != '\0'; ++cursor) {
        if (*cursor == '/' || *cursor == '\\') {
            basename = cursor + 1;
            dot = NULL;
        } else if (*cursor == '.') {
            dot = cursor;
        }
    }
    wcrt_path_component(directory, start, (size_t)(basename - start));
    if (dot == NULL) dot = cursor;
    wcrt_path_component(filename, basename, (size_t)(dot - basename));
    wcrt_path_component(extension, dot, (size_t)(cursor - dot));
}
