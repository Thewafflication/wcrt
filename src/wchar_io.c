/**
 * @file wchar_io.c
 * @brief Implements stream orientation and unformatted wide I/O.
 */

#include <errno.h>
#include <wchar.h>

#include "internal/file.h"

int fwide(FILE *stream, int mode)
{
    return __wcrt_orient_stream(stream, mode);
}

static wint_t wcrt_read_wide_byte(FILE *stream)
{
    unsigned char byte;
    size_t transferred;

    if (__wcrt_file_read(stream, &byte, 1, &transferred) != 0) {
        stream->error = 1;
        return WEOF;
    }
    if (transferred == 0) {
        stream->end_of_file = 1;
        return WEOF;
    }
    return (wint_t)byte;
}

wint_t fgetwc(FILE *stream)
{
    wint_t character;

    if (stream == (FILE *)0) return WEOF;
    __wcrt_prepare_stream(stream);
    if (__wcrt_require_orientation(stream, WCRT_ORIENTATION_WIDE) != 0) {
        return WEOF;
    }
    if (!(stream->flags & WCRT_FILE_READ)) {
        stream->error = 1;
        return WEOF;
    }
    if (stream->has_wide_pushback) {
        character = stream->wide_pushback;
        stream->has_wide_pushback = 0;
        stream->wide_pushback = WEOF;
        return character;
    }
    character = wcrt_read_wide_byte(stream);
    if (!(stream->flags & WCRT_FILE_BINARY) && character == (wint_t)'\r') {
        wint_t next = wcrt_read_wide_byte(stream);
        if (next == (wint_t)'\n') return (wint_t)'\n';
        if (next != WEOF) {
            stream->wide_pushback = next;
            stream->has_wide_pushback = 1;
        } else if (!stream->error) {
            stream->end_of_file = 0;
        }
    }
    return character;
}

wint_t getwc(FILE *stream)
{
    return fgetwc(stream);
}

wint_t getwchar(void)
{
    return fgetwc(stdin);
}

wint_t fputwc(wchar_t character, FILE *stream)
{
    unsigned char byte;
    size_t transferred;

    if (stream == (FILE *)0) return WEOF;
    __wcrt_prepare_stream(stream);
    if (__wcrt_require_orientation(stream, WCRT_ORIENTATION_WIDE) != 0) {
        return WEOF;
    }
    if (!(stream->flags & WCRT_FILE_WRITE)) {
        stream->error = 1;
        return WEOF;
    }
    if ((wint_t)character > 0xffU) {
        stream->error = 1;
        errno = EILSEQ;
        return WEOF;
    }
    byte = (unsigned char)character;
    if (!(stream->flags & WCRT_FILE_BINARY) && byte == '\n') {
        unsigned char carriage_return = '\r';
        if (__wcrt_file_write(stream, &carriage_return, 1, &transferred) != 0 ||
            transferred != 1) {
            stream->error = 1;
            return WEOF;
        }
    }
    if (__wcrt_file_write(stream, &byte, 1, &transferred) != 0 ||
        transferred != 1) {
        stream->error = 1;
        return WEOF;
    }
    return (wint_t)character;
}

wint_t putwc(wchar_t character, FILE *stream)
{
    return fputwc(character, stream);
}

wint_t putwchar(wchar_t character)
{
    return fputwc(character, stdout);
}

wchar_t *fgetws(wchar_t *destination, int capacity, FILE *stream)
{
    int length = 0;
    wint_t character = WEOF;

    if (destination == (wchar_t *)0 || stream == (FILE *)0 || capacity <= 0) {
        return (wchar_t *)0;
    }
    while (length + 1 < capacity) {
        character = fgetwc(stream);
        if (character == WEOF) break;
        destination[length++] = (wchar_t)character;
        if (character == (wint_t)'\n') break;
    }
    if (length == 0 && capacity > 1 && character == WEOF) {
        return (wchar_t *)0;
    }
    destination[length] = 0;
    return destination;
}

int fputws(const wchar_t *string, FILE *stream)
{
    while (*string != 0) {
        if (fputwc(*string++, stream) == WEOF) return -1;
    }
    return 0;
}

wint_t ungetwc(wint_t character, FILE *stream)
{
    if (stream == (FILE *)0 || character == WEOF) return WEOF;
    if (__wcrt_require_orientation(stream, WCRT_ORIENTATION_WIDE) != 0) {
        return WEOF;
    }
    if (character > 0xffU) {
        stream->error = 1;
        errno = EILSEQ;
        return WEOF;
    }
    if (stream->has_wide_pushback) return WEOF;
    stream->wide_pushback = character;
    stream->has_wide_pushback = 1;
    stream->end_of_file = 0;
    return character;
}
