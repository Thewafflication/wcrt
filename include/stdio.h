/**
 * @file stdio.h
 * @brief Declares C89 input and output facilities.
 */

#ifndef WCRT_STDIO_H
#define WCRT_STDIO_H

#include <stddef.h>
#include <stdarg.h>

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
FILE *fopen(const char *path, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);
void setbuf(FILE *stream, char *buffer);
int setvbuf(FILE *stream, char *buffer, int mode, size_t size);
int fprintf(FILE *stream, const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
int printf(const char *format, ...);
int scanf(const char *format, ...);
int sprintf(char *destination, const char *format, ...);
int sscanf(const char *source, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list arguments);
int vprintf(const char *format, va_list arguments);
int vsprintf(char *destination, const char *format, va_list arguments);
int fgetc(FILE *stream);
char *fgets(char *destination, int count, FILE *stream);
int fputc(int character, FILE *stream);
int fputs(const char *string, FILE *stream);
int getc(FILE *stream);
int getchar(void);
char *gets(char *destination);
int putc(int character, FILE *stream);
int putchar(int character);
int puts(const char *string);
int ungetc(int character, FILE *stream);
size_t fread(void *destination, size_t size, size_t count, FILE *stream);
size_t fwrite(const void *source, size_t size, size_t count, FILE *stream);
int fgetpos(FILE *stream, fpos_t *position);
int fseek(FILE *stream, long offset, int origin);
int fsetpos(FILE *stream, const fpos_t *position);
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
