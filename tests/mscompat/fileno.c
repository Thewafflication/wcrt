/**
 * @file fileno.c
 * @brief Exercises Microsoft stream-to-file-descriptor compatibility.
 */

#include <errno.h>
#include <stdio.h>

/** @brief Runs Microsoft _fileno compatibility checks. */
int main(int argument_count, char **arguments)
{
    FILE *stream;
    int descriptor;
    long position;

    if (argument_count != 2) return 1;
    if (_fileno(stdin) != 0) return 2;
    if (_fileno(stdout) != 1) return 3;
    if (_fileno(stderr) != 2) return 4;

    stream = fopen(arguments[1], "w+");
    if (stream == NULL) return 5;
    descriptor = _fileno(stream);
    if (descriptor < 3 || _fileno(stream) != descriptor) return 6;
    if (fputs("descriptor", stream) == EOF) return 7;
    position = ftell(stream);
    if (position < 0 || _fileno(stream) != descriptor) return 8;
    if (ftell(stream) != position) return 9;
    if (fseek(stream, 0, SEEK_END) != 0) return 10;
    if (fgetc(stream) != EOF || !feof(stream)) return 11;
    if (_fileno(stream) != descriptor || !feof(stream)) return 12;
    stream = freopen(arguments[1], "r", stream);
    if (stream == NULL || _fileno(stream) != descriptor) return 13;
    if (feof(stream) || ftell(stream) != 0) return 14;
    if (fclose(stream) != 0) return 15;

    errno = 0;
    if (_fileno(NULL) != -1 || errno != EINVAL) return 16;
    if (remove(arguments[1]) != 0) return 17;
    return 0;
}
