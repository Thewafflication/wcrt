/**
 * @file process.c
 * @brief Implements C89 process and environment services on Windows.
 */

#define WCRT_POSIX 1

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../../internal/file.h"
#include "../../internal/stdlib.h"

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_INFINITE 0xffffffffUL
#define WCRT_ERROR_ENVVAR_NOT_FOUND 203UL
#define WCRT_STARTF_USESTDHANDLES 0x100UL
#define WCRT_HANDLE_FLAG_INHERIT 1UL

/** @brief Inheritable-handle security descriptor. */
struct wcrt_security_attributes {
    unsigned long size;
    void *security_descriptor;
    int inherit;
};

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
__declspec(dllimport) unsigned long WCRT_WINAPI GetCurrentProcessId(void);
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
__declspec(dllimport) void *WCRT_WINAPI GetStdHandle(unsigned long selector);
__declspec(dllimport) int WCRT_WINAPI CreatePipe(void **reader, void **writer,
    struct wcrt_security_attributes *attributes, unsigned long size);
__declspec(dllimport) int WCRT_WINAPI SetHandleInformation(void *handle,
    unsigned long mask, unsigned long flags);

/** @brief Active process associated with one popen stream. */
struct wcrt_popen_entry {
    FILE *stream;
    void *process;
};

static struct wcrt_popen_entry wcrt_popen_entries[FOPEN_MAX];

void __wcrt_process_exit(unsigned int status)
{
    ExitProcess(status);
}

long long getpid(void)
{
    return (long long)GetCurrentProcessId();
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

FILE *__wcrt_process_popen(const char *command, const char *mode)
{
    char command_line[32768];
    const char prefix[] = "cmd.exe /C ";
    struct wcrt_security_attributes security;
    struct wcrt_startup_info startup;
    struct wcrt_process_info process;
    void *reader;
    void *writer;
    void *parent;
    void *child;
    FILE *stream;
    int reading;
    int slot;
    size_t length;
    if (command == NULL || mode == NULL ||
        ((*mode != 'r' && *mode != 'w')) ||
        (mode[1] != '\0' && !((mode[1] == 'b' || mode[1] == 't') &&
        mode[2] == '\0'))) {
        errno = EINVAL;
        return NULL;
    }
    for (slot = 0; slot < FOPEN_MAX; ++slot)
        if (wcrt_popen_entries[slot].stream == NULL) break;
    if (slot == FOPEN_MAX) {
        errno = EMFILE;
        return NULL;
    }
    length = strlen(command);
    if (length + sizeof(prefix) > sizeof(command_line)) {
        errno = EINVAL;
        return NULL;
    }
    security.size = sizeof(security);
    security.security_descriptor = NULL;
    security.inherit = 1;
    if (!CreatePipe(&reader, &writer, &security, 0)) {
        errno = EIO;
        return NULL;
    }
    reading = *mode == 'r';
    parent = reading ? reader : writer;
    child = reading ? writer : reader;
    if (!SetHandleInformation(parent, WCRT_HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(reader);
        CloseHandle(writer);
        errno = EIO;
        return NULL;
    }
    strcpy(command_line, prefix);
    strcat(command_line, command);
    memset(&startup, 0, sizeof(startup));
    memset(&process, 0, sizeof(process));
    startup.size = sizeof(startup);
    startup.flags = WCRT_STARTF_USESTDHANDLES;
    startup.standard_input = reading ? GetStdHandle((unsigned long)-10) : child;
    startup.standard_output = reading ? child :
        GetStdHandle((unsigned long)-11);
    startup.standard_error = GetStdHandle((unsigned long)-12);
    if (!CreateProcessA(NULL, command_line, NULL, NULL, 1, 0, NULL, NULL,
        &startup, &process)) {
        CloseHandle(reader);
        CloseHandle(writer);
        errno = EIO;
        return NULL;
    }
    CloseHandle(process.thread);
    CloseHandle(child);
    stream = __wcrt_adopt_file_handle(parent,
        reading ? WCRT_FILE_READ : WCRT_FILE_WRITE);
    if (stream == NULL) {
        CloseHandle(parent);
        CloseHandle(process.process);
        return NULL;
    }
    wcrt_popen_entries[slot].stream = stream;
    wcrt_popen_entries[slot].process = process.process;
    return stream;
}

int __wcrt_process_pclose(FILE *stream)
{
    unsigned long exit_code = (unsigned long)-1;
    unsigned long wait_result;
    int close_result;
    int status = 0;
    int slot;
    if (stream == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (slot = 0; slot < FOPEN_MAX; ++slot)
        if (wcrt_popen_entries[slot].stream == stream) break;
    if (slot == FOPEN_MAX) {
        errno = EINVAL;
        return -1;
    }
    close_result = fclose(stream);
    wait_result = WaitForSingleObject(wcrt_popen_entries[slot].process,
        WCRT_INFINITE);
    if (wait_result == (unsigned long)-1 ||
        !GetExitCodeProcess(wcrt_popen_entries[slot].process, &exit_code))
        status = -1;
    if (!CloseHandle(wcrt_popen_entries[slot].process)) status = -1;
    wcrt_popen_entries[slot].stream = NULL;
    wcrt_popen_entries[slot].process = NULL;
    if (close_result != 0 || status != 0) {
        errno = EIO;
        return -1;
    }
    return (int)exit_code;
}
