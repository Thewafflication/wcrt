/** @file native-handle.c @brief Tests native ownership with owned handles. */
#define WCRT_POSIX 1
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

/** @brief Opens the selected fixture/device with permissive sharing. */
static HANDLE open_native(const char *path, DWORD creation, DWORD access)
{
    return CreateFileA(path, access,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
        creation, FILE_ATTRIBUTE_NORMAL, NULL);
}

/** @brief Makes a separately owned native copy of a borrowed source handle. */
static HANDLE duplicate_native(HANDLE source)
{
    HANDLE result = INVALID_HANDLE_VALUE;
    HANDLE process = GetCurrentProcess();
    if (!DuplicateHandle(process, source, process, &result, 0, FALSE,
        DUPLICATE_SAME_ACCESS)) return INVALID_HANDLE_VALUE;
    return result;
}

int main(int argument_count, char **arguments)
{
    HANDLE handle;
    HANDLE retained;
    HANDLE reader;
    HANDLE writer;
    FILE *stream;
    int descriptor;
    int read_descriptor;
    int write_descriptor;
    int index;
    int held[FOPEN_MAX];
    int invalid[] = {3, _O_TEXT | _O_BINARY, _O_CREAT, _O_TRUNC, _O_EXCL,
        0x80, -1};
    char bytes[8];
    if (argument_count != 2) return 1;
    if (_open_osfhandle(0, _O_BINARY) != -1 || errno != EBADF ||
        _open_osfhandle(-1, _O_BINARY) != -1 || errno != EBADF) return 2;
    handle = open_native(arguments[1], CREATE_ALWAYS,
        GENERIC_READ | GENERIC_WRITE);
    if (handle == INVALID_HANDLE_VALUE) return 3;
    for (index = 0; index < 140; ++index) {
        if (_open_osfhandle((__wcrt_intptr_t)handle, invalid[index % 7]) !=
            -1 || errno != EINVAL || GetFileType(handle) != FILE_TYPE_DISK)
            return 4;
    }
    retained = duplicate_native(handle);
    if (retained == INVALID_HANDLE_VALUE) return 5;
    errno = EDOM;
    descriptor = _open_osfhandle((__wcrt_intptr_t)handle, _O_RDWR | _O_TEXT);
    if (descriptor < 0 || errno != EDOM || _get_osfhandle(descriptor) !=
        (__wcrt_intptr_t)handle) return 6;
    if (_write(descriptor, "A\nB", 3) != 3 ||
        _filelength(descriptor) != 4 || _telli64(descriptor) != 4 ||
        _setmode(descriptor, _O_BINARY) != _O_TEXT ||
        _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 4) != 4 || memcmp(bytes, "A\r\nB", 4))
        return 7;
    stream = _fdopen(descriptor, "r+b");
    if (stream == NULL || fclose(stream) != 0 ||
        GetFileType(handle) != FILE_TYPE_UNKNOWN ||
        GetLastError() != ERROR_INVALID_HANDLE ||
        GetFileType(retained) != FILE_TYPE_DISK) return 8;
    descriptor = _open_osfhandle((__wcrt_intptr_t)retained, _O_RDONLY);
    if (descriptor < 0 || _setmode(descriptor, _O_BINARY) != _O_BINARY ||
        _write(descriptor, "X", 1) != -1 || errno != EBADF ||
        _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 4) != 4 || memcmp(bytes, "A\r\nB", 4) ||
        _close(descriptor) != 0) return 9;
    handle = open_native(arguments[1], OPEN_EXISTING,
        GENERIC_READ | GENERIC_WRITE);
    if (handle == INVALID_HANDLE_VALUE) return 10;
    descriptor = _open_osfhandle((__wcrt_intptr_t)handle,
        _O_RDWR | _O_APPEND | _O_BINARY);
    if (descriptor < 0 || _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _write(descriptor, "C", 1) != 1 || _filelength(descriptor) != 5 ||
        _telli64(descriptor) != 5 || _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 5) != 5 || memcmp(bytes, "A\r\nBC", 5) ||
        _close(descriptor) != 0) return 11;
    handle = open_native(arguments[1], OPEN_EXISTING, GENERIC_WRITE);
    if (handle == INVALID_HANDLE_VALUE) return 12;
    descriptor = _open_osfhandle((__wcrt_intptr_t)handle,
        _O_WRONLY | _O_BINARY);
    if (descriptor < 0 || _read(descriptor, bytes, 1) != -1 ||
        errno != EBADF || _write(descriptor, "W", 1) != 1 ||
        _close(descriptor) != 0) return 13;
    handle = open_native(arguments[1], OPEN_EXISTING, GENERIC_READ);
    if (handle == INVALID_HANDLE_VALUE) return 14;
    descriptor = _open_osfhandle((__wcrt_intptr_t)handle, _O_RDWR | _O_BINARY);
    if (descriptor < 0 || _write(descriptor, "X", 1) != -1 ||
        errno != EACCES || _close(descriptor) != 0) return 15;
    handle = open_native("NUL", OPEN_EXISTING, GENERIC_READ | GENERIC_WRITE);
    if (handle == INVALID_HANDLE_VALUE ||
        GetFileType(handle) != FILE_TYPE_CHAR) return 27;
    descriptor = _open_osfhandle((__wcrt_intptr_t)handle, _O_RDWR | _O_BINARY);
    if (descriptor < 0 || _write(descriptor, "N", 1) != 1 ||
        _read(descriptor, bytes, 1) != 0 || _close(descriptor) != 0)
        return 28;
    handle = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (handle == NULL || _open_osfhandle((__wcrt_intptr_t)handle,
        _O_BINARY) != -1 || errno != EBADF || !CloseHandle(handle))
        return 16;
    if (!CreatePipe(&reader, &writer, NULL, 0)) return 17;
    if (_open_osfhandle((__wcrt_intptr_t)reader, _O_APPEND) != -1 ||
        errno != EINVAL || GetFileType(reader) != FILE_TYPE_PIPE) return 18;
    read_descriptor = _open_osfhandle((__wcrt_intptr_t)reader, _O_BINARY);
    write_descriptor = _open_osfhandle((__wcrt_intptr_t)writer,
        _O_WRONLY | _O_BINARY);
    if (read_descriptor < 0 || write_descriptor < 0 ||
        _write(write_descriptor, "\n", 1) != 1 ||
        _read(read_descriptor, bytes, 1) != 1 || bytes[0] != '\n' ||
        _close(write_descriptor) != 0 ||
        _read(read_descriptor, bytes, 1) != 0 ||
        _close(read_descriptor) != 0) return 19;
    handle = open_native(arguments[1], OPEN_EXISTING, GENERIC_READ);
    if (handle == INVALID_HANDLE_VALUE) return 20;
    for (index = 0; index < FOPEN_MAX; ++index) {
        retained = duplicate_native(handle);
        if (retained == INVALID_HANDLE_VALUE) return 21;
        held[index] = _open_osfhandle((__wcrt_intptr_t)retained, _O_BINARY);
        if (held[index] < 0) return 22;
    }
    retained = duplicate_native(handle);
    if (retained == INVALID_HANDLE_VALUE ||
        _open_osfhandle((__wcrt_intptr_t)retained, _O_BINARY) != -1 ||
        errno != EMFILE || GetFileType(retained) != FILE_TYPE_DISK ||
        !CloseHandle(retained)) return 23;
    if (_close(held[0]) != 0) return 24;
    held[0] = _open_osfhandle((__wcrt_intptr_t)handle, _O_BINARY);
    if (held[0] < 0 || _get_osfhandle(held[0]) !=
        (__wcrt_intptr_t)handle) return 25;
    for (index = 0; index < FOPEN_MAX; ++index) {
        if (_close(held[index]) != 0) return 26;
    }
    return 0;
}
