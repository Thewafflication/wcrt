/** @file popen.c @brief Verifies command streams and process status. */
#define WCRT_POSIX 1
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int count, char **arguments)
{
    FILE *stream;
    FILE *file;
    char buffer[128];
    char command[512];
    if (count != 2) return 1;
    stream = _popen("echo wcrt-popen", "rb");
    if (stream == NULL || fgets(buffer, sizeof(buffer), stream) == NULL ||
        strstr(buffer, "wcrt-popen") == NULL || _pclose(stream) != 0) return 2;
    sprintf(command, "findstr wcrt > \"%s\"", arguments[1]);
    stream = popen(command, "w");
    if (stream == NULL || fputs("wcrt input\n", stream) == EOF ||
        pclose(stream) != 0) return 3;
    file = fopen(arguments[1], "r");
    if (file == NULL || fgets(buffer, sizeof(buffer), file) == NULL ||
        strstr(buffer, "wcrt input") == NULL || fclose(file) != 0) return 4;
    remove(arguments[1]);
    stream = popen("exit /B 7", "r");
    if (stream == NULL || pclose(stream) != 7) return 5;
    errno = 0;
    if (pclose(NULL) != -1 || errno != EINVAL) return 6;
    return 0;
}
