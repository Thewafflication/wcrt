/** @file popen.c @brief Implements Windows shell command streams. */
#define WCRT_POSIX 1
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "../../internal/file.h"

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif
#define WCRT_INFINITE 0xffffffffUL
#define WCRT_STARTF_USESTDHANDLES 0x100UL
#define WCRT_HANDLE_FLAG_INHERIT 1UL
struct wcrt_security_attributes {
    unsigned long size;
    void *descriptor;
    int inherit;
};
struct wcrt_popen_startup {
    unsigned long size; char *reserved; char *desktop; char *title;
    unsigned long x, y, x_size, y_size, x_chars, y_chars, fill, flags;
    unsigned short show, reserved_count; unsigned char *reserved_bytes;
    void *input; void *output; void *error;
};
struct wcrt_popen_process {
    void *process;
    void *thread;
    unsigned long process_id;
    unsigned long thread_id;
};
struct wcrt_popen_entry { FILE *stream; void *process; };
static struct wcrt_popen_entry entries[FOPEN_MAX];
__declspec(dllimport) int WCRT_WINAPI CreateProcessA(const char *, char *,
    void *, void *, int, unsigned long, void *, const char *,
    struct wcrt_popen_startup *, struct wcrt_popen_process *);
__declspec(dllimport) unsigned long WCRT_WINAPI WaitForSingleObject(void *,
    unsigned long);
__declspec(dllimport) int WCRT_WINAPI GetExitCodeProcess(void *,
    unsigned long *);
__declspec(dllimport) int WCRT_WINAPI CloseHandle(void *);
__declspec(dllimport) void *WCRT_WINAPI GetStdHandle(unsigned long);
__declspec(dllimport) int WCRT_WINAPI CreatePipe(void **, void **,
    struct wcrt_security_attributes *, unsigned long);
__declspec(dllimport) int WCRT_WINAPI SetHandleInformation(void *,
    unsigned long, unsigned long);

FILE *__wcrt_process_popen(const char *command, const char *mode)
{
    char line[32768];
    const char prefix[] = "cmd.exe /C ";
    struct wcrt_security_attributes security;
    struct wcrt_popen_startup startup;
    struct wcrt_popen_process process;
    void *reader;
    void *writer;
    void *parent;
    void *child;
    FILE *stream;
    int reading;
    int slot;
    size_t length;
    if (command == NULL || mode == NULL ||
        (*mode != 'r' && *mode != 'w') ||
        (mode[1] != '\0' && !((mode[1] == 'b' || mode[1] == 't') &&
        mode[2] == '\0'))) {
        errno = EINVAL;
        return NULL;
    }
    for (slot = 0; slot < FOPEN_MAX; ++slot)
        if (entries[slot].stream == NULL) break;
    if (slot == FOPEN_MAX) {
        errno = EMFILE;
        return NULL;
    }
    length = strlen(command);
    if (length + sizeof(prefix) > sizeof(line)) {
        errno = EINVAL;
        return NULL;
    }
    security.size = sizeof(security);
    security.descriptor = NULL;
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
    strcpy(line, prefix);
    strcat(line, command);
    memset(&startup, 0, sizeof(startup));
    memset(&process, 0, sizeof(process));
    startup.size = sizeof(startup);
    startup.flags = WCRT_STARTF_USESTDHANDLES;
    startup.input = reading ? GetStdHandle((unsigned long)-10) : child;
    startup.output = reading ? child : GetStdHandle((unsigned long)-11);
    startup.error = GetStdHandle((unsigned long)-12);
    if (!CreateProcessA(NULL, line, NULL, NULL, 1, 0, NULL, NULL,
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
    entries[slot].stream = stream;
    entries[slot].process = process.process;
    return stream;
}

int __wcrt_process_pclose(FILE *stream)
{
    unsigned long code = (unsigned long)-1;
    unsigned long wait;
    int slot;
    int closed;
    int status = 0;
    if (stream == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (slot = 0; slot < FOPEN_MAX; ++slot)
        if (entries[slot].stream == stream) break;
    if (slot == FOPEN_MAX) {
        errno = EINVAL;
        return -1;
    }
    closed = fclose(stream);
    wait = WaitForSingleObject(entries[slot].process, WCRT_INFINITE);
    if (wait == (unsigned long)-1 ||
        !GetExitCodeProcess(entries[slot].process, &code)) status = -1;
    if (!CloseHandle(entries[slot].process)) status = -1;
    entries[slot].stream = NULL;
    entries[slot].process = NULL;
    if (closed != 0 || status != 0) {
        errno = EIO;
        return -1;
    }
    return (int)code;
}
