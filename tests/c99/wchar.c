/**
 * @file wchar.c
 * @brief Exercises C99 wide strings, I/O, numeric text, and time formatting.
 */
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

static int wcrt_vformat(wchar_t *buffer, size_t length,
    const wchar_t *format, ...)
{
    int result;
    va_list arguments;

    va_start(arguments, format);
    result = vswprintf(buffer, length, format, arguments);
    va_end(arguments);
    return result;
}

static int wcrt_vscan(const wchar_t *input, const wchar_t *format, ...)
{
    int result;
    va_list arguments;

    va_start(arguments, format);
    result = vswscanf(input, format, arguments);
    va_end(arguments);
    return result;
}

static int wcrt_vfile_format(FILE *stream, const wchar_t *format, ...)
{
    int result;
    va_list arguments;

    va_start(arguments, format);
    result = vfwprintf(stream, format, arguments);
    va_end(arguments);
    return result;
}

static int wcrt_vfile_scan(FILE *stream, const wchar_t *format, ...)
{
    int result;
    va_list arguments;

    va_start(arguments, format);
    result = vfwscanf(stream, format, arguments);
    va_end(arguments);
    return result;
}

static int wcrt_test_strings(void)
{
    wchar_t buffer[32];
    wchar_t overlap[8] = { L'a', L'b', L'c', L'd', 0, 0, 0, 0 };
    wchar_t tokens[] = L",one,,two";
    const wchar_t abca[] = L"abca";
    const wchar_t abc[] = L"abc";
    const wchar_t abcabc[] = L"abcabc";
    wchar_t *context = (wchar_t *)0;
    wchar_t *token;

    if (wcslen(L"") != 0 || wcslen(L"abc") != 3) return 1;
    if (wcscpy(buffer, L"ab") != buffer || wcscmp(buffer, L"ab")) return 2;
    if (wcscat(buffer, L"cd") != buffer || wcscmp(buffer, L"abcd")) return 3;
    wmemset(buffer, L'x', 6);
    if (wcsncpy(buffer, L"ab", 5) != buffer || buffer[0] != L'a' ||
        buffer[1] != L'b' || buffer[2] || buffer[3] || buffer[4]) return 4;
    wcscpy(buffer, L"ab");
    if (wcsncat(buffer, L"cdef", 2) != buffer || wcscmp(buffer, L"abcd"))
        return 5;
    if (!(wcscmp(L"a", L"b") < 0) || !(wcsncmp(L"aa", L"ab", 2) < 0) ||
        wcscmp(L"same", L"same")) return 6;
    if (wcscoll(L"alpha", L"alpha") || wcscoll(L"a", L"b") >= 0) return 7;
    wmemset(buffer, L'?', 6);
    if (wcsxfrm(buffer, L"abc", 6) != 3 || wcscmp(buffer, L"abc")) return 8;
    buffer[0] = L'!';
    if (wcsxfrm(buffer, L"abc", 0) != 3 || buffer[0] != L'!') return 9;
    if (wcschr(abca, L'a') != abca ||
        wcsrchr(abca, L'a') != abca + 3 ||
        wcschr(abc, 0) != abc + 3) return 10;
    if (wcscspn(L"abcde", L"dx") != 3 || wcsspn(L"aab", L"ab") != 3 ||
        wcspbrk(abc, L"xz") != (wchar_t *)0 ||
        wcspbrk(abc, L"xb") != abc + 1) return 11;
    if (wcsstr(abcabc, L"cab") != abcabc + 2 ||
        wcsstr(abc, L"") != abc ||
        wcsstr(abc, L"z") != (wchar_t *)0) return 12;

    token = wcstok(tokens, L",", &context);
    if (!token || wcscmp(token, L"one")) return 13;
    token = wcstok((wchar_t *)0, L",", &context);
    if (!token || wcscmp(token, L"two")) return 14;
    if (wcstok((wchar_t *)0, L",", &context) != (wchar_t *)0) return 15;

    if (wmemcpy(buffer, L"abcd", 5) != buffer || wmemcmp(buffer, L"abcd", 5))
        return 16;
    if (wmemchr(buffer, L'c', 5) != buffer + 2 ||
        wmemchr(buffer, L'z', 5) != (wchar_t *)0) return 17;
    if (wmemset(buffer, L'q', 3) != buffer || buffer[0] != L'q' ||
        buffer[2] != L'q') return 18;
    if (wmemmove(overlap + 1, overlap, 4) != overlap + 1 ||
        overlap[0] != L'a' || overlap[1] != L'a' || overlap[4] != L'd')
        return 19;
    return 0;
}

static int wcrt_test_numeric(void)
{
    wchar_t *end;
    double value;

    errno = 0;
    if (wcstol(L" -0x2a!", &end, 0) != -42 || *end != L'!') return 30;
    if (wcstoul(L"377x", &end, 8) != 255 || *end != L'x') return 31;
    if (wcstoll(L"9223372036854775807", &end, 10) !=
        9223372036854775807LL || *end) return 32;
    if (wcstoull(L"18446744073709551615", &end, 10) !=
        18446744073709551615ULL || *end) return 33;
    if (wcstoimax(L"-123q", &end, 10) != (intmax_t)-123 || *end != L'q')
        return 34;
    if (wcstoumax(L"123q", &end, 10) != (uintmax_t)123 || *end != L'q')
        return 35;
    value = wcstod(L"0x1.8p+1z", &end);
    if (value != 3.0 || *end != L'z') return 36;
    if (wcstof(L"1.25x", &end) != 1.25f || *end != L'x') return 37;
    if (wcstold(L"-0.5x", &end) != (long double)-0.5 || *end != L'x')
        return 38;
    return 0;
}

static int wcrt_test_format(void)
{
    wchar_t buffer[128];
    wchar_t word[16];
    char narrow[16];
    int integer;
    unsigned hex;
    double real;
    int count = -1;
    int result;
    signed char signed_hh;
    short signed_h;
    long signed_l;
    long long signed_ll;
    intmax_t signed_j;
    ptrdiff_t signed_z;
    ptrdiff_t signed_t;
    unsigned char unsigned_hh;
    unsigned short unsigned_h;
    unsigned long unsigned_l;
    unsigned long long unsigned_ll;
    uintmax_t unsigned_j;
    size_t unsigned_z;
    size_t unsigned_t;
    wchar_t tail;
    float malformed_value = 9.0f;
    const wchar_t high_text[] = { 0xd83d, 0xde00, 0 };
    const wchar_t high_input[] = { 0xd83d, 0xde00, L' ', 0x20ac, 0 };

    result = swprintf(buffer, 128, L"%s:%d:%#x:%.1f:%c:%lc%n",
        "byte", -12, 42u, 2.5, 'W', (wint_t)L'N', &count);
    if (result != 21 || count != result ||
        wcscmp(buffer, L"byte:-12:0x2a:2.5:W:N")) return 40;
    result = wcrt_vformat(buffer, 128, L"%08lld %.2a %ls",
        42LL, 1.5, L"wide");
    if (result <= 0 || wcscmp(buffer, L"00000042 0x1.80p+0 wide")) return 41;
    buffer[0] = L'!';
    if (swprintf(buffer, 4, L"hello") >= 0) return 42;

    integer = 0;
    hex = 0;
    real = 0.0;
    word[0] = 0;
    narrow[0] = 0;
    result = swscanf(L"-12 2a 1.5 wide byte", L"%d %x %lf %s %ls",
        &integer, &hex, &real, word, narrow);
    if (result != 5 || integer != -12 || hex != 42 || real != 1.5 ||
        wcscmp(word, L"wide") || strcmp(narrow, "byte")) return 43;
    integer = 0;
    count = -1;
    if (wcrt_vscan(L"77!", L"%d%n", &integer, &count) != 1 ||
        integer != 77 || count != 2) return 44;
    result = swprintf(buffer, 128, L"%ls:%lc", high_text, (wint_t)0x20ac);
    if (result != 4 || buffer[0] != 0xd83d || buffer[1] != 0xde00 ||
        buffer[2] != L':' || buffer[3] != 0x20ac || buffer[4] != 0) return 45;
    word[0] = word[1] = word[2] = 0;
    buffer[0] = 0;
    result = swscanf(high_input, L"%s %c", word, buffer);
    if (result != 2 || word[0] != 0xd83d || word[1] != 0xde00 || word[2] ||
        buffer[0] != 0x20ac) return 46;
    result = swprintf(buffer, 128, L"%hhd %hd %ld %lld %jd %zd %td",
        -1, -2, -3L, -4LL, (intmax_t)-5, (ptrdiff_t)-6,
        (ptrdiff_t)-7);
    if (result < 0 || wcscmp(buffer, L"-1 -2 -3 -4 -5 -6 -7")) return 47;
    signed_hh = 0;
    signed_h = 0;
    signed_l = 0;
    signed_ll = 0;
    signed_j = 0;
    signed_z = 0;
    signed_t = 0;
    result = swscanf(L"-1 -2 -3 -4 -5 -6 -7",
        L"%hhd %hd %ld %lld %jd %zd %td", &signed_hh, &signed_h,
        &signed_l, &signed_ll, &signed_j, &signed_z, &signed_t);
    if (result != 7 || signed_hh != -1 || signed_h != -2 ||
        signed_l != -3 || signed_ll != -4 || signed_j != -5 ||
        signed_z != -6 || signed_t != -7) return 48;
    result = swprintf(buffer, 128, L"%hhu %hu %lu %llu %ju %zu %tu",
        1U, 2U, 3UL, 4ULL, (uintmax_t)5, (size_t)6, (size_t)7);
    if (result < 0 || wcscmp(buffer, L"1 2 3 4 5 6 7")) return 49;
    unsigned_hh = 0;
    unsigned_h = 0;
    unsigned_l = 0;
    unsigned_ll = 0;
    unsigned_j = 0;
    unsigned_z = 0;
    unsigned_t = 0;
    result = swscanf(L"1 2 3 4 5 6 7",
        L"%hhu %hu %lu %llu %ju %zu %tu", &unsigned_hh, &unsigned_h,
        &unsigned_l, &unsigned_ll, &unsigned_j, &unsigned_z, &unsigned_t);
    if (result != 7 || unsigned_hh != 1 || unsigned_h != 2 ||
        unsigned_l != 3 || unsigned_ll != 4 || unsigned_j != 5 ||
        unsigned_z != 6 || unsigned_t != 7) return 50;
    tail = 0;
    integer = 0;
    if (swscanf(L"123", L"%2d%c", &integer, &tail) != 2 ||
        integer != 12 || tail != L'3') return 51;
    tail = L'!';
    if (swscanf(L"100er", L"%f%c", &malformed_value, &tail) != 0 ||
        malformed_value != 9.0f || tail != L'!') return 52;
    return 0;
}

static int wcrt_test_streams(void)
{
    FILE *file = tmpfile();
    wchar_t buffer[8];
    wchar_t invalid_format[2];
    int number;
    long position;

    if (!file) return 50;
    if (fputws(L"ab\n", file) < 0 || putwc(L'c', file) == WEOF) return 51;
    if (fseek(file, 0, SEEK_SET) != 0) return 52;
    if (fgetws(buffer, 8, file) != buffer || wcscmp(buffer, L"ab\n"))
        return 53;
    if (getwc(file) != (wint_t)L'c') return 54;
    if (getwc(file) != WEOF || !feof(file)) return 55;
    if (ungetwc(L'Z', file) != (wint_t)L'Z' || feof(file)) return 56;
    if (ungetwc(L'Y', file) != WEOF || getwc(file) != (wint_t)L'Z') return 57;
    fclose(file);

    file = tmpfile();
    if (!file) return 58;
    if (fwprintf(file, L"%d ", 42) != 3 ||
        wcrt_vfile_format(file, L"%ls!", L"wide") != 5) return 59;
    if (fseek(file, 0, SEEK_SET) != 0) return 60;
    number = 0;
    buffer[0] = 0;
    if (fwscanf(file, L"%d", &number) != 1 || number != 42) return 61;
    if (wcrt_vfile_scan(file, L"%4s", buffer) != 1) return 62;
    if (wcscmp(buffer, L"wide")) return 63;
    if (fgetwc(file) != (wint_t)L'!') return 64;
    fclose(file);

    file = tmpfile();
    if (!file || fputwc(L'A', file) == WEOF ||
        fseek(file, 0, SEEK_SET) != 0) return 65;
    invalid_format[0] = (wchar_t)0x100;
    invalid_format[1] = 0;
    position = ftell(file);
    errno = 0;
    if (fwscanf(file, invalid_format) != EOF || errno != EILSEQ ||
        ftell(file) != position) return 66;
    if (fgetwc(file) != (wint_t)L'A') return 67;
    fclose(file);
    return 0;
}

static int wcrt_test_time(void)
{
    struct tm value;
    wchar_t buffer[64];

    memset(&value, 0, sizeof(value));
    value.tm_year = 100;
    value.tm_mon = 0;
    value.tm_mday = 2;
    value.tm_wday = 0;
    value.tm_yday = 1;
    if (wcsftime(buffer, 64, L"%Y-%m-%d %F", &value) != 21 ||
        wcscmp(buffer, L"2000-01-02 2000-01-02")) return 70;
    buffer[0] = L'!';
    if (wcsftime(buffer, 4, L"%Y", &value) != 0) return 71;
    value.tm_year = 116;
    value.tm_mon = 0;
    value.tm_mday = 1;
    value.tm_wday = 5;
    value.tm_yday = 0;
    if (wcsftime(buffer, 64, L"%G-W%V-%u", &value) != 10 ||
        wcscmp(buffer, L"2015-W53-5")) return 72;
    buffer[0] = L'!';
    if (wcsftime(buffer, 64, L"%z", &value) != 0 || buffer[0] != 0)
        return 73;
    buffer[0] = L'!';
    if (wcsftime(buffer, 64, L"", &value) != 0 || buffer[0] != 0)
        return 74;
    return 0;
}

/** @brief Runs the T3 wide-library behavior checks. */
int main(void)
{
    int result;

    result = wcrt_test_strings();
    if (result != 0) return result;
    result = wcrt_test_numeric();
    if (result != 0) return result;
    result = wcrt_test_format();
    if (result != 0) return result;
    result = wcrt_test_streams();
    if (result != 0) return result;
    return wcrt_test_time();
}
