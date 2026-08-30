/**
 * @file string.h
 * @brief Declares C89 byte and string handling facilities.
 */

#ifndef WCRT_STRING_H
#define WCRT_STRING_H

#include <stddef.h>
#include <errno.h>
#include <wcrt/restrict.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Microsoft secure-CRT object-size type. */
typedef size_t rsize_t;

/** @brief Requests truncation with guaranteed termination. */
#ifndef _TRUNCATE
#define _TRUNCATE ((rsize_t)-1)
#endif

/** @brief Reports successful truncation by a secure string function. */
#ifndef STRUNCATE
#define STRUNCATE 80
#endif

/** @brief Return value for invalid Microsoft comparison parameters. */
#define _NLSCMPERROR 2147483647

void *memcpy(void *WCRT_RESTRICT destination,
    const void *WCRT_RESTRICT source, size_t count);
void *memmove(void *destination, const void *source, size_t count);
char *strcpy(char *WCRT_RESTRICT destination,
    const char *WCRT_RESTRICT source);
char *strncpy(char *WCRT_RESTRICT destination,
    const char *WCRT_RESTRICT source, size_t count);
char *strcat(char *WCRT_RESTRICT destination,
    const char *WCRT_RESTRICT source);
char *strncat(char *WCRT_RESTRICT destination,
    const char *WCRT_RESTRICT source, size_t count);
int memcmp(const void *left, const void *right, size_t count);
int strcmp(const char *left, const char *right);
int strcoll(const char *left, const char *right);
int strncmp(const char *left, const char *right, size_t count);
size_t strxfrm(char *WCRT_RESTRICT destination,
    const char *WCRT_RESTRICT source, size_t count);
void *memchr(const void *memory, int character, size_t count);
char *strchr(const char *string, int character);
size_t strcspn(const char *string, const char *reject);
char *strpbrk(const char *string, const char *accept);
char *strrchr(const char *string, int character);
size_t strspn(const char *string, const char *accept);
char *strstr(const char *haystack, const char *needle);
char *strtok(char *WCRT_RESTRICT string,
    const char *WCRT_RESTRICT delimiters);
void *memset(void *memory, int character, size_t count);
char *strerror(int error_number);
size_t strlen(const char *string);
/** @brief Compares two strings without regard to case. */
int _stricmp(const char *left, const char *right);
/** @brief Compares bounded strings without regard to case. */
int _strnicmp(const char *left, const char *right, size_t count);
/** @brief Copies a string under the Microsoft secure-CRT contract. */
errno_t strcpy_s(char *destination, rsize_t destination_size,
    const char *source);
/** @brief Copies a bounded string under the secure-CRT contract. */
errno_t strncpy_s(char *destination, rsize_t destination_size,
    const char *source, rsize_t count);
/** @brief Appends a string under the Microsoft secure-CRT contract. */
errno_t strcat_s(char *destination, rsize_t destination_size,
    const char *source);

#ifdef __cplusplus
}
#endif

#endif
