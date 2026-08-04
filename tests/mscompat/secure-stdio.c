/** @file secure-stdio.c @brief Tests Microsoft secure stdio. */
#include <errno.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Runs secure stdio checks.
 * @param argc Argument count.
 * @param argv Test path in element one.
 * @return Zero on success.
 */
int main(int argc, char **argv)
{
    FILE *stream = (FILE *)1;
    unsigned long long number = 0;
    char trailing = 0;
    char word[4] = { 'X', 'X', 'X', 'X' };
    errno_t error;
    if (argc != 2) return 1;
    error = fopen_s(&stream, argv[1], "w+");
    if (error != 0 || stream == NULL) return 2;
    if (fclose(stream) != 0) return 3;
    if (sscanf_s("42x", "%llu%c", &number, &trailing,
        (unsigned int)sizeof(trailing)) != 2) return 4;
    if (number != 42 || trailing != 'x') return 5;
    if (sscanf_s("abc", "%3s", word,
        (unsigned int)sizeof(word)) != 1) return 6;
    if (strcmp(word, "abc") != 0) return 7;
    stream = (FILE *)1;
    errno = 0;
    if (fopen_s(&stream, "missing-directory/file", "r") == 0) return 8;
    if (stream != NULL || errno == 0) return 9;
    if (remove(argv[1]) != 0) return 10;
    return 0;
}
