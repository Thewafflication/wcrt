/**
 * @file stdio_slots.c
 * @brief Verifies stream-table failure cleanup and descriptor ownership.
 */

#define WCRT_POSIX 1

#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

/** @brief Descriptors held while testing the dynamic table boundary. */
static int held[FOPEN_MAX];

/**
 * @brief Fills every dynamic slot and checks descriptor uniqueness.
 * @return Zero on success, otherwise a diagnostic exit code.
 */
static int fill_table(void)
{
    int index;
    int other;
    char byte;

    for (index = 0; index < FOPEN_MAX; ++index) {
        held[index] = _open("tc0074-slots.tmp", _O_RDWR | _O_BINARY);
        if (held[index] < 3) return 10;
        for (other = 0; other < index; ++other) {
            if (held[other] == held[index]) return 11;
        }
    }
    errno = 0;
    if (fopen("tc0074-slots.tmp", "rb") != NULL || errno != EMFILE)
        return 12;
    errno = 0;
    if (_open("tc0074-slots.tmp", _O_RDONLY) != -1 || errno != EMFILE)
        return 13;
    errno = 0;
    if (_dup(held[0]) != -1 || errno != EMFILE) return 14;
    for (index = 0; index < FOPEN_MAX; ++index) {
        if (_read(held[index], &byte, 1) != 1 || byte != 's') return 15;
    }
    return 0;
}

/**
 * @brief Checks that unsuccessful opens leave the full table available.
 * @return Zero on success, otherwise a diagnostic exit code.
 */
static int failed_opens(void)
{
    int index;
    FILE *stream;

    stream = fopen("tc0074-slots.tmp", "wb");
    if (stream == NULL) return 20;
    if (fputc('s', stream) == EOF || fclose(stream) != 0) return 21;
    remove("tc0074-missing.tmp");
    for (index = 0; index < FOPEN_MAX * 2; ++index) {
        errno = 0;
        if (fopen("tc0074-missing.tmp", "rb") != NULL)
            return 22;
        errno = 0;
        if (_open("tc0074-missing.tmp", _O_RDONLY) != -1 ||
            errno != ENOENT) return 23;
        stream = fopen("tc0074-slots.tmp", "rb");
        if (stream == NULL) return 24;
        if (freopen("tc0074-missing.tmp", "rb", stream) != NULL)
            return 25;
    }
    return fill_table();
}

/**
 * @brief Checks pipe rollback with one slot and success with two slots.
 * @return Zero on success, otherwise a diagnostic exit code.
 */
static int pipe_slots(void)
{
    int pair[2];
    int replacement;
    char byte;

    if (_close(held[0]) != 0) return 30;
    pair[0] = -7;
    pair[1] = -9;
    errno = 0;
    if (_pipe(pair, 0, _O_BINARY) != -1 || errno != EMFILE) return 31;
    if (pair[0] != -7 || pair[1] != -9) return 32;
    replacement = _open("tc0074-slots.tmp", _O_RDONLY | _O_BINARY);
    if (replacement != held[0]) return 33;
    if (_close(replacement) != 0 || _close(held[1]) != 0) return 34;
    if (_pipe(pair, 0, _O_BINARY) != 0) return 35;
    if (pair[0] == pair[1] || pair[0] < 3 || pair[1] < 3) return 36;
    if (_write(pair[1], "p", 1) != 1 || _read(pair[0], &byte, 1) != 1 ||
        byte != 'p') return 37;
    if (_close(pair[0]) != 0 || _close(pair[1]) != 0) return 38;
    return 0;
}

/**
 * @brief Checks explicit and allocated duplicate targets retain ownership.
 * @return Zero on success, otherwise a diagnostic exit code.
 */
static int duplicate_slots(void)
{
    int copy;
    int index;
    char byte;

    errno = 0;
    if (_dup2(-1, held[0]) != -1 || errno != EBADF) return 48;
    if (_dup2(held[2], held[0]) != 0) return 40;
    copy = _dup(held[2]);
    if (copy != held[1]) return 41;
    errno = 0;
    if (_open("tc0074-slots.tmp", _O_RDONLY) != -1 || errno != EMFILE)
        return 42;
    if (_close(held[2]) != 0) return 43;
    if (_lseek(held[0], 0, SEEK_SET) != 0 ||
        _read(held[0], &byte, 1) != 1 || byte != 's') return 44;
    if (_lseek(copy, 0, SEEK_SET) != 0 ||
        _read(copy, &byte, 1) != 1 || byte != 's') return 45;
    if (_close(held[0]) != 0 || _close(copy) != 0) return 46;
    for (index = 3; index < FOPEN_MAX; ++index) {
        if (_close(held[index]) != 0) return 47;
    }
    return 0;
}

int main(void)
{
    int result;
    int index;

    result = failed_opens();
    if (result != 0) return result;
    result = pipe_slots();
    if (result != 0) return result;
    result = duplicate_slots();
    if (result != 0) return result;
    result = fill_table();
    if (result != 0) return result;
    for (index = 0; index < FOPEN_MAX; ++index) {
        if (_close(held[index]) != 0) return 50;
    }
    return remove("tc0074-slots.tmp") == 0 ? 0 : 51;
}
