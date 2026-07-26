/**
 * @file startup_gui.c
 * @brief Verifies the optional WCRT ANSI GUI startup contract.
 */

#include <stdlib.h>
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
 * @brief Checks the ANSI GUI startup arguments and termination behavior.
 * @param instance Current executable module instance.
 * @param previous_instance Reserved null previous-instance value.
 * @param command_line Application command line without the program name.
 * @param show_command Initial window-show value.
 * @return Controlled test status.
 */
int WCRT_WINAPI WinMain(void *instance, void *previous_instance,
    char *command_line, int show_command)
{
    if (instance == NULL || previous_instance != NULL || command_line == NULL ||
        show_command < 0) {
        return 111;
    }
    if (strcmp(command_line, "--return") == 0) {
        return 23;
    }
    if (strcmp(command_line, "--atexit") == 0) {
        if (atexit(startup_exit_callback) != 0) {
            return 112;
        }
        return 23;
    }
    if (strcmp(command_line, "--show-hidden") == 0) {
        return show_command == 0 ? 31 : 114;
    }
    if (strcmp(command_line, "empty \"\" tail") == 0) {
        return 0;
    }
    if (strcmp(command_line, "alpha \"two words\" quote\\\"mark") != 0) {
        return 113;
    }
    return 0;
}
