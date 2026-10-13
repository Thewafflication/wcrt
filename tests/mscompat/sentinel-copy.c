/** @file sentinel-copy.c @brief Verifies bounded sentinel copying. */
#include <memory.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <limits.h>

/** @brief Checks all bytes, stop boundaries and source preservation. */
int check_sentinel_copy(void)
{
    void *(*copy)(void *, const void *, int, size_t) = _memccpy;
    unsigned char source[258], output[260], saved[258];
    unsigned int byte, index, count;
    int sentinel;
    void *result;
    for (index = 0; index < 258; ++index)
        source[index] = (unsigned char)index;
    memcpy(saved, source, sizeof(source));
    for (byte = 0; byte < 256; ++byte) {
        for (count = 0; count <= 257; ++count) {
            memset(output, 0xa5, sizeof(output));
            errno = 123;
            result = copy(output + 1, source, (int)byte, count);
            if (result != (count > byte ? output + byte + 2 : NULL) ||
                errno != 123 || memcmp(source, saved, sizeof(source)))
                return 1;
            for (index = 0; index < sizeof(output); ++index) {
                unsigned int copied = count > byte ? byte + 1 : count;
                unsigned char expected = index && index <= copied ?
                    source[index - 1] : 0xa5;
                if (output[index] != expected) return 2;
            }
        }
        sentinel = (int)byte + 256;
        if (copy(output, source, sentinel, 256) != output + byte + 1)
            return 3;
        sentinel = (int)byte - 256;
        if (copy(output, source, sentinel, 256) != output + byte + 1)
            return 4;
    }
    memset(source, 0x7e, sizeof(source));
    memset(output, 0xa5, sizeof(output));
    errno = 124;
    if (copy(output + 1, source, 0, 258) || errno != 124 ||
        output[0] != 0xa5 || output[259] != 0xa5) return 5;
    for (index = 1; index < 259; ++index)
        if (output[index] != 0x7e) return 6;
    if (copy(NULL, NULL, INT_MIN, 0) || errno != 124) return 7;
    source[0] = (unsigned char)((unsigned int)INT_MIN & UCHAR_MAX);
    if (copy(output, source, INT_MIN, 1) != output + 1) return 8;
    source[0] = (unsigned char)((unsigned int)INT_MAX & UCHAR_MAX);
    if (copy(output, source, INT_MAX, 1) != output + 1) return 9;
    return 0;
}
