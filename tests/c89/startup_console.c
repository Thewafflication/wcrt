/**
 * @file startup_console.c
 * @brief Verifies the optional WCRT console startup contract.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

__declspec(dllimport) void WCRT_WINAPI ExitProcess(unsigned int status);

/**
 * @brief Terminates with the callback sentinel status.
 * @return This function does not return.
 */
static void startup_exit_callback(void)
{
    ExitProcess(29);
}

/**
 * @brief Checks arguments, return propagation, and normal termination.
 * @param argument_count Number of command-line arguments.
 * @param argument_values Null-terminated command-line argument vector.
 * @return Controlled test status.
 */
int main(int argument_count, char **argument_values)
{
    char answer[8];
    if (argument_count < 2 || argument_values == NULL ||
        argument_values[0] == NULL || argument_values[argument_count] != NULL) {
        return 101;
    }
    if (strcmp(argument_values[1], "--return") == 0) {
        return argument_count == 2 ? 23 : 102;
    }
    if (strcmp(argument_values[1], "--atexit") == 0) {
        if (argument_count != 2 || atexit(startup_exit_callback) != 0) {
            return 103;
        }
        return 23;
    }
    if (strcmp(argument_values[1], "--stdin") == 0) {
        if (argument_count != 3 || argument_values[2][0] == '\0' ||
            argument_values[2][1] != '\0') {
            return 107;
        }
        if (fgets(answer, sizeof(answer), stdin) == NULL) {
            return 108;
        }
        if (answer[0] != argument_values[2][0] ||
            strcmp(answer + 1, "\n") != 0) {
            return 109;
        }
        if (fgets(answer, sizeof(answer), stdin) != NULL || !feof(stdin) ||
            ferror(stdin)) {
            return 110;
        }
        return 0;
    }
    if (strcmp(argument_values[1], "empty") == 0) {
        if (argument_count != 4 || argument_values[2][0] != '\0' ||
            strcmp(argument_values[3], "tail") != 0) {
            return 105;
        }
        return 0;
    }
    if (strcmp(argument_values[1], "doubled") == 0) {
        if (argument_count != 3 ||
            strcmp(argument_values[2], "double\"quote") != 0) {
            return 106;
        }
        return 0;
    }
    if (argument_count != 4 || strcmp(argument_values[1], "alpha") != 0 ||
        strcmp(argument_values[2], "two words") != 0 ||
        strcmp(argument_values[3], "quote\"mark") != 0) {
        return 104;
    }
    return 0;
}
