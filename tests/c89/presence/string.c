/* REQ-0014: <string.h> presence. Compile only; do not link. */
#include <string.h>

#ifndef NULL
#error "NULL must be defined"
#endif

typedef void *(*wcrt_memcpy_fn)(void *, const void *, size_t);
typedef char *(*wcrt_strcpy_fn)(char *, const char *);
typedef char *(*wcrt_strncpy_fn)(char *, const char *, size_t);
typedef char *(*wcrt_strcat_fn)(char *, const char *);
typedef char *(*wcrt_strncat_fn)(char *, const char *, size_t);
typedef int (*wcrt_memcmp_fn)(const void *, const void *, size_t);
typedef int (*wcrt_strcmp_fn)(const char *, const char *);
typedef int (*wcrt_strncmp_fn)(const char *, const char *, size_t);
typedef size_t (*wcrt_strxfrm_fn)(char *, const char *, size_t);
typedef void *(*wcrt_memchr_fn)(const void *, int, size_t);
typedef char *(*wcrt_strchr_fn)(const char *, int);
typedef size_t (*wcrt_strset_fn)(const char *, const char *);
typedef char *(*wcrt_strpair_fn)(const char *, const char *);
typedef char *(*wcrt_strrchr_fn)(const char *, int);
typedef char *(*wcrt_strtok_fn)(char *, const char *);
typedef void *(*wcrt_memset_fn)(void *, int, size_t);
typedef char *(*wcrt_strerror_fn)(int);
typedef size_t (*wcrt_strlen_fn)(const char *);

static wcrt_memcpy_fn wcrt_memory_copy_functions[] = { (memcpy), (memmove) };
static wcrt_strcpy_fn wcrt_strcpy_address = (strcpy);
static wcrt_strncpy_fn wcrt_strncpy_address = (strncpy);
static wcrt_strcat_fn wcrt_strcat_address = (strcat);
static wcrt_strncat_fn wcrt_strncat_address = (strncat);
static wcrt_memcmp_fn wcrt_memcmp_address = (memcmp);
static wcrt_strcmp_fn wcrt_string_compare_functions[] = {
    (strcmp), (strcoll)
};
static wcrt_strncmp_fn wcrt_strncmp_address = (strncmp);
static wcrt_strxfrm_fn wcrt_strxfrm_address = (strxfrm);
static wcrt_memchr_fn wcrt_memchr_address = (memchr);
static wcrt_strchr_fn wcrt_strchr_address = (strchr);
static wcrt_strset_fn wcrt_string_set_functions[] = {
    (strcspn), (strspn)
};
static wcrt_strpair_fn wcrt_string_pair_functions[] = {
    (strpbrk), (strstr)
};
static wcrt_strrchr_fn wcrt_strrchr_address = (strrchr);
static wcrt_strtok_fn wcrt_strtok_address = (strtok);
static wcrt_memset_fn wcrt_memset_address = (memset);
static wcrt_strerror_fn wcrt_strerror_address = (strerror);
static wcrt_strlen_fn wcrt_strlen_address = (strlen);
static size_t wcrt_string_size;
