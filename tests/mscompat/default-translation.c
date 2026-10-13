/** @file default-translation.c @brief Verifies default translation modes. */
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../../src/internal/file.h"

/** @brief Checks physical bytes and the selected descriptor translation. */
static int descriptor_mode(const char *path, int kind, int flags, int binary)
{
    int descriptor;
    int length = binary ? 2 : 3;
    char bytes[3];
    int base = _O_CREAT | _O_TRUNC | _O_RDWR;
    if (!kind) descriptor = _open(path, base | flags);
    else if (kind == 1) descriptor = _sopen(path, base | flags,
        _SH_DENYNO, _S_IREAD | _S_IWRITE);
    else descriptor = _creat(path, _S_IREAD | _S_IWRITE);
    if (descriptor < 0 || _write(descriptor, "A\n", 2) != 2 ||
        _filelength(descriptor) != length ||
        _setmode(descriptor, _O_BINARY) != (binary ? _O_BINARY : _O_TEXT) ||
        _lseek(descriptor, 0, SEEK_SET) != 0 ||
        _read(descriptor, bytes, 3) != length ||
        memcmp(bytes, binary ? "A\n" : "A\r\n", length) ||
        _close(descriptor) || remove(path)) return 1;
    return 0;
}
/** @brief Checks implicit/explicit stream writes, including freopen. */
static int stream_mode(const char *path, int kind, const char *mode, int binary)
{
    FILE *stream;
    if (!kind) stream = fopen(path, mode);
    else if (kind == 1) stream = _fsopen(path, mode, _SH_DENYNO);
    else {
        stream = fopen(path, "wb");
        if (stream) stream = freopen(path, mode, stream);
    }
    if (!stream || fputs("A\n", stream) ||
        _filelength(_fileno(stream)) != (binary ? 2 : 3) ||
        !!(stream->flags & WCRT_FILE_BINARY) != binary ||
        fclose(stream) || remove(path)) return 1;
    return 0;
}
/** @brief Checks read translation of explicitly written CRLF bytes. */
static int read_mode(const char *path, int shared, const char *mode, int binary)
{
    FILE *stream = fopen(path, "wt");
    char bytes[3];
    int length = binary ? 3 : 2;
    if (!stream || fputs("A\n", stream) || fclose(stream)) return 1;
    stream = shared ? _fsopen(path, mode, _SH_DENYNO) : fopen(path, mode);
    if (!stream || fread(bytes, 1, 3, stream) != (size_t)length ||
        memcmp(bytes, binary ? "A\r\n" : "A\n", length) ||
        fclose(stream) || remove(path)) return 2;
    return 0;
}
/** @brief Exercises defaults, overrides, data writes and existing modes. */
int main(int argc, char **argv)
{
    int original = _fmode;
    int index, kind, descriptor;
    unsigned int input_flags, output_flags, error_flags;
    FILE *existing;
    if (argc != 2 || original != _O_TEXT || sizeof(_fmode) != 4) return 1;
    if (descriptor_mode(argv[1], 0, 0, 0)) return 2;
    input_flags = stdin->flags;
    output_flags = stdout->flags;
    error_flags = stderr->flags;
    for (index = 0; index < 2; ++index) {
        _fmode = index ? _O_BINARY : _O_TEXT;
        for (kind = 0; kind < 3; ++kind) {
            if (descriptor_mode(argv[1], kind, 0, index)) return 3;
            if (kind < 2 && (descriptor_mode(argv[1], kind, _O_BINARY, 1) ||
                descriptor_mode(argv[1], kind, _O_TEXT, 0))) return 4;
            if (stream_mode(argv[1], kind, "w", index) ||
                stream_mode(argv[1], kind, "wb", 1) ||
                stream_mode(argv[1], kind, "wt", 0)) return 5;
        }
        for (kind = 0; kind < 2; ++kind) {
            if (read_mode(argv[1], kind, "r", index) ||
                read_mode(argv[1], kind, "rb", 1) ||
                read_mode(argv[1], kind, "rt", 0)) return 6;
        }
    }
    _fmode = _O_TEXT;
    existing = fopen(argv[1], "w");
    if (!existing) return 7;
    _fmode = _O_BINARY;
    if (fputs("A\n", existing) || _filelength(_fileno(existing)) != 3 ||
        fclose(existing) || remove(argv[1])) return 8;
    _fmode = 12345;
    if (_open(argv[1], _O_CREAT | _O_RDWR) != -1 || errno != EINVAL ||
        _sopen(argv[1], _O_CREAT | _O_RDWR, _SH_DENYNO,
            _S_IREAD | _S_IWRITE) != -1 || errno != EINVAL ||
        _creat(argv[1], _S_IWRITE) != -1 || errno != EINVAL ||
        fopen(argv[1], "w") || errno != EINVAL ||
        _fsopen(argv[1], "w", _SH_DENYNO) || errno != EINVAL ||
        _access(argv[1], 0) != -1 || errno != ENOENT) return 9;
    if (descriptor_mode(argv[1], 0, _O_BINARY, 1) ||
        descriptor_mode(argv[1], 1, _O_TEXT, 0) ||
        stream_mode(argv[1], 0, "wb", 1) ||
        stream_mode(argv[1], 1, "wt", 0)) return 10;
    existing = fopen(argv[1], "wb");
    if (!existing || fputc('Q', existing) != 'Q' ||
        freopen(argv[1], "w", existing) || errno != EINVAL) return 15;
    descriptor = _open(argv[1], _O_RDONLY | _O_BINARY);
    if (descriptor < 0 || _filelength(descriptor) != 1 ||
        _close(descriptor) || remove(argv[1])) return 16;
    descriptor = _open(argv[1], _O_CREAT | _O_RDWR | _O_BINARY);
    if (descriptor < 0) return 11;
    _fmode = _O_TEXT;
    if (_write(descriptor, "A\n", 2) != 2 || _filelength(descriptor) != 2 ||
        _close(descriptor) || remove(argv[1])) return 12;
    if (stdin->flags != input_flags || stdout->flags != output_flags ||
        stderr->flags != error_flags) return 13;
    errno = 123;
    _fmode = original;
    if (errno != 123) return 14;
    return 0;
}
