/* REQ-0012: <stdio.h> presence. Compile only; do not link. */
#include <stdio.h>

#ifndef NULL
#error "NULL must be defined"
#endif
#ifndef EOF
#error "EOF must be defined"
#endif
#ifndef FOPEN_MAX
#error "FOPEN_MAX must be defined"
#endif
#ifndef FILENAME_MAX
#error "FILENAME_MAX must be defined"
#endif
#ifndef BUFSIZ
#error "BUFSIZ must be defined"
#endif
#ifndef _IOFBF
#error "_IOFBF must be defined"
#endif
#ifndef _IOLBF
#error "_IOLBF must be defined"
#endif
#ifndef _IONBF
#error "_IONBF must be defined"
#endif
#ifndef SEEK_SET
#error "SEEK_SET must be defined"
#endif
#ifndef SEEK_CUR
#error "SEEK_CUR must be defined"
#endif
#ifndef SEEK_END
#error "SEEK_END must be defined"
#endif
#ifndef TMP_MAX
#error "TMP_MAX must be defined"
#endif
#ifndef L_tmpnam
#error "L_tmpnam must be defined"
#endif
#ifndef stdin
#error "stdin must be defined"
#endif
#ifndef stdout
#error "stdout must be defined"
#endif
#ifndef stderr
#error "stderr must be defined"
#endif

typedef int (*wcrt_path_fn)(const char *);
typedef int (*wcrt_rename_fn)(const char *, const char *);
typedef FILE *(*wcrt_tmpfile_fn)(void);
typedef char *(*wcrt_tmpnam_fn)(char *);
typedef int (*wcrt_stream_int_fn)(FILE *);
typedef FILE *(*wcrt_fopen_fn)(const char *, const char *);
typedef FILE *(*wcrt_freopen_fn)(const char *, const char *, FILE *);
typedef void (*wcrt_setbuf_fn)(FILE *, char *);
typedef int (*wcrt_setvbuf_fn)(FILE *, char *, int, size_t);
typedef int (*wcrt_fprintf_fn)(FILE *, const char *, ...);
typedef int (*wcrt_fscanf_fn)(FILE *, const char *, ...);
typedef int (*wcrt_printf_fn)(const char *, ...);
typedef int (*wcrt_scanf_fn)(const char *, ...);
typedef int (*wcrt_sprintf_fn)(char *, const char *, ...);
typedef int (*wcrt_sscanf_fn)(const char *, const char *, ...);
typedef int (*wcrt_vfprintf_fn)(FILE *, const char *, va_list);
typedef int (*wcrt_vprintf_fn)(const char *, va_list);
typedef int (*wcrt_vsprintf_fn)(char *, const char *, va_list);
typedef int (*wcrt_fgetc_fn)(FILE *);
typedef char *(*wcrt_fgets_fn)(char *, int, FILE *);
typedef int (*wcrt_fputc_fn)(int, FILE *);
typedef int (*wcrt_ungetc_fn)(int, FILE *);
typedef int (*wcrt_fputs_fn)(const char *, FILE *);
typedef int (*wcrt_getchar_fn)(void);
typedef char *(*wcrt_gets_fn)(char *);
typedef int (*wcrt_putchar_fn)(int);
typedef int (*wcrt_puts_fn)(const char *);
typedef size_t (*wcrt_fread_fn)(void *, size_t, size_t, FILE *);
typedef size_t (*wcrt_fwrite_fn)(const void *, size_t, size_t, FILE *);
typedef int (*wcrt_fgetpos_fn)(FILE *, fpos_t *);
typedef int (*wcrt_fseek_fn)(FILE *, long, int);
typedef int (*wcrt_fsetpos_fn)(FILE *, const fpos_t *);
typedef long (*wcrt_ftell_fn)(FILE *);
typedef void (*wcrt_rewind_fn)(FILE *);
typedef void (*wcrt_clearerr_fn)(FILE *);
typedef void (*wcrt_perror_fn)(const char *);

static wcrt_path_fn wcrt_remove_address = (remove);
static wcrt_rename_fn wcrt_rename_address = (rename);
static wcrt_tmpfile_fn wcrt_tmpfile_address = (tmpfile);
static wcrt_tmpnam_fn wcrt_tmpnam_address = (tmpnam);
static wcrt_stream_int_fn wcrt_stream_int_functions[] = {
    (fclose), (fflush), (feof), (ferror)
};
static wcrt_fopen_fn wcrt_fopen_address = (fopen);
static wcrt_freopen_fn wcrt_freopen_address = (freopen);
static wcrt_setbuf_fn wcrt_setbuf_address = (setbuf);
static wcrt_setvbuf_fn wcrt_setvbuf_address = (setvbuf);
static wcrt_fprintf_fn wcrt_fprintf_address = (fprintf);
static wcrt_fscanf_fn wcrt_fscanf_address = (fscanf);
static wcrt_printf_fn wcrt_printf_address = (printf);
static wcrt_scanf_fn wcrt_scanf_address = (scanf);
static wcrt_sprintf_fn wcrt_sprintf_address = (sprintf);
static wcrt_sscanf_fn wcrt_sscanf_address = (sscanf);
static wcrt_vfprintf_fn wcrt_vfprintf_address = (vfprintf);
static wcrt_vprintf_fn wcrt_vprintf_address = (vprintf);
static wcrt_vsprintf_fn wcrt_vsprintf_address = (vsprintf);
static wcrt_fgetc_fn wcrt_fgetc_functions[] = {
    (fgetc), (getc)
};
static wcrt_ungetc_fn wcrt_ungetc_address = (ungetc);
static wcrt_fgets_fn wcrt_fgets_address = (fgets);
static wcrt_fputc_fn wcrt_fputc_functions[] = { (fputc), (putc) };
static wcrt_fputs_fn wcrt_fputs_address = (fputs);
static wcrt_getchar_fn wcrt_getchar_address = (getchar);
static wcrt_gets_fn wcrt_gets_address = (gets);
static wcrt_putchar_fn wcrt_putchar_address = (putchar);
static wcrt_puts_fn wcrt_puts_address = (puts);
static wcrt_fread_fn wcrt_fread_address = (fread);
static wcrt_fwrite_fn wcrt_fwrite_address = (fwrite);
static wcrt_fgetpos_fn wcrt_fgetpos_address = (fgetpos);
static wcrt_fseek_fn wcrt_fseek_address = (fseek);
static wcrt_fsetpos_fn wcrt_fsetpos_address = (fsetpos);
static wcrt_ftell_fn wcrt_ftell_address = (ftell);
static wcrt_rewind_fn wcrt_rewind_address = (rewind);
static wcrt_clearerr_fn wcrt_clearerr_address = (clearerr);
static wcrt_perror_fn wcrt_perror_address = (perror);

static fpos_t wcrt_file_position;
static size_t wcrt_io_size;

static void wcrt_use_stdio_macros_and_streams(void)
{
    FILE *streams[] = { stdin, stdout, stderr };
    int values[] = {
        EOF, FOPEN_MAX, FILENAME_MAX, BUFSIZ, _IOFBF, _IOLBF, _IONBF,
        SEEK_SET, SEEK_CUR, SEEK_END, TMP_MAX, L_tmpnam
    };
    (void)streams;
    (void)values;
}
