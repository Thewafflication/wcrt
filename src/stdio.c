/**
 * @file stdio.c
 * @brief Implements C89 streams, files, and unformatted input and output.
 */

#define WCRT_POSIX 1

#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "internal/file.h"
#include "internal/stdlib.h"

FILE __wcrt_stdin;
FILE __wcrt_stdout;
FILE __wcrt_stderr;

/** @brief Storage for dynamically opened streams. */
static FILE wcrt_streams[FOPEN_MAX];
/** @brief Storage returned when tmpnam receives a null pointer. */
static char wcrt_temporary_name[L_tmpnam];

/** @brief Initializes a standard stream when first referenced. */
void __wcrt_prepare_stream(FILE *stream)
{
    if (stream == stdin) {
        __wcrt_file_initialize_standard(stream, -10, 0, WCRT_FILE_READ);
    } else if (stream == stdout) {
        __wcrt_file_initialize_standard(stream, -11, 1, WCRT_FILE_WRITE);
    } else if (stream == stderr) {
        __wcrt_file_initialize_standard(stream, -12, 2, WCRT_FILE_WRITE);
    }
}

void __wcrt_reset_stream_conversion(FILE *stream)
{
    stream->wide_state.__value = 0;
    stream->wide_state.__bytes = 0;
    stream->wide_state.__state = 0;
    stream->wide_pushback = WEOF;
    stream->has_wide_pushback = 0;
}

int __wcrt_orient_stream(FILE *stream, int mode)
{
    if (stream == NULL) return 0;
    __wcrt_prepare_stream(stream);
    if (stream->orientation == WCRT_ORIENTATION_NONE && mode != 0) {
        stream->orientation = mode > 0 ? WCRT_ORIENTATION_WIDE :
            WCRT_ORIENTATION_BYTE;
        __wcrt_reset_stream_conversion(stream);
    }
    return stream->orientation;
}

int __wcrt_require_orientation(FILE *stream, int orientation)
{
    int actual = __wcrt_orient_stream(stream, orientation);
    if (actual == orientation) return 0;
    stream->error = 1;
    errno = EINVAL;
    return -1;
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

/** @brief Resolves an open WCRT descriptor to its shared stream slot. */
static FILE *wcrt_descriptor_stream(int descriptor)
{
    FILE *stream;
    if (descriptor == 0) stream = stdin;
    else if (descriptor == 1) stream = stdout;
    else if (descriptor == 2) stream = stderr;
    else if (descriptor >= 3 && descriptor < FOPEN_MAX + 3)
        stream = &wcrt_streams[descriptor - 3];
    else return NULL;
    __wcrt_prepare_stream(stream);
    return stream->handle != NULL && stream->descriptor == descriptor ?
        stream : NULL;
}

/** @brief Resolves any representable descriptor to its stream slot. */
static FILE *wcrt_descriptor_slot(int descriptor)
{
    if (descriptor == 0) return stdin;
    if (descriptor == 1) return stdout;
    if (descriptor == 2) return stderr;
    if (descriptor >= 3 && descriptor < FOPEN_MAX + 3)
        return &wcrt_streams[descriptor - 3];
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
    stream->descriptor = (int)(stream - wcrt_streams) + 3;
    return stream;
}

errno_t fopen_s(FILE **stream, const char *path, const char *mode)
{
    if (stream == NULL || path == NULL || mode == NULL) {
        errno = EINVAL;
        return EINVAL;
    }
    *stream = fopen(path, mode);
    if (*stream == NULL) {
        if (errno == 0) errno = ENOENT;
        return errno;
    }
    return 0;
}

FILE *freopen(const char *path, const char *mode, FILE *stream)
{
    int descriptor;
    if (stream == NULL) {
        return NULL;
    }
    __wcrt_prepare_stream(stream);
    descriptor = stream->descriptor;
    __wcrt_file_close(stream);
    if (__wcrt_file_open(stream, path, mode) != 0) {
        return NULL;
    }
    stream->descriptor = descriptor;
    return stream;
}

int fclose(FILE *stream)
{
    if (stream == NULL) {
        return EOF;
    }
    __wcrt_prepare_stream(stream);
    return __wcrt_file_close(stream) == 0 ? 0 : EOF;
}

int _fileno(FILE *stream)
{
    if (stream == NULL) {
        errno = EINVAL;
        return -1;
    }
    __wcrt_prepare_stream(stream);
    if ((stream == stdout || stream == stderr) &&
        (stream->handle == NULL ||
        stream->handle == (void *)(long long)-1)) {
        return -2;
    }
    return stream->descriptor;
}

int fileno(FILE *stream)
{
    return _fileno(stream);
}

__wcrt_intptr_t _get_osfhandle(int descriptor)
{
    FILE *stream = wcrt_descriptor_stream(descriptor);
    if (stream == NULL) {
        errno = EBADF;
        return (__wcrt_intptr_t)-1;
    }
    return (__wcrt_intptr_t)stream->handle;
}

int _open(const char *path, int flags, ...)
{
    FILE *stream = wcrt_allocate_stream();
    if (stream == NULL) {
        errno = EMFILE;
        return -1;
    }
    if (__wcrt_file_open_flags(stream, path, flags) != 0) return -1;
    stream->descriptor = (int)(stream - wcrt_streams) + 3;
    return stream->descriptor;
}

int open(const char *path, int flags, ...)
{
    return _open(path, (flags & ~_O_TEXT) | _O_BINARY);
}

int __wcrt_open_directory_descriptor(const char *path)
{
    FILE *stream = wcrt_allocate_stream();
    if (stream == NULL) {
        errno = EMFILE;
        return -1;
    }
    if (__wcrt_file_open_directory(stream, path) != 0) return -1;
    stream->descriptor = (int)(stream - wcrt_streams) + 3;
    return stream->descriptor;
}

FILE *__wcrt_adopt_file_handle(void *handle, unsigned int flags)
{
    FILE *stream = wcrt_allocate_stream();
    if (stream == NULL) {
        errno = EMFILE;
        return NULL;
    }
    memset(stream, 0, sizeof(*stream));
    stream->handle = handle;
    stream->flags = flags | WCRT_FILE_OWNED | WCRT_FILE_BINARY;
    stream->descriptor = (int)(stream - wcrt_streams) + 3;
    stream->pushback = EOF;
    stream->wide_pushback = WEOF;
    stream->buffering = _IOFBF;
    return stream;
}

FILE *_popen(const char *command, const char *mode)
{
    return __wcrt_process_popen(command, mode);
}

int _pclose(FILE *stream) { return __wcrt_process_pclose(stream); }

FILE *popen(const char *command, const char *mode)
{
    return _popen(command, mode);
}

int pclose(FILE *stream) { return _pclose(stream); }

int _close(int descriptor)
{
    FILE *stream = wcrt_descriptor_stream(descriptor);
    if (stream == NULL) {
        errno = EBADF;
        return -1;
    }
    return fclose(stream);
}

int close(int descriptor) { return _close(descriptor); }

int _read(int descriptor, void *buffer, unsigned int count)
{
    FILE *stream = wcrt_descriptor_stream(descriptor);
    size_t transferred;
    if (stream == NULL) {
        errno = EBADF;
        return -1;
    }
    if (buffer == NULL && count != 0) {
        errno = EINVAL;
        return -1;
    }
    if ((stream->flags & WCRT_FILE_READ) == 0) {
        errno = EBADF;
        return -1;
    }
    transferred = fread(buffer, 1, count > 0x7fffffffU ?
        0x7fffffffU : count, stream);
    return transferred == 0 && ferror(stream) ? -1 : (int)transferred;
}

int _write(int descriptor, const void *buffer, unsigned int count)
{
    FILE *stream = wcrt_descriptor_stream(descriptor);
    size_t transferred;
    if (stream == NULL) {
        errno = EBADF;
        return -1;
    }
    if (buffer == NULL && count != 0) {
        errno = EINVAL;
        return -1;
    }
    if ((stream->flags & WCRT_FILE_WRITE) == 0) {
        errno = EBADF;
        return -1;
    }
    transferred = fwrite(buffer, 1, count > 0x7fffffffU ?
        0x7fffffffU : count, stream);
    return transferred == 0 && ferror(stream) ? -1 : (int)transferred;
}

ssize_t read(int descriptor, void *buffer, size_t count)
{
    return (ssize_t)_read(descriptor, buffer,
        count > 0x7fffffffU ? 0x7fffffffU : (unsigned int)count);
}

ssize_t write(int descriptor, const void *buffer, size_t count)
{
    return (ssize_t)_write(descriptor, buffer,
        count > 0x7fffffffU ? 0x7fffffffU : (unsigned int)count);
}

static long long wcrt_descriptor_seek(int descriptor, long long offset,
    int origin)
{
    FILE *stream = wcrt_descriptor_stream(descriptor);
    long long position;
    if (stream == NULL) {
        errno = EBADF;
        return -1;
    }
    if (__wcrt_file_seek(stream, offset, origin, &position) != 0) return -1;
    stream->end_of_file = 0;
    stream->pushback = EOF;
    __wcrt_reset_stream_conversion(stream);
    return position;
}

long _lseek(int descriptor, long offset, int origin)
{
    long long result = wcrt_descriptor_seek(descriptor, offset, origin);
    if (result > 2147483647LL || result < -2147483647LL - 1LL) {
        errno = EFBIG;
        return -1;
    }
    return (long)result;
}

long _tell(int descriptor) { return _lseek(descriptor, 0, SEEK_CUR); }

off_t lseek(int descriptor, off_t offset, int origin)
{
    return (off_t)wcrt_descriptor_seek(descriptor, offset, origin);
}

int _commit(int descriptor)
{
    FILE *stream = wcrt_descriptor_stream(descriptor);
    if (stream == NULL) {
        errno = EBADF;
        return -1;
    }
    return __wcrt_file_flush(stream);
}

int fsync(int descriptor) { return _commit(descriptor); }

int _isatty(int descriptor)
{
    FILE *stream = wcrt_descriptor_stream(descriptor);
    if (stream == NULL) {
        errno = EBADF;
        return 0;
    }
    return __wcrt_file_is_terminal(stream);
}

int isatty(int descriptor) { return _isatty(descriptor); }

FILE *_fdopen(int descriptor, const char *mode)
{
    FILE *stream = wcrt_descriptor_stream(descriptor);
    if (stream == NULL) errno = EBADF;
    else if (mode == NULL || *mode == '\0' ||
        ((*mode == 'r') && (stream->flags & WCRT_FILE_READ) == 0) ||
        ((*mode == 'w' || *mode == 'a') &&
        (stream->flags & WCRT_FILE_WRITE) == 0) ||
        (strchr(mode, '+') != NULL &&
        (stream->flags & (WCRT_FILE_READ | WCRT_FILE_WRITE)) !=
        (WCRT_FILE_READ | WCRT_FILE_WRITE))) {
        errno = EINVAL;
        return NULL;
    }
    if (*mode != 'r' && *mode != 'w' && *mode != 'a') {
        errno = EINVAL;
        return NULL;
    }
    if (*mode == 'a') stream->flags |= WCRT_FILE_APPEND;
    if (strchr(mode, 'b') != NULL) stream->flags |= WCRT_FILE_BINARY;
    return stream;
}

FILE *fdopen(int descriptor, const char *mode)
{
    return _fdopen(descriptor, mode);
}

int _dup2(int descriptor, int target)
{
    FILE *source = wcrt_descriptor_stream(descriptor);
    FILE *destination = wcrt_descriptor_slot(target);
    if (source == NULL || destination == NULL) {
        errno = EBADF;
        return -1;
    }
    if (descriptor == target) return 0;
    if (__wcrt_file_duplicate(source, destination) != 0) return -1;
    destination->descriptor = target;
    return 0;
}

int _dup(int descriptor)
{
    FILE *destination;
    int target;
    if (wcrt_descriptor_stream(descriptor) == NULL) {
        errno = EBADF;
        return -1;
    }
    destination = wcrt_allocate_stream();
    if (destination == NULL) {
        errno = EMFILE;
        return -1;
    }
    target = (int)(destination - wcrt_streams) + 3;
    return _dup2(descriptor, target) == 0 ? target : -1;
}

int dup(int descriptor) { return _dup(descriptor); }

int dup2(int descriptor, int target)
{
    return _dup2(descriptor, target) == 0 ? target : -1;
}

int _pipe(int descriptors[2], unsigned int size, int text_mode)
{
    FILE *reader = NULL;
    FILE *writer = NULL;
    int index;
    if (descriptors == NULL ||
        (text_mode & ~(_O_TEXT | _O_BINARY)) != 0 ||
        ((text_mode & _O_TEXT) && (text_mode & _O_BINARY))) {
        errno = EINVAL;
        return -1;
    }
    for (index = 0; index < FOPEN_MAX; ++index) {
        if (wcrt_streams[index].handle != NULL) continue;
        if (reader == NULL) reader = &wcrt_streams[index];
        else {
            writer = &wcrt_streams[index];
            break;
        }
    }
    if (writer == NULL) {
        errno = EMFILE;
        return -1;
    }
    if (__wcrt_file_create_pipe(reader, writer, size,
        (text_mode & _O_BINARY) != 0) != 0) return -1;
    reader->descriptor = (int)(reader - wcrt_streams) + 3;
    writer->descriptor = (int)(writer - wcrt_streams) + 3;
    descriptors[0] = reader->descriptor;
    descriptors[1] = writer->descriptor;
    return 0;
}

int pipe(int descriptors[2])
{
    return _pipe(descriptors, 0, _O_BINARY);
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
    __wcrt_prepare_stream(stream);
    if (__wcrt_require_orientation(stream, WCRT_ORIENTATION_BYTE) != 0) {
        return EOF;
    }
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
    if (__wcrt_require_orientation(stream, WCRT_ORIENTATION_BYTE) != 0) {
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
    __wcrt_prepare_stream(stream);
    if (__wcrt_require_orientation(stream, WCRT_ORIENTATION_BYTE) != 0) {
        return EOF;
    }
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
    __wcrt_prepare_stream(stream);
    if (position == NULL || __wcrt_file_seek(stream, 0, SEEK_CUR,
        &current) != 0) {
        return -1;
    }
    *position = current - (stream->pushback != EOF ||
        stream->has_wide_pushback ? 1 : 0);
    return 0;
}

int fseek(FILE *stream, long offset, int origin)
{
    __wcrt_prepare_stream(stream);
    if (__wcrt_file_seek(stream, offset, origin, NULL) != 0) {
        stream->error = 1;
        return -1;
    }
    stream->end_of_file = 0;
    stream->pushback = EOF;
    __wcrt_reset_stream_conversion(stream);
    return 0;
}

int fsetpos(FILE *stream, const fpos_t *position)
{
    __wcrt_prepare_stream(stream);
    if (position == NULL || __wcrt_file_seek(stream, *position, SEEK_SET,
        NULL) != 0) {
        return -1;
    }
    stream->end_of_file = 0;
    stream->pushback = EOF;
    __wcrt_reset_stream_conversion(stream);
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
