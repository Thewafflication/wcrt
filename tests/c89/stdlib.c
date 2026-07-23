/**
 * @file stdlib.c
 * @brief Exercises C89 conversions, allocation, algorithms, and multibyte APIs.
 */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/** @brief Integer comparator used by qsort and bsearch. */
static int compare_int(const void *left, const void *right)
{
    int first = *(const int *)left;
    int second = *(const int *)right;
    return first < second ? -1 : first != second;
}

/**
 * @brief Runs non-destructive general-utility checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    char *end;
    int values[] = {7, -2, 9, 0, 7};
    int key = 7;
    unsigned char *memory;
    wchar_t wide[4];
    char narrow[4];
    div_t divided;
    ldiv_t long_divided;
    int first_random;

    if (atoi(" -42x") != -42 || atol("123") != 123L ||
        atof("1.25") != 1.25) {
        return 1;
    }
    if (strtol("0x20z", &end, 0) != 32 || *end != 'z' ||
        strtoul("077!", &end, 0) != 63 || *end != '!') {
        return 2;
    }
    errno = 0;
    if (strtol("999999999999999999999", &end, 10) != LONG_MAX ||
        errno != ERANGE) {
        return 3;
    }
    if (strtod("-12.5e2x", &end) != -1250.0 || *end != 'x') {
        return 4;
    }
    srand(123);
    first_random = rand();
    srand(123);
    if (rand() != first_random || first_random < 0 ||
        first_random > RAND_MAX) {
        return 5;
    }
    memory = (unsigned char *)calloc(8, 1);
    if (memory == NULL || memory[0] != 0 || memory[7] != 0) {
        return 6;
    }
    memory[0] = 11;
    memory = (unsigned char *)realloc(memory, 32);
    if (memory == NULL || memory[0] != 11) {
        return 7;
    }
    free(memory);
    free(NULL);
    qsort(values, 5, sizeof(int), compare_int);
    if (values[0] != -2 || values[4] != 9 ||
        bsearch(&key, values, 5, sizeof(int), compare_int) == NULL) {
        return 8;
    }
    if (bsearch(&key, values, 0, sizeof(int), compare_int) != NULL) {
        return 9;
    }
    divided = div(-7, 3);
    long_divided = ldiv(20L, 6L);
    if (abs(-4) != 4 || labs(-5L) != 5L || divided.quot != -2 ||
        divided.rem != -1 || long_divided.quot != 3 ||
        long_divided.rem != 2) {
        return 10;
    }
    if (mblen(NULL, 0) != 0 || mblen("A", 1) != 1 ||
        mbtowc(&wide[0], "A", 1) != 1 || wide[0] != (wchar_t)'A') {
        return 11;
    }
    if (wctomb(&narrow[0], (wchar_t)'B') != 1 || narrow[0] != 'B' ||
        mbstowcs(wide, "abc", 4) != 3 ||
        wcstombs(narrow, wide, 4) != 3 || strcmp(narrow, "abc") != 0) {
        return 12;
    }
    if (getenv("PATH") == NULL || getenv("") != NULL || system(NULL) == 0) {
        return 13;
    }
    return 0;
}
