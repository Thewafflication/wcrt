/**
 * @file process.c
 * @brief Implements C89 process and environment services on Windows.
 */

#include <string.h>

#include "../../internal/stdlib.h"

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_INFINITE 0xffffffffUL
#define WCRT_ERROR_ENVVAR_NOT_FOUND 203UL

/** @brief Windows process startup fields used by CreateProcessA. */
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

/** @brief Handles and identifiers returned by CreateProcessA. */
struct wcrt_process_info {
    void *process;
    void *thread;
    unsigned long process_id;
    unsigned long thread_id;
};

__declspec(dllimport) void WCRT_WINAPI ExitProcess(unsigned int status);
__declspec(dllimport) unsigned long WCRT_WINAPI GetEnvironmentVariableA(
    const char *name, char *value, unsigned long size);
__declspec(dllimport) unsigned long WCRT_WINAPI GetLastError(void);
__declspec(dllimport) int WCRT_WINAPI CreateProcessA(const char *application,
    char *command_line, void *process_attributes, void *thread_attributes,
    int inherit_handles, unsigned long flags, void *environment,
    const char *directory, struct wcrt_startup_info *startup,
    struct wcrt_process_info *process);
__declspec(dllimport) unsigned long WCRT_WINAPI WaitForSingleObject(
    void *handle, unsigned long milliseconds);
__declspec(dllimport) int WCRT_WINAPI GetExitCodeProcess(void *process,
    unsigned long *exit_code);
__declspec(dllimport) int WCRT_WINAPI CloseHandle(void *handle);

void __wcrt_process_exit(unsigned int status)
{
    ExitProcess(status);
}

char *__wcrt_process_getenv(const char *name)
{
    static char value[32768];
    unsigned long length;

    if (name == NULL || *name == '\0' || strchr(name, '=') != NULL) {
        return NULL;
    }
    length = GetEnvironmentVariableA(name, value, sizeof(value));
    if (length == 0 && GetLastError() == WCRT_ERROR_ENVVAR_NOT_FOUND) {
        return NULL;
    }
    if (length >= sizeof(value)) {
        return NULL;
    }
    return value;
}

int __wcrt_process_system(const char *command)
{
    char command_line[32768];
    const char prefix[] = "cmd.exe /C ";
    struct wcrt_startup_info startup;
    struct wcrt_process_info process;
    unsigned long exit_code = (unsigned long)-1;
    size_t command_length;

    if (command == NULL) {
        return 1;
    }
    command_length = strlen(command);
    if (command_length + sizeof(prefix) > sizeof(command_line)) {
        return -1;
    }
    strcpy(command_line, prefix);
    strcat(command_line, command);
    memset(&startup, 0, sizeof(startup));
    memset(&process, 0, sizeof(process));
    startup.size = sizeof(startup);
    if (!CreateProcessA(NULL, command_line, NULL, NULL, 0, 0, NULL, NULL,
        &startup, &process)) {
        return -1;
    }
    WaitForSingleObject(process.process, WCRT_INFINITE);
    GetExitCodeProcess(process.process, &exit_code);
    CloseHandle(process.thread);
    CloseHandle(process.process);
    return (int)exit_code;
}
