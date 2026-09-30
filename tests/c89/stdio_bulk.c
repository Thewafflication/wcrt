/** @file stdio_bulk.c @brief Regresses issue 5 bulk binary stream reads. */

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "../../src/internal/file.h"

#define TEST_WINAPI __attribute__((stdcall))
/** @brief Windows process I/O accounting layout. */
struct test_io_counters {
    unsigned long long read_operations;
    unsigned long long write_operations;
    unsigned long long other_operations;
    unsigned long long read_bytes;
    unsigned long long write_bytes;
    unsigned long long other_bytes;
};
__declspec(dllimport) void *TEST_WINAPI GetCurrentProcess(void);
__declspec(dllimport) void *TEST_WINAPI GetModuleHandleA(const char *name);
__declspec(dllimport) void *TEST_WINAPI GetProcAddress(void *module,
    const char *name);
/** @brief Resolves I/O accounting absent from some TinyCC import files. */
static int read_counters(struct test_io_counters *counters)
{
    typedef int (TEST_WINAPI *query_type)(void *, struct test_io_counters *);
    query_type query = (query_type)GetProcAddress(
        GetModuleHandleA("kernel32.dll"), "GetProcessIoCounters");
    return query != NULL && query(GetCurrentProcess(), counters);
}

/** @brief Verifies contents and bounds native reads for each buffering mode. */
static int check_bulk(const char *path, int mode)
{
    unsigned char buffer[65536];
    char stream_buffer[65536];
    struct test_io_counters before, after;
    FILE *stream = fopen(path, "rb");
    size_t total = 0, amount, index;
    if (stream == NULL) return 1;
    if (mode >= 0 && setvbuf(stream, mode == _IOFBF ? NULL :
        stream_buffer, mode == 3 ? _IOFBF : mode,
        sizeof(stream_buffer)) != 0) return 2;
    if (!read_counters(&before)) return 3;
    while ((amount = fread(buffer, 1, sizeof(buffer), stream)) != 0) {
        for (index = 0; index < amount; ++index) {
            if (buffer[index] != (unsigned char)((total + index) * 37)) {
                return 4;
            }
        }
        total += amount;
    }
    if (!read_counters(&after)) return 5;
    /* Sixteen full chunks, one short chunk and EOF, with generous headroom
     * for incidental process I/O; byte-at-a-time reads exceed one million.
     */
    if (after.read_operations - before.read_operations > 64) return 6;
    printf("Bulk fread mode %d: %llu reads for %lu bytes\n", mode,
        after.read_operations - before.read_operations, (unsigned long)total);
    if (total != 16 * sizeof(buffer) + 7 || !feof(stream) ||
        ferror(stream)) return 7;
    return fclose(stream) == 0 ? 0 : 8;
}

/** @brief Exercises partial elements, pushback, errors and stream state. */
static int check_state(const char *path)
{
    FILE *stream = fopen(path, "rb");
    unsigned char buffer[32];
    void *handle;
    if (stream == NULL) return 20;
    if (fread(buffer, 0, 5, stream) != 0 ||
        fread(buffer, 5, 0, stream) != 0 ||
        fread(buffer, (size_t)-1, 2, stream) != 0 ||
        fwide(stream, 0) != 0) return 21;
    if (fseek(stream, -7, SEEK_END) != 0 ||
        fread(buffer, 3, 3, stream) != 2 || !feof(stream) ||
        ferror(stream) || buffer[6] != (unsigned char)(6 * 37)) return 22;
    if (ungetc('Z', stream) != 'Z' || feof(stream) ||
        fread(buffer, 1, 2, stream) != 1 || buffer[0] != 'Z' ||
        !feof(stream)) return 23;
    rewind(stream);
    if (ungetc('Q', stream) != 'Q' || fread(buffer, 2, 3, stream) != 3 ||
        buffer[0] != 'Q' || buffer[1] != 0 || buffer[5] != 148 ||
        ftell(stream) != 5 || fgetc(stream) != 185 || feof(stream)) return 24;
    if (fseek(stream, -2, SEEK_CUR) != 0 ||
        fread(buffer, 1, 1, stream) != 1 || buffer[0] != 148) return 25;
    handle = stream->handle;
    stream->handle = (void *)-1;
    if (ungetc('E', stream) != 'E' ||
        fread(buffer, 1, 2, stream) != 1 || buffer[0] != 'E' ||
        !ferror(stream) || feof(stream)) return 26;
    stream->handle = handle;
    clearerr(stream);
    if (fread(buffer, 1, 1, stream) != 1 || ferror(stream)) return 27;
    fclose(stream);
    stream = fopen(path, "rb");
    if (stream == NULL || fwide(stream, 1) <= 0 ||
        fread(buffer, 1, 1, stream) != 0 || !ferror(stream)) return 28;
    fclose(stream);
    stream = fopen(path, "wb");
    if (stream == NULL || fread(buffer, 1, 1, stream) != 0 ||
        !ferror(stream) || feof(stream)) return 29;
    fclose(stream);
    stream = fopen(path, "rb");
    if (stream == NULL || fread(buffer, 2, 2, stream) != 0 ||
        !feof(stream) || ferror(stream)) return 30;
    fclose(stream);
    stream = fopen(path, "wb");
    if (stream == NULL || fwrite("a\r\nb\rc", 1, 6, stream) != 6) return 31;
    fclose(stream);
    stream = fopen(path, "r");
    if (stream == NULL || fread(buffer, 1, 8, stream) != 5 ||
        memcmp(buffer, "a\nb\rc", 5) != 0 || !feof(stream)) return 32;
    return fclose(stream) == 0 ? 0 : 33;
}

/** @brief Runs operation-count and stream-semantics regression checks. */
int main(void)
{
    const char *path = "wcrt-bulk-read.tmp";
    unsigned char buffer[65536];
    FILE *stream = fopen(path, "wb");
    size_t index;
    int chunk, result, mode;
    if (stream == NULL) return 40;
    for (index = 0; index < sizeof(buffer); ++index) {
        buffer[index] = (unsigned char)(index * 37);
    }
    for (chunk = 0; chunk < 16; ++chunk) {
        if (fwrite(buffer, 1, sizeof(buffer), stream) != sizeof(buffer)) {
            return 41;
        }
    }
    if (fwrite(buffer, 1, 7, stream) != 7 || fclose(stream) != 0) return 42;
    for (mode = -1; mode <= 3; ++mode) {
        result = check_bulk(path, mode);
        if (result != 0) return result;
    }
    result = check_state(path);
    remove(path);
    return result;
}
