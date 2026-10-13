/** @file byte-swap.c @brief Verifies guarded byte-pair copying. */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

/** @brief Exercises every byte value and complete-pair boundary. */
int check_byte_swap(void)
{
    void (__cdecl *swap)(char *, char *, int) = _swab;
    char source[260], original[260], destination[260], expected[260];
    int count, index;
    for (index = 0; index < 260; ++index) source[index] = (char)index;
    memcpy(original, source, sizeof(source));
    for (count = 0; count <= 257; ++count) {
        memset(destination, 0x7c, sizeof(destination));
        memcpy(expected, destination, sizeof(expected));
        for (index = 0; index < count - 1; index += 2) {
            expected[index + 1] = source[index + 2];
            expected[index + 2] = source[index + 1];
        }
        errno = 123;
        swap(source + 1, destination + 1, count);
        if (errno != 123 || memcmp(destination, expected, sizeof(expected)) ||
            memcmp(source, original, sizeof(source))) return 1;
        memcpy(destination, source, sizeof(source));
        memcpy(expected, destination, sizeof(expected));
        for (index = 0; index < count - 1; index += 2) {
            expected[index + 1] = source[index + 2];
            expected[index + 2] = source[index + 1];
        }
        swap(destination + 1, destination + 1, count);
        if (errno != 123 || memcmp(destination, expected, sizeof(expected)))
            return 2;
    }
    memcpy(expected, destination, sizeof(expected));
    swap(source, destination, -1);
    if (errno != EINVAL || memcmp(destination, expected, sizeof(expected)))
        return 3;
    errno = 123;
    swap(source, destination, INT_MIN);
    if (errno != EINVAL || memcmp(destination, expected, sizeof(expected)))
        return 4;
    errno = 123;
    swap(NULL, destination, 0);
    if (errno != EINVAL || memcmp(destination, expected, sizeof(expected)))
        return 5;
    errno = 123;
    swap(source, NULL, 0);
    if (errno != EINVAL || memcmp(source, original, sizeof(source))) return 6;
    errno = 123;
    swap(NULL, NULL, INT_MIN);
    if (errno != EINVAL) return 7;
    return 0;
}
