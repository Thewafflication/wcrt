/**
 * @file startup_console.c
 * @brief Implements the optional Windows console process startup.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

__declspec(dllimport) char *WCRT_WINAPI GetCommandLineA(void);

/** @brief Application entry point supplied by a console consumer. */
int main(int argument_count, char **argument_values);

/**
 * @brief Parses one Windows command-line argument in place.
 * @param input Address of the next unparsed input byte.
 * @param output Address at which to write the parsed argument.
 * @return Address of the next unparsed argument.
 */
static char *wcrt_parse_argument(char *input, char **output);

/**
 * @brief Starts a console application and terminates it normally.
 * @return This function does not return.
 * @note This definition must remain the first function in the startup object.
 */
void _start(void)
{
    char *command_line = GetCommandLineA();
    char *storage;
    char *cursor;
    char **arguments;
    size_t length;
    int count = 0;
    int index = 0;

    length = strlen(command_line);
    storage = (char *)malloc(length + 1);
    if (storage == NULL) {
        exit(127);
    }
    memcpy(storage, command_line, length + 1);
    cursor = storage;
    while (*cursor != '\0') {
        while (*cursor == ' ' || *cursor == '\t') {
            ++cursor;
        }
        if (*cursor != '\0') {
            ++count;
            cursor = wcrt_parse_argument(cursor, NULL);
        }
    }
    arguments = (char **)malloc((size_t)(count + 1) * sizeof(char *));
    if (arguments == NULL) {
        free(storage);
        exit(127);
    }
    memcpy(storage, command_line, length + 1);
    cursor = storage;
    while (*cursor != '\0') {
        while (*cursor == ' ' || *cursor == '\t') {
            ++cursor;
        }
        if (*cursor != '\0') {
            cursor = wcrt_parse_argument(cursor, &arguments[index++]);
        }
    }
    arguments[index] = NULL;
    exit(main(count, arguments));
}

static char *wcrt_parse_argument(char *input, char **output)
{
    char *read = input;
    char *write = input;
    int quoted = 0;

    if (output != NULL) {
        *output = write;
    }
    while (*read != '\0' && (quoted || (*read != ' ' && *read != '\t'))) {
        unsigned int slashes = 0;

        while (*read == '\\') {
            ++slashes;
            ++read;
        }
        if (*read == '"') {
            while (slashes >= 2) {
                if (output != NULL) {
                    *write++ = '\\';
                }
                slashes -= 2;
            }
            if (slashes == 0 && quoted && read[1] == '"') {
                if (output != NULL) {
                    *write++ = '"';
                }
                read += 2;
            } else if (slashes == 0) {
                quoted = !quoted;
                ++read;
            } else {
                if (output != NULL) {
                    *write++ = '"';
                }
                ++read;
            }
        } else {
            while (slashes-- != 0) {
                if (output != NULL) {
                    *write++ = '\\';
                }
            }
            if (*read != '\0') {
                if (output != NULL) {
                    *write++ = *read;
                }
                ++read;
            }
        }
    }
    while (*read == ' ' || *read == '\t') {
        ++read;
    }
    if (output != NULL) {
        *write = '\0';
    }
    return read;
}
