/**
 * @file string.h
 * @brief Declares C89 byte and string handling facilities.
 */

#ifndef WCRT_STRING_H
#define WCRT_STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void *destination, const void *source, size_t count);
void *memmove(void *destination, const void *source, size_t count);
char *strcpy(char *destination, const char *source);
char *strncpy(char *destination, const char *source, size_t count);
char *strcat(char *destination, const char *source);
char *strncat(char *destination, const char *source, size_t count);
int memcmp(const void *left, const void *right, size_t count);
int strcmp(const char *left, const char *right);
int strcoll(const char *left, const char *right);
int strncmp(const char *left, const char *right, size_t count);
size_t strxfrm(char *destination, const char *source, size_t count);
void *memchr(const void *memory, int character, size_t count);
char *strchr(const char *string, int character);
size_t strcspn(const char *string, const char *reject);
char *strpbrk(const char *string, const char *accept);
char *strrchr(const char *string, int character);
size_t strspn(const char *string, const char *accept);
char *strstr(const char *haystack, const char *needle);
char *strtok(char *string, const char *delimiters);
void *memset(void *memory, int character, size_t count);
char *strerror(int error_number);
size_t strlen(const char *string);

#ifdef __cplusplus
}
#endif

#endif
