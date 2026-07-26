/**
 * @file startup_gui.c
 * @brief Implements the optional Windows ANSI GUI process startup.
 */

#include <stdlib.h>

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_STARTF_USESHOWWINDOW 0x00000001UL
#define WCRT_SW_SHOWDEFAULT 10

/** @brief Windows process startup fields used by GetStartupInfoA. */
struct wcrt_startup_info {
    unsigned long size;
    char *reserved;
    char *desktop;
    char *title;
    unsigned long x;
    unsigned long y;
    unsigned long x_size;
    unsigned long y_size;
    unsigned long x_count_chars;
    unsigned long y_count_chars;
    unsigned long fill_attribute;
    unsigned long flags;
    unsigned short show_window;
    unsigned short reserved_count;
    unsigned char *reserved_bytes;
    void *standard_input;
    void *standard_output;
    void *standard_error;
};

__declspec(dllimport) char *WCRT_WINAPI GetCommandLineA(void);
__declspec(dllimport) void *WCRT_WINAPI GetModuleHandleA(const char *name);
__declspec(dllimport) void WCRT_WINAPI GetStartupInfoA(
    struct wcrt_startup_info *startup);

/** @brief Application entry point supplied by an ANSI GUI consumer. */
int WCRT_WINAPI WinMain(void *instance, void *previous_instance,
    char *command_line, int show_command);

/**
 * @brief Returns the application-argument portion of a Windows command line.
 * @param command_line Complete command line returned by Windows.
 * @return Pointer following the program name and separating whitespace.
 */
static char *wcrt_gui_arguments(char *command_line);

/**
 * @brief Starts an ANSI GUI application and terminates it normally.
 * @return This function does not return.
 * @note This definition must remain the first function in the startup object.
 */
void _start(void)
{
    struct wcrt_startup_info startup;
    int show_command;
    int status;

    GetStartupInfoA(&startup);
    show_command = (startup.flags & WCRT_STARTF_USESHOWWINDOW) != 0
        ? (int)startup.show_window : WCRT_SW_SHOWDEFAULT;
    status = WinMain(GetModuleHandleA(NULL), NULL,
        wcrt_gui_arguments(GetCommandLineA()), show_command);
    exit(status);
}

/**
 * @brief Provides TinyCC's 32-bit GUI entry-symbol spelling.
 * @post The process terminates and this function does not return.
 */
void _winstart(void)
{
    _start();
}

static char *wcrt_gui_arguments(char *command_line)
{
    char *cursor = command_line;
    int quoted = 0;

    while (*cursor != '\0') {
        if (*cursor == '"') {
            quoted = !quoted;
        } else if (!quoted && (*cursor == ' ' || *cursor == '\t')) {
            break;
        }
        ++cursor;
    }
    while (*cursor == ' ' || *cursor == '\t') {
        ++cursor;
    }
    return cursor;
}
