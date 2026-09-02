#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char source[] = "duplicate";
    char *copy = _strdup(source);
    char *empty;
    if (copy == 0 || copy == source || strcmp(copy, source) != 0) return 1;
    source[0] = 'D';
    if (strcmp(copy, "duplicate") != 0) return 2;
    free(copy);
    empty = _strdup("");
    if (empty == 0 || empty[0] != '\0') return 3;
    free(empty);
    errno = 0;
    if (_strdup(0) != 0 || errno != EINVAL) return 4;
    return 0;
}
