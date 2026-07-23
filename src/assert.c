/**
 * @file assert.c
 * @brief Implements C89 assertion diagnostics with Windows 2000 APIs.
 *
 * Waughtal C Run Time
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#if defined(__TINYC__) || defined(__GNUC__)
/** @brief Selects the Windows API calling convention. */
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

/** @brief Windows standard-error handle selector. */
#define WCRT_STD_ERROR_HANDLE ((unsigned long)-12)

/** @brief Retrieves a Windows standard stream handle. */
__declspec(dllimport) void *WCRT_WINAPI GetStdHandle(unsigned long selector);

/** @brief Writes bytes to a Windows handle. */
__declspec(dllimport) int WCRT_WINAPI WriteFile(void *handle,
    const void *buffer, unsigned long count, unsigned long *written,
    void *overlapped);

/** @brief Terminates the current Windows process. */
__declspec(dllimport) void WCRT_WINAPI ExitProcess(unsigned int exit_code);

/**
 * @brief Appends a terminated string to a bounded diagnostic buffer.
 * @param destination Destination diagnostic buffer.
 * @param position Current destination position.
 * @param capacity Total destination capacity.
 * @param source Terminated source string.
 * @return Updated destination position.
 */
static unsigned long wcrt_append(char *destination, unsigned long position,
    unsigned long capacity, const char *source)
{
    while (*source != '\0' && position < capacity) {
        destination[position++] = *source++;
    }
    return position;
}

/**
 * @brief Appends a positive decimal integer to a bounded buffer.
 * @param destination Destination diagnostic buffer.
 * @param position Current destination position.
 * @param capacity Total destination capacity.
 * @param value Nonnegative value to append.
 * @return Updated destination position.
 */
static unsigned long wcrt_append_number(char *destination,
    unsigned long position, unsigned long capacity, unsigned int value)
{
    char digits[10];
    unsigned int count = 0;

    do {
        digits[count++] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    while (count != 0 && position < capacity) {
        destination[position++] = digits[--count];
    }
    return position;
}

void __wcrt_assert_fail(const char *expression, const char *file, int line)
{
    char diagnostic[512];
    unsigned long length = 0;
    unsigned long written;
    void *standard_error;

    length = wcrt_append(diagnostic, length, sizeof(diagnostic),
        "Assertion failed: ");
    length = wcrt_append(diagnostic, length, sizeof(diagnostic), expression);
    length = wcrt_append(diagnostic, length, sizeof(diagnostic), ", file ");
    length = wcrt_append(diagnostic, length, sizeof(diagnostic), file);
    length = wcrt_append(diagnostic, length, sizeof(diagnostic), ", line ");
    length = wcrt_append_number(diagnostic, length, sizeof(diagnostic),
        (unsigned int)line);
    length = wcrt_append(diagnostic, length, sizeof(diagnostic), "\r\n");
    standard_error = GetStdHandle(WCRT_STD_ERROR_HANDLE);
    WriteFile(standard_error, diagnostic, length, &written, (void *)0);
    ExitProcess(3);
}
