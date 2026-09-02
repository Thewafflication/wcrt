/**
 * @file string.c
 * @brief Implements C89 byte and string handling in the C locale.
 */

#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
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

int _strnicmp(const char *left, const char *right, size_t count)
{
    if (count == 0) return 0;
    if (left == NULL || right == NULL) {
        errno = EINVAL;
        return _NLSCMPERROR;
    }
    while (count-- != 0) {
        int first = tolower((unsigned char)*left++);
        int second = tolower((unsigned char)*right++);
        if (first != second) {
            return first < second ? -1 : 1;
        }
        if (first == 0) return 0;
    }
    return 0;
}

int _stricmp(const char *left, const char *right)
{
    return _strnicmp(left, right, (size_t)-1);
}

int _memicmp(const void *left, const void *right, size_t count)
{
    const unsigned char *first = (const unsigned char *)left;
    const unsigned char *second = (const unsigned char *)right;

    if (count == 0) return 0;
    if (left == NULL || right == NULL) {
        errno = EINVAL;
        return _NLSCMPERROR;
    }
    while (count-- != 0) {
        int left_value = tolower(*first++);
        int right_value = tolower(*second++);
        if (left_value != right_value) {
            return left_value < right_value ? -1 : 1;
        }
    }
    return 0;
}

int strcasecmp(const char *left, const char *right)
{
    return _stricmp(left, right);
}

int strncasecmp(const char *left, const char *right, size_t count)
{
    return _strnicmp(left, right, count);
}

char *_strlwr(char *string)
{
    char *cursor;
    if (string == NULL) {
        errno = EINVAL;
        return NULL;
    }
    for (cursor = string; *cursor != '\0'; ++cursor) {
        *cursor = (char)tolower((unsigned char)*cursor);
    }
    return string;
}

char *_strupr(char *string)
{
    char *cursor;
    if (string == NULL) {
        errno = EINVAL;
        return NULL;
    }
    for (cursor = string; *cursor != '\0'; ++cursor) {
        *cursor = (char)toupper((unsigned char)*cursor);
    }
    return string;
}

char *_strrev(char *string)
{
    size_t left;
    size_t right;
    if (string == NULL) {
        errno = EINVAL;
        return NULL;
    }
    right = strlen(string);
    if (right == 0) return string;
    --right;
    for (left = 0; left < right; ++left, --right) {
        char temporary = string[left];
        string[left] = string[right];
        string[right] = temporary;
    }
    return string;
}

char *_strdup(const char *string)
{
    size_t size;
    char *duplicate;
    if (string == NULL) {
        errno = EINVAL;
        return NULL;
    }
    size = strlen(string) + 1;
    duplicate = (char *)malloc(size);
    if (duplicate == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(duplicate, string, size);
    return duplicate;
}

char *strdup(const char *string)
{
    return _strdup(string);
}

char *_strnset(char *string, int character, size_t count)
{
    char *cursor;
    if (string == NULL) {
        errno = EINVAL;
        return NULL;
    }
    cursor = string;
    while (count-- != 0 && *cursor != '\0') {
        *cursor++ = (char)character;
    }
    return string;
}

char *_strset(char *string, int character)
{
    return _strnset(string, character, (size_t)-1);
}

errno_t strcpy_s(char *destination, rsize_t destination_size,
    const char *source)
{
    size_t source_size;
    if (destination == NULL) return EINVAL;
    if (destination_size == 0) return ERANGE;
    if (source == NULL) {
        destination[0] = '\0';
        return EINVAL;
    }
    source_size = strlen(source) + 1;
    if (source_size > destination_size) {
        destination[0] = '\0';
        return ERANGE;
    }
    memcpy(destination, source, source_size);
    return 0;
}

errno_t strncpy_s(char *destination, rsize_t destination_size,
    const char *source, rsize_t count)
{
    size_t source_length;
    size_t copy_length;
    int truncate = count == _TRUNCATE;
    if (destination == NULL) return EINVAL;
    if (destination_size == 0) return ERANGE;
    if (source == NULL) {
        destination[0] = '\0';
        return EINVAL;
    }
    source_length = strlen(source);
    copy_length = truncate || count > source_length ? source_length : count;
    if (copy_length >= destination_size) {
        if (!truncate) {
            destination[0] = '\0';
            return ERANGE;
        }
        copy_length = destination_size - 1;
    }
    memcpy(destination, source, copy_length);
    destination[copy_length] = '\0';
    return truncate && copy_length < source_length ? STRUNCATE : 0;
}

errno_t strcat_s(char *destination, rsize_t destination_size,
    const char *source)
{
    size_t destination_length = 0;
    size_t source_size;

    if (destination == NULL) return EINVAL;
    if (destination_size == 0) return ERANGE;
    while (destination_length < destination_size &&
        destination[destination_length] != '\0') {
        ++destination_length;
    }
    if (destination_length == destination_size) return EINVAL;
    if (source == NULL) {
        destination[0] = '\0';
        return EINVAL;
    }
    source_size = strlen(source) + 1;
    if (source_size > destination_size - destination_length) {
        destination[0] = '\0';
        return ERANGE;
    }
    memcpy(destination + destination_length, source, source_size);
    return 0;
}
