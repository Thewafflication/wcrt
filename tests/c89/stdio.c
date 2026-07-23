/**
 * @file stdio.c
 * @brief Exercises C89 stream, file, format, scan, and indicator facilities.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/** @brief Calls vsprintf for direct va_list coverage. */
static int call_vsprintf(char *output, const char *format, ...)
{
    va_list arguments; int result;
    va_start(arguments, format);
    result = vsprintf(output, format, arguments);
    va_end(arguments);
    return result;
}

/** @brief Calls vfprintf for direct va_list coverage. */
static int call_vfprintf(FILE *stream, const char *format, ...)
{
    va_list arguments; int result;
    va_start(arguments, format);
    result = vfprintf(stream, format, arguments);
    va_end(arguments);
    return result;
}

/**
 * @brief Runs file, stream, formatting, scanning, and temporary-file checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    const char *first_path = "wcrt-tc-0012-a.tmp";
    const char *second_path = "wcrt-tc-0012-b.tmp";
    FILE *stream;
    char buffer[256];
    char word[16];
    char scanset[16];
    int integer, count_written, count_read;
    unsigned int hexadecimal;
    float real;
    fpos_t position;
    size_t amount;

    remove(first_path);
    remove(second_path);
    stream = fopen(first_path, "w+b");
    if (stream == NULL ||
        setvbuf(stream, buffer, _IOFBF, sizeof(buffer)) != 0) {
        return 1;
    }
    setbuf(stream, NULL);
    if (fputs("line\n", stream) == EOF || fputc('A', stream) == EOF ||
        putc('B', stream) == EOF || call_vfprintf(stream, "%d", 12) != 2) {
        return 2;
    }
    if (fflush(stream) != 0 || ftell(stream) != 9 ||
        fgetpos(stream, &position) != 0) {
        return 3;
    }
    rewind(stream);
    if (fgets(buffer, sizeof(buffer), stream) == NULL ||
        strcmp(buffer, "line\n") != 0 || getc(stream) != 'A' ||
        ungetc('Z', stream) != 'Z' || fgetc(stream) != 'Z') {
        return 4;
    }
    if (fsetpos(stream, &position) != 0 || fseek(stream, -2, SEEK_CUR) != 0 ||
        ftell(stream) != 7) {
        return 5;
    }
    rewind(stream);
    amount = fread(buffer, 1, 9, stream);
    if (amount != 9 || fread(buffer, 1, 1, stream) != 0 || !feof(stream)) {
        return 6;
    }
    clearerr(stream);
    if (feof(stream) || ferror(stream)) return 7;
    if (fclose(stream) != 0 || rename(first_path, second_path) != 0) return 8;
    stream = freopen(second_path, "rb", fopen(second_path, "rb"));
    if (stream == NULL || fclose(stream) != 0 || remove(second_path) != 0) {
        return 9;
    }
    stream = tmpfile();
    if (stream == NULL || fwrite("abc", 1, 3, stream) != 3 ||
        fclose(stream) != 0 || tmpnam(buffer) == NULL) {
        return 10;
    }
    count_written = -1;
    if (sprintf(buffer, "%+06d %#x %#o %.2f %e %g %c %.3s%n %%",
        12, 42, 9, 1.25, 10.0, 2.5, 'Q', "string", &count_written) <= 0 ||
        count_written <= 0 || strstr(buffer, "+00012") == NULL ||
        strstr(buffer, "0x2a") == NULL) {
        return 11;
    }
    if (call_vsprintf(buffer, "%ld %u %X", 123L, 7U, 255U) <= 0 ||
        strcmp(buffer, "123 7 FF") != 0) {
        return 12;
    }
    count_read = -1;
    if (sscanf(" -12 0x2a 1.5 word abc123", "%d %x %f %s %3[a-z]%n",
        &integer, &hexadecimal, &real, word, scanset, &count_read) != 5 ||
        integer != -12 || hexadecimal != 42 || real != 1.5f ||
        strcmp(word, "word") != 0 || strcmp(scanset, "abc") != 0 ||
        count_read <= 0) {
        return 13;
    }
    stream = fopen(first_path, "w+");
    if (stream == NULL || fprintf(stream, "%d %s", 17, "scan") != 7) {
        return 14;
    }
    rewind(stream);
    if (fscanf(stream, "%d %s", &integer, word) != 2 || integer != 17 ||
        strcmp(word, "scan") != 0) {
        return 15;
    }
    fclose(stream);
    remove(first_path);
    if (printf("") != 0 || vprintf == NULL || stdin == NULL ||
        stdout == NULL || stderr == NULL) {
        return 16;
    }
    return 0;
}
