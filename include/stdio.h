/**
 * @file stdio.h
 * @brief Declares C89 input and output facilities.
 */

#ifndef WCRT_STDIO_H
#define WCRT_STDIO_H

#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <wcrt/restrict.h>

typedef struct wcrt_file FILE;
typedef long long fpos_t;

#define EOF (-1)
#define FOPEN_MAX 32
#define FILENAME_MAX 260
#define BUFSIZ 512
#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define TMP_MAX 32767
#define L_tmpnam 260

extern FILE __wcrt_stdin;
extern FILE __wcrt_stdout;
extern FILE __wcrt_stderr;
#define stdin (&__wcrt_stdin)
#define stdout (&__wcrt_stdout)
#define stderr (&__wcrt_stderr)

#ifdef __cplusplus
extern "C" {
#endif

int remove(const char *path);
int rename(const char *old_path, const char *new_path);
FILE *tmpfile(void);
char *tmpnam(char *name);
int fclose(FILE *stream);
int fflush(FILE *stream);
FILE *fopen(const char *WCRT_RESTRICT path,
    const char *WCRT_RESTRICT mode);
/** @brief Opens a file through the Microsoft secure-CRT contract. */
errno_t fopen_s(FILE **stream, const char *path, const char *mode);
FILE *freopen(const char *WCRT_RESTRICT path,
    const char *WCRT_RESTRICT mode, FILE *WCRT_RESTRICT stream);
void setbuf(FILE *WCRT_RESTRICT stream, char *WCRT_RESTRICT buffer);
int setvbuf(FILE *WCRT_RESTRICT stream, char *WCRT_RESTRICT buffer,
    int mode, size_t size);
int fprintf(FILE *WCRT_RESTRICT stream,
    const char *WCRT_RESTRICT format, ...);
int fscanf(FILE *WCRT_RESTRICT stream,
    const char *WCRT_RESTRICT format, ...);
int printf(const char *WCRT_RESTRICT format, ...);
int scanf(const char *WCRT_RESTRICT format, ...);
int sprintf(char *WCRT_RESTRICT destination,
    const char *WCRT_RESTRICT format, ...);
int sscanf(const char *WCRT_RESTRICT source,
    const char *WCRT_RESTRICT format, ...);
/** @brief Scans a string with Microsoft destination-size arguments. */
int sscanf_s(const char *source, const char *format, ...);
int vfprintf(FILE *WCRT_RESTRICT stream,
    const char *WCRT_RESTRICT format, va_list arguments);
int vprintf(const char *WCRT_RESTRICT format, va_list arguments);
int vsprintf(char *WCRT_RESTRICT destination,
    const char *WCRT_RESTRICT format, va_list arguments);
#if !defined(WCRT_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
int vfscanf(FILE *WCRT_RESTRICT stream,
    const char *WCRT_RESTRICT format, va_list arguments);
int vscanf(const char *WCRT_RESTRICT format, va_list arguments);
int vsscanf(const char *WCRT_RESTRICT source,
    const char *WCRT_RESTRICT format, va_list arguments);
int snprintf(char *WCRT_RESTRICT destination, size_t size,
    const char *WCRT_RESTRICT format, ...);
int vsnprintf(char *WCRT_RESTRICT destination, size_t size,
    const char *WCRT_RESTRICT format, va_list arguments);
#endif
int _snprintf(char *destination, size_t size, const char *format, ...);
int _vsnprintf(char *destination, size_t size, const char *format,
    va_list arguments);
/** @brief Returns the descriptor associated with an open stream. */
int _fileno(FILE *stream);
int fgetc(FILE *stream);
char *fgets(char *WCRT_RESTRICT destination, int count,
    FILE *WCRT_RESTRICT stream);
int fputc(int character, FILE *stream);
int fputs(const char *WCRT_RESTRICT string, FILE *WCRT_RESTRICT stream);
int getc(FILE *stream);
int getchar(void);
char *gets(char *destination);
int putc(int character, FILE *stream);
int putchar(int character);
int puts(const char *string);
int ungetc(int character, FILE *stream);
size_t fread(void *WCRT_RESTRICT destination, size_t size, size_t count,
    FILE *WCRT_RESTRICT stream);
size_t fwrite(const void *WCRT_RESTRICT source, size_t size, size_t count,
    FILE *WCRT_RESTRICT stream);
int fgetpos(FILE *WCRT_RESTRICT stream, fpos_t *WCRT_RESTRICT position);
int fseek(FILE *stream, long offset, int origin);
int fsetpos(FILE *WCRT_RESTRICT stream,
    const fpos_t *WCRT_RESTRICT position);
long ftell(FILE *stream);
void rewind(FILE *stream);
void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *prefix);

#ifdef __cplusplus
}
#endif

#endif
