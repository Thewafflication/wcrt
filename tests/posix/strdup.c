/** @file strdup.c @brief Verifies selected POSIX strdup behavior. */
#define WCRT_POSIX 1
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char source[] = "portable";
    char *copy = strdup(source);
    char *empty;

    if (copy == 0 || copy == source || strcmp(copy, source) != 0) return 1;
    source[0] = 'P';
    if (strcmp(copy, "portable") != 0) return 2;
    free(copy);
    empty = strdup("");
    if (empty == 0 || empty[0] != '\0') return 3;
    free(empty);
    return 0;
}
