/**
 * @file stdio_stdin.c
 * @brief Verifies fgets with redirected and genuine Windows console input.
 */

#include <stdio.h>
#include <string.h>

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_STD_INPUT_HANDLE ((unsigned long)-10)
#define WCRT_KEY_EVENT 1
#define WCRT_GENERIC_READ 0x80000000UL
#define WCRT_GENERIC_WRITE 0x40000000UL
#define WCRT_SHARE_READ 1UL
#define WCRT_SHARE_WRITE 2UL
#define WCRT_OPEN_EXISTING 3UL
#define WCRT_INVALID_HANDLE ((void *)(long long)-1)

typedef struct wcrt_key_event_record {
    int key_down;
    unsigned short repeat_count;
    unsigned short virtual_key_code;
    unsigned short virtual_scan_code;
    union {
        unsigned short unicode_character;
        char ascii_character;
    } character;
    unsigned long control_key_state;
} wcrt_key_event_record;

typedef struct wcrt_input_record {
    unsigned short event_type;
    union {
        wcrt_key_event_record key_event;
        unsigned char padding[16];
    } event;
} wcrt_input_record;

__declspec(dllimport) int WCRT_WINAPI AllocConsole(void);
__declspec(dllimport) int WCRT_WINAPI FreeConsole(void);
__declspec(dllimport) void *WCRT_WINAPI CreateFileA(const char *path,
    unsigned long access, unsigned long sharing, void *security,
    unsigned long creation, unsigned long attributes, void *template_file);
__declspec(dllimport) int WCRT_WINAPI CloseHandle(void *handle);
__declspec(dllimport) void *WCRT_WINAPI GetStdHandle(unsigned long selector);
__declspec(dllimport) int WCRT_WINAPI SetStdHandle(unsigned long selector,
    void *handle);
__declspec(dllimport) int WCRT_WINAPI GetConsoleMode(void *handle,
    unsigned long *mode);
__declspec(dllimport) int WCRT_WINAPI WriteConsoleInputA(void *handle,
    const wcrt_input_record *records, unsigned long count,
    unsigned long *written);

static void set_key(wcrt_input_record *record, char character,
    unsigned short virtual_key_code)
{
    memset(record, 0, sizeof(*record));
    record->event_type = WCRT_KEY_EVENT;
    record->event.key_event.key_down = 1;
    record->event.key_event.repeat_count = 1;
    record->event.key_event.virtual_key_code = virtual_key_code;
    record->event.key_event.character.ascii_character = character;
}

static int test_console_input(void)
{
    wcrt_input_record records[2];
    unsigned long mode;
    unsigned long written;
    void *handle;
    char answer[8];

    FreeConsole();
    if (!AllocConsole()) {
        return 10;
    }
    handle = CreateFileA("CONIN$", WCRT_GENERIC_READ | WCRT_GENERIC_WRITE,
        WCRT_SHARE_READ | WCRT_SHARE_WRITE, NULL, WCRT_OPEN_EXISTING, 0, NULL);
    if (handle == NULL || handle == WCRT_INVALID_HANDLE) {
        return 11;
    }
    if (!SetStdHandle(WCRT_STD_INPUT_HANDLE, handle)) {
        CloseHandle(handle);
        return 12;
    }
    if (GetStdHandle(WCRT_STD_INPUT_HANDLE) != handle ||
        !GetConsoleMode(handle, &mode)) {
        CloseHandle(handle);
        return 13;
    }
    set_key(&records[0], 'n', 'N');
    set_key(&records[1], '\r', 13);
    if (!WriteConsoleInputA(handle, records, 2, &written) || written != 2) {
        CloseHandle(handle);
        return 14;
    }
    if (fgets(answer, sizeof(answer), stdin) == NULL ||
        strcmp(answer, "n\n") != 0) {
        CloseHandle(handle);
        return 15;
    }
    CloseHandle(handle);
    return 0;
}

int main(int argument_count, char **arguments)
{
    char answer[8];
    int expected;
    if (argument_count == 2 && strcmp(arguments[1], "console") == 0) {
        return test_console_input();
    }
    if (argument_count != 3 || strcmp(arguments[1], "redirected") != 0 ||
        arguments[2][0] == '\0' || arguments[2][1] != '\0') {
        return 1;
    }
    expected = (unsigned char)arguments[2][0];
    if (fgets(answer, sizeof(answer), stdin) == NULL ||
        answer[0] != expected || strcmp(answer + 1, "\n") != 0) {
        return 2;
    }
    if (fgets(answer, sizeof(answer), stdin) != NULL) {
        return 3;
    }
    if (!feof(stdin)) {
        return 4;
    }
    if (ferror(stdin)) {
        return 5;
    }
    return 0;
}
