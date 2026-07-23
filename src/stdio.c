/**
 * @file stdio.c
 * @brief Implements C89 streams, files, and unformatted input and output.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "internal/file.h"

FILE __wcrt_stdin;
FILE __wcrt_stdout;
FILE __wcrt_stderr;

/** @brief Storage for dynamically opened streams. */
static FILE wcrt_streams[FOPEN_MAX];
/** @brief Storage returned when tmpnam receives a null pointer. */
static char wcrt_temporary_name[L_tmpnam];

/** @brief Initializes a standard stream when first referenced. */
static void wcrt_prepare_stream(FILE *stream)
{
    if (stream == stdin) {
        __wcrt_file_initialize_standard(stream, -10, WCRT_FILE_READ);
    } else if (stream == stdout) {
        __wcrt_file_initialize_standard(stream, -11, WCRT_FILE_WRITE);
    } else if (stream == stderr) {
        __wcrt_file_initialize_standard(stream, -12, WCRT_FILE_WRITE);
    }
}

/** @brief Finds an unused dynamic stream slot. */
static FILE *wcrt_allocate_stream(void)
{
    int index;
    for (index = 0; index < FOPEN_MAX; ++index) {
        if (wcrt_streams[index].handle == NULL) {
            return &wcrt_streams[index];
        }
    }
    return NULL;
}

int remove(const char *path)
{
    return __wcrt_file_remove(path);
}

int rename(const char *old_path, const char *new_path)
{
    return __wcrt_file_rename(old_path, new_path);
}

char *tmpnam(char *name)
{
    char *result = name == NULL ? wcrt_temporary_name : name;
    return __wcrt_file_temporary(result) == 0 ? result : NULL;
}

FILE *tmpfile(void)
{
    char path[L_tmpnam];
    FILE *stream;
    if (tmpnam(path) == NULL) {
        return NULL;
    }
    stream = fopen(path, "w+b");
    if (stream == NULL) {
        remove(path);
        return NULL;
    }
    stream->flags |= WCRT_FILE_DELETE;
    strcpy(stream->delete_path, path);
    return stream;
}

FILE *fopen(const char *path, const char *mode)
{
    FILE *stream = wcrt_allocate_stream();
    if (stream == NULL || __wcrt_file_open(stream, path, mode) != 0) {
        return NULL;
    }
    return stream;
}

FILE *freopen(const char *path, const char *mode, FILE *stream)
{
    if (stream == NULL) {
        return NULL;
    }
    wcrt_prepare_stream(stream);
    __wcrt_file_close(stream);
    return __wcrt_file_open(stream, path, mode) == 0 ? stream : NULL;
}

int fclose(FILE *stream)
{
    if (stream == NULL) {
        return EOF;
    }
    wcrt_prepare_stream(stream);
    return __wcrt_file_close(stream) == 0 ? 0 : EOF;
}

int fflush(FILE *stream)
{
    (void)stream;
    return 0;
}

int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
    if (stream == NULL || (mode != _IOFBF && mode != _IOLBF &&
        mode != _IONBF)) {
        return -1;
    }
    stream->buffer = buffer;
    stream->buffer_size = size;
    stream->buffering = mode;
    return 0;
}

void setbuf(FILE *stream, char *buffer)
{
    setvbuf(stream, buffer, buffer == NULL ? _IONBF : _IOFBF, BUFSIZ);
}

int fgetc(FILE *stream)
{
    unsigned char character;
    size_t transferred;
    if (stream == NULL) {
        return EOF;
    }
    wcrt_prepare_stream(stream);
    if (!(stream->flags & WCRT_FILE_READ)) {
        stream->error = 1;
        return EOF;
    }
    if (stream->pushback != EOF) {
        int result = stream->pushback;
        stream->pushback = EOF;
        return result;
    }
    if (__wcrt_file_read(stream, &character, 1, &transferred) != 0) {
        stream->error = 1;
        return EOF;
    }
    if (transferred == 0) {
        stream->end_of_file = 1;
        return EOF;
    }
    if (!(stream->flags & WCRT_FILE_BINARY) && character == '\r') {
        int next = fgetc(stream);
        if (next == '\n') {
            return '\n';
        }
        if (next != EOF) {
            ungetc(next, stream);
        }
    }
    return character;
}

int getc(FILE *stream)
{
    return fgetc(stream);
}

int getchar(void)
{
    return fgetc(stdin);
}

int ungetc(int character, FILE *stream)
{
    if (stream == NULL || character == EOF || stream->pushback != EOF) {
        return EOF;
    }
    stream->pushback = (unsigned char)character;
    stream->end_of_file = 0;
    return stream->pushback;
}

char *fgets(char *destination, int count, FILE *stream)
{
    int length = 0;
    int character;
    if (destination == NULL || count <= 0) {
        return NULL;
    }
    while (length + 1 < count && (character = fgetc(stream)) != EOF) {
        destination[length++] = (char)character;
        if (character == '\n') {
            break;
        }
    }
    if (length == 0 && character == EOF) {
        return NULL;
    }
    destination[length] = '\0';
    return destination;
}

char *gets(char *destination)
{
    int character;
    char *cursor = destination;
    if (destination == NULL) {
        return NULL;
    }
    while ((character = getchar()) != EOF && character != '\n') {
        *cursor++ = (char)character;
    }
    if (cursor == destination && character == EOF) {
        return NULL;
    }
    *cursor = '\0';
    return destination;
}

int fputc(int character, FILE *stream)
{
    unsigned char byte = (unsigned char)character;
    size_t transferred;
    if (stream == NULL) {
        return EOF;
    }
    wcrt_prepare_stream(stream);
    if (!(stream->flags & WCRT_FILE_WRITE)) {
        stream->error = 1;
        return EOF;
    }
    if (!(stream->flags & WCRT_FILE_BINARY) && byte == '\n') {
        unsigned char carriage_return = '\r';
        if (__wcrt_file_write(stream, &carriage_return, 1, &transferred) != 0 ||
            transferred != 1) {
            stream->error = 1;
            return EOF;
        }
    }
    if (__wcrt_file_write(stream, &byte, 1, &transferred) != 0 ||
        transferred != 1) {
        stream->error = 1;
        return EOF;
    }
    return byte;
}

int putc(int character, FILE *stream)
{
    return fputc(character, stream);
}

int putchar(int character)
{
    return fputc(character, stdout);
}

int fputs(const char *string, FILE *stream)
{
    while (*string != '\0') {
        if (fputc((unsigned char)*string++, stream) == EOF) {
            return EOF;
        }
    }
    return 0;
}

int puts(const char *string)
{
    return fputs(string, stdout) == EOF || fputc('\n', stdout) == EOF ?
        EOF : 0;
}

size_t fread(void *destination, size_t size, size_t count, FILE *stream)
{
    unsigned char *output = (unsigned char *)destination;
    size_t total;
    size_t index;
    if (size != 0 && count > (size_t)-1 / size) {
        return 0;
    }
    total = size * count;
    for (index = 0; index < total; ++index) {
        int character = fgetc(stream);
        if (character == EOF) {
            break;
        }
        output[index] = (unsigned char)character;
    }
    return size == 0 ? 0 : index / size;
}

size_t fwrite(const void *source, size_t size, size_t count, FILE *stream)
{
    const unsigned char *input = (const unsigned char *)source;
    size_t total;
    size_t index;
    if (size != 0 && count > (size_t)-1 / size) {
        return 0;
    }
    total = size * count;
    for (index = 0; index < total; ++index) {
        if (fputc(input[index], stream) == EOF) {
            break;
        }
    }
    return size == 0 ? 0 : index / size;
}

int fgetpos(FILE *stream, fpos_t *position)
{
    long long current;
    wcrt_prepare_stream(stream);
    if (position == NULL || __wcrt_file_seek(stream, 0, SEEK_CUR,
        &current) != 0) {
        return -1;
    }
    *position = current - (stream->pushback != EOF ? 1 : 0);
    return 0;
}

int fseek(FILE *stream, long offset, int origin)
{
    wcrt_prepare_stream(stream);
    if (__wcrt_file_seek(stream, offset, origin, NULL) != 0) {
        stream->error = 1;
        return -1;
    }
    stream->end_of_file = 0;
    stream->pushback = EOF;
    return 0;
}

int fsetpos(FILE *stream, const fpos_t *position)
{
    if (position == NULL || __wcrt_file_seek(stream, *position, SEEK_SET,
        NULL) != 0) {
        return -1;
    }
    stream->end_of_file = 0;
    stream->pushback = EOF;
    return 0;
}

long ftell(FILE *stream)
{
    fpos_t position;
    if (fgetpos(stream, &position) != 0 || position > 2147483647LL ||
        position < -2147483647LL - 1LL) {
        return -1L;
    }
    return (long)position;
}

void rewind(FILE *stream)
{
    clearerr(stream);
    fseek(stream, 0, SEEK_SET);
}

void clearerr(FILE *stream)
{
    stream->error = 0;
    stream->end_of_file = 0;
}

int feof(FILE *stream)
{
    return stream->end_of_file;
}

int ferror(FILE *stream)
{
    return stream->error;
}

void perror(const char *prefix)
{
    if (prefix != NULL && *prefix != '\0') {
        fputs(prefix, stderr);
        fputs(": ", stderr);
    }
    fputs(strerror(errno), stderr);
    fputc('\n', stderr);
}
