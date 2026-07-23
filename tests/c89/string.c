/**
 * @file string.c
 * @brief Exercises every C89 byte and string handling function.
 */

#include <errno.h>
#include <string.h>

/**
 * @brief Runs copy, comparison, search, locale, token, and error tests.
 * @param argument_count Number of command-line arguments.
 * @param arguments Extra argument requests WCRT-specific message checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(int argument_count, char **arguments)
{
    unsigned char bytes[8];
    unsigned char high_left[] = {0x80, 0};
    unsigned char high_right[] = {0x7f, 0};
    char text[32];
    char padded[6];
    char transformed[8];
    char tokens[] = ",one;;two,";
    char *token;
    size_t index;

    (void)arguments;

    memset(bytes, 0xaa, sizeof(bytes));
    if (bytes[0] != 0xaa || bytes[7] != 0xaa) {
        return 1;
    }
    memcpy(bytes + 1, "abc", 3);
    if (memcmp(bytes + 1, "abc", 3) != 0 || memcmp(bytes, bytes, 0) != 0) {
        return 2;
    }
    memcpy(bytes, "abcdef", 7);
    memmove(bytes + 1, bytes, 6);
    if (memcmp(bytes, "aabcdef", 7) != 0) {
        return 3;
    }
    memmove(bytes, bytes + 1, 6);
    if (memcmp(bytes, "abcdef", 6) != 0) {
        return 4;
    }
    strcpy(text, "ab");
    strcat(text, "cd");
    strncat(text, "efgh", 2);
    if (strcmp(text, "abcdef") != 0) {
        return 5;
    }
    memset(padded, 'x', sizeof(padded));
    strncpy(padded, "ab", sizeof(padded));
    for (index = 2; index < sizeof(padded); ++index) {
        if (padded[index] != '\0') {
            return 6;
        }
    }
    if (memcmp(high_left, high_right, 1) <= 0 ||
        strcmp((char *)high_left, (char *)high_right) <= 0) {
        return 7;
    }
    if (strncmp("abc", "abd", 2) != 0 || strcoll("a", "b") >= 0) {
        return 8;
    }
    if (strxfrm(transformed, "hello", sizeof(transformed)) != 5 ||
        strcmp(transformed, "hello") != 0) {
        return 9;
    }
    if (memchr("a\0b", 0, 3) == NULL || strchr("abc", 'b') == NULL ||
        strchr("abc", '\0') == NULL || strrchr("abca", 'a') == NULL) {
        return 10;
    }
    if (strcspn("abcde", "dx") != 3 || strspn("aabbc", "ab") != 4) {
        return 11;
    }
    if (strpbrk("abc", "xz") != NULL || strstr("abcabc", "cab") == NULL ||
        strstr("abc", "") == NULL) {
        return 12;
    }
    token = strtok(tokens, ",;");
    if (token == NULL || strcmp(token, "one") != 0) {
        return 13;
    }
    token = strtok(NULL, ",;");
    if (token == NULL || strcmp(token, "two") != 0 ||
        strtok(NULL, ",;") != NULL) {
        return 14;
    }
    if (strlen(strerror(EDOM)) == 0 || strlen(strerror(ERANGE)) == 0 ||
        strlen(strerror(999)) == 0) {
        return 15;
    }
    if (argument_count > 1 &&
        (strcmp(strerror(EDOM), "Domain error") != 0 ||
        strcmp(strerror(ERANGE), "Range error") != 0)) {
        return 17;
    }
    return strlen("sixsix") == 6 ? 0 : 16;
}
