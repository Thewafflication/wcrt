/** @file utime.c @brief Tests Microsoft explicit-width file times. */
#include <stdio.h>
#include <sys/utime.h>
#include <windows.h>

/**
 * @brief Converts a Windows file time to Unix seconds.
 * @param value Windows file time.
 * @return Signed seconds since the Unix epoch.
 */
static long long file_seconds(FILETIME value)
{
    ULARGE_INTEGER time;
    time.LowPart = value.dwLowDateTime;
    time.HighPart = value.dwHighDateTime;
    return (long long)(time.QuadPart / 10000000ULL) - 11644473600LL;
}

/**
 * @brief Runs explicit-width file-time checks.
 * @param argc Argument count.
 * @param argv Test path in element one.
 * @return Zero on success.
 */
int main(int argc, char **argv)
{
    struct __utimbuf32 time32;
    struct __utimbuf64 time64;
    WIN32_FILE_ATTRIBUTE_DATA data;
    FILE *stream;
    if (argc != 2) return 1;
    stream = fopen(argv[1], "wb");
    if (stream == NULL || fclose(stream) != 0) return 2;
    time32.actime = 946684800L;
    time32.modtime = 946684900L;
    if (_utime32(argv[1], &time32) != 0) return 3;
    if (!GetFileAttributesExA(argv[1], GetFileExInfoStandard, &data)) return 4;
    if (file_seconds(data.ftLastAccessTime) != time32.actime) return 5;
    if (file_seconds(data.ftLastWriteTime) != time32.modtime) return 6;
    time64.actime = 2208988800LL;
    time64.modtime = 2208988900LL;
    if (_utime64(argv[1], &time64) != 0) return 7;
    if (!GetFileAttributesExA(argv[1], GetFileExInfoStandard, &data)) return 8;
    if (file_seconds(data.ftLastWriteTime) != time64.modtime) return 9;
    if (_utime64(argv[1], NULL) != 0) return 10;
    if (remove(argv[1]) != 0) return 11;
    return 0;
}
