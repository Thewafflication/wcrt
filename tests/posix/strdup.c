/** @file strdup.c @brief Verifies selected POSIX strdup behavior. */
#define WCRT_POSIX 1
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char source[] = "portable";
    char *copy = strdup(source);
    char *empty;
    char *bounded;

    if (copy == 0 || copy == source || strcmp(copy, source) != 0) return 1;
    source[0] = 'P';
    if (strcmp(copy, "portable") != 0) return 2;
    free(copy);
    empty = strdup("");
    if (empty == 0 || empty[0] != '\0') return 3;
    free(empty);
    bounded = strndup("bounded", 4);
    if (bounded == 0 || strcmp(bounded, "boun") != 0) return 4;
    free(bounded);
    bounded = strndup("short", 30);
    if (bounded == 0 || strcmp(bounded, "short") != 0) return 5;
    free(bounded);
    bounded = strndup("ignored", 0);
    if (bounded == 0 || bounded[0] != '\0') return 6;
    free(bounded);
    return 0;
}
