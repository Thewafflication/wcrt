/**
 * @file string.c
 * @brief Implements C89 byte and string handling in the C locale.
 */

#include <errno.h>
#include <string.h>

/** @brief Saved continuation used by the non-reentrant strtok function. */
static char *wcrt_token_next;

void *memcpy(void *destination, const void *source, size_t count)
{
    unsigned char *output = (unsigned char *)destination;
    const unsigned char *input = (const unsigned char *)source;
    size_t index;

    for (index = 0; index < count; ++index) {
        output[index] = input[index];
    }
    return destination;
}

void *memmove(void *destination, const void *source, size_t count)
{
    unsigned char *output = (unsigned char *)destination;
    const unsigned char *input = (const unsigned char *)source;

    if (output < input) {
        size_t index;
        for (index = 0; index < count; ++index) {
            output[index] = input[index];
        }
    } else if (output > input) {
        while (count != 0) {
            --count;
            output[count] = input[count];
        }
    }
    return destination;
}

char *strcpy(char *destination, const char *source)
{
    char *result = destination;
    while ((*destination++ = *source++) != '\0') {
    }
    return result;
}

char *strncpy(char *destination, const char *source, size_t count)
{
    char *result = destination;
    while (count != 0 && *source != '\0') {
        *destination++ = *source++;
        --count;
    }
    while (count-- != 0) {
        *destination++ = '\0';
    }
    return result;
}

char *strcat(char *destination, const char *source)
{
    char *result = destination;
    while (*destination != '\0') {
        ++destination;
    }
    strcpy(destination, source);
    return result;
}

char *strncat(char *destination, const char *source, size_t count)
{
    char *result = destination;
    while (*destination != '\0') {
        ++destination;
    }
    while (count-- != 0 && *source != '\0') {
        *destination++ = *source++;
    }
    *destination = '\0';
    return result;
}

int memcmp(const void *left, const void *right, size_t count)
{
    const unsigned char *first = (const unsigned char *)left;
    const unsigned char *second = (const unsigned char *)right;
    size_t index;

    for (index = 0; index < count; ++index) {
        if (first[index] != second[index]) {
            return first[index] < second[index] ? -1 : 1;
        }
    }
    return 0;
}

int strcmp(const char *left, const char *right)
{
    while (*left != '\0' && *left == *right) {
        ++left;
        ++right;
    }
    return *(const unsigned char *)left < *(const unsigned char *)right ? -1 :
        *(const unsigned char *)left != *(const unsigned char *)right;
}

int strcoll(const char *left, const char *right)
{
    return strcmp(left, right);
}

int strncmp(const char *left, const char *right, size_t count)
{
    while (count != 0 && *left != '\0' && *left == *right) {
        ++left;
        ++right;
        --count;
    }
    if (count == 0) {
        return 0;
    }
    return *(const unsigned char *)left < *(const unsigned char *)right ? -1 :
        *(const unsigned char *)left != *(const unsigned char *)right;
}

size_t strxfrm(char *destination, const char *source, size_t count)
{
    size_t length = strlen(source);
    size_t index;

    for (index = 0; index < count && index < length; ++index) {
        destination[index] = source[index];
    }
    if (index < count) {
        destination[index] = '\0';
    }
    return length;
}

void *memchr(const void *memory, int character, size_t count)
{
    const unsigned char *bytes = (const unsigned char *)memory;
    unsigned char wanted = (unsigned char)character;
    size_t index;

    for (index = 0; index < count; ++index) {
        if (bytes[index] == wanted) {
            return (void *)(bytes + index);
        }
    }
    return NULL;
}

char *strchr(const char *string, int character)
{
    char wanted = (char)character;
    do {
        if (*string == wanted) {
            return (char *)string;
        }
    } while (*string++ != '\0');
    return NULL;
}

size_t strcspn(const char *string, const char *reject)
{
    size_t length = 0;
    while (string[length] != '\0' &&
        strchr(reject, string[length]) == NULL) {
        ++length;
    }
    return length;
}

char *strpbrk(const char *string, const char *accept)
{
    while (*string != '\0') {
        if (strchr(accept, *string) != NULL) {
            return (char *)string;
        }
        ++string;
    }
    return NULL;
}

char *strrchr(const char *string, int character)
{
    const char *result = NULL;
    char wanted = (char)character;
    do {
        if (*string == wanted) {
            result = string;
        }
    } while (*string++ != '\0');
    return (char *)result;
}

size_t strspn(const char *string, const char *accept)
{
    size_t length = 0;
    while (string[length] != '\0' &&
        strchr(accept, string[length]) != NULL) {
        ++length;
    }
    return length;
}

char *strstr(const char *haystack, const char *needle)
{
    size_t length = strlen(needle);
    do {
        if (strncmp(haystack, needle, length) == 0) {
            return (char *)haystack;
        }
    } while (*haystack++ != '\0');
    return NULL;
}

char *strtok(char *string, const char *delimiters)
{
    char *token;
    if (string != NULL) {
        wcrt_token_next = string;
    }
    if (wcrt_token_next == NULL) {
        return NULL;
    }
    wcrt_token_next += strspn(wcrt_token_next, delimiters);
    if (*wcrt_token_next == '\0') {
        wcrt_token_next = NULL;
        return NULL;
    }
    token = wcrt_token_next;
    wcrt_token_next += strcspn(wcrt_token_next, delimiters);
    if (*wcrt_token_next != '\0') {
        *wcrt_token_next++ = '\0';
    } else {
        wcrt_token_next = NULL;
    }
    return token;
}

void *memset(void *memory, int character, size_t count)
{
    unsigned char *bytes = (unsigned char *)memory;
    size_t index;
    for (index = 0; index < count; ++index) {
        bytes[index] = (unsigned char)character;
    }
    return memory;
}

char *strerror(int error_number)
{
    static char unknown[] = "Unknown error";
    static char domain[] = "Domain error";
    static char range[] = "Range error";

    if (error_number == EDOM) {
        return domain;
    }
    if (error_number == ERANGE) {
        return range;
    }
    return unknown;
}

size_t strlen(const char *string)
{
    const char *end = string;
    while (*end != '\0') {
        ++end;
    }
    return (size_t)(end - string);
}
