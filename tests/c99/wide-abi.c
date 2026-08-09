/**
 * @file wide-abi.c
 * @brief Exercises the T3 scalar, state, conversion, and orientation ABI.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

struct wcrt_mbstate_alignment {
    char lead;
    mbstate_t state;
};

typedef unsigned short wcrt_windows_wchar;

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_TEST_WINAPI __attribute__((stdcall))
#else
#define WCRT_TEST_WINAPI __stdcall
#endif

__declspec(dllimport) int WCRT_TEST_WINAPI lstrlenW(
    const wcrt_windows_wchar *string);

static int wcrt_test_types(void)
{
    mbstate_t state;
    wcrt_windows_wchar windows_text[] = { 0x0041, 0xd83d, 0xde00, 0 };
    const wchar_t *wide_text = (const wchar_t *)windows_text;

    if (sizeof(wchar_t) != 2 || (wchar_t)-1 < (wchar_t)0) return 1;
    if (sizeof(wint_t) != 4 || (wint_t)-1 < (wint_t)0) return 2;
    if (WEOF != (wint_t)-1 || WEOF <= (wint_t)(wchar_t)-1) return 3;
    if (WINT_MIN != 0 || WINT_MAX != UINT32_MAX) return 4;
    if (sizeof(mbstate_t) != 8) return 5;
    if (offsetof(struct wcrt_mbstate_alignment, state) != 4) return 6;
    memset(&state, 0, sizeof(state));
    if (!mbsinit(&state)) return 7;
    if (wide_text[0] != 0x0041 || wide_text[1] != 0xd83d ||
        wide_text[2] != 0xde00 || wide_text[3] != 0) return 8;
    if (lstrlenW((const wcrt_windows_wchar *)wide_text) != 3) return 9;
    return 0;
}

static int wcrt_test_single_conversions(void)
{
    mbstate_t state;
    wchar_t wc = 0x7777;
    char byte = (char)0x55;
    size_t result;

    memset(&state, 0, sizeof(state));
    if (btowc(EOF) != WEOF || btowc(0xff) != (wint_t)0xff) return 10;
    if (wctob(WEOF) != EOF || wctob((wint_t)0x100) != EOF) return 11;
    if ((unsigned char)wctob((wint_t)0xff) != 0xff) return 12;

    result = mbrtowc(&wc, "A", 1, &state);
    if (result != 1 || wc != L'A' || !mbsinit(&state)) return 13;
    result = mbrtowc(&wc, "", 1, &state);
    if (result != 0 || wc != 0 || !mbsinit(&state)) return 14;
    wc = 0x7777;
    result = mbrtowc(&wc, "A", 0, &state);
    if (result != (size_t)-2 || wc != 0x7777 || !mbsinit(&state)) return 15;
    if (mbrtowc((wchar_t *)0, (const char *)0, 0, &state) != 0) return 16;
    if (mbrlen("Z", 1, &state) != 1 || mbrlen("", 1, &state) != 0) return 17;

    result = wcrtomb(&byte, L'Q', &state);
    if (result != 1 || byte != 'Q' || !mbsinit(&state)) return 18;
    errno = 0;
    byte = (char)0x55;
    result = wcrtomb(&byte, (wchar_t)0x100, &state);
    if (result != (size_t)-1 || errno != EILSEQ || byte != (char)0x55 ||
        !mbsinit(&state)) return 19;
    if (wcrtomb((char *)0, 0, &state) != 1 || !mbsinit(&state)) return 20;
    return 0;
}

static int wcrt_test_string_conversions(void)
{
    mbstate_t state;
    const char input[] = { 'A', (char)0xff, 'B', 0 };
    const char *source;
    const wchar_t wide_input[] = { L'A', (wchar_t)0xff, L'B', 0 };
    const wchar_t invalid_input[] = { L'A', (wchar_t)0x100, L'B', 0 };
    const wchar_t *wide_source;
    wchar_t wide[5] = { 9, 9, 9, 9, 9 };
    char bytes[5] = { 9, 9, 9, 9, 9 };
    size_t result;

    memset(&state, 0, sizeof(state));
    source = input;
    result = mbsrtowcs(wide, &source, 2, &state);
    if (result != 2 || source != input + 2 || wide[0] != L'A' ||
        wide[1] != (wchar_t)0xff || wide[2] != 9) return 30;
    result = mbsrtowcs(wide + 2, &source, 3, &state);
    if (result != 1 || source != (const char *)0 || wide[2] != L'B' ||
        wide[3] != 0 || !mbsinit(&state)) return 31;
    source = input;
    if (mbsrtowcs((wchar_t *)0, &source, 0, &state) != 3 ||
        source != input) return 32;

    wide_source = wide_input;
    result = wcsrtombs(bytes, &wide_source, 2, &state);
    if (result != 2 || wide_source != wide_input + 2 || bytes[0] != 'A' ||
        (unsigned char)bytes[1] != 0xff || bytes[2] != 9) return 33;
    result = wcsrtombs(bytes + 2, &wide_source, 3, &state);
    if (result != 1 || wide_source != (const wchar_t *)0 || bytes[2] != 'B' ||
        bytes[3] != 0 || !mbsinit(&state)) return 34;
    wide_source = wide_input;
    if (wcsrtombs((char *)0, &wide_source, 0, &state) != 3 ||
        wide_source != wide_input) return 35;

    wide_source = invalid_input;
    errno = 0;
    bytes[0] = bytes[1] = 9;
    result = wcsrtombs(bytes, &wide_source, sizeof(bytes), &state);
    if (result != (size_t)-1 || errno != EILSEQ ||
        wide_source != invalid_input + 1 || bytes[0] != 'A' || bytes[1] != 9)
        return 36;
    return 0;
}

static int wcrt_test_orientation(void)
{
    FILE *file;
    wint_t wc;
    fpos_t position;
    long before;

    file = tmpfile();
    if (file == (FILE *)0) return 40;
    if (fwide(file, 0) != 0) return 41;
    if (fputwc(L'A', file) != (wint_t)L'A' || fwide(file, 0) <= 0) return 42;
    if (fseek(file, 0, SEEK_SET) != 0 || fwide(file, 0) <= 0) return 43;
    wc = fgetwc(file);
    if (wc != (wint_t)L'A') return 44;
    clearerr(file);
    errno = 0;
    before = ftell(file);
    if (fputc('B', file) != EOF || !ferror(file) || errno != EINVAL ||
        ftell(file) != before) return 45;
    fclose(file);

    file = tmpfile();
    if (file == (FILE *)0) return 46;
    if (fputc('A', file) != 'A' || fwide(file, 0) >= 0) return 47;
    clearerr(file);
    errno = 0;
    before = ftell(file);
    if (fputwc(L'B', file) != WEOF || !ferror(file) || errno != EINVAL ||
        ftell(file) != before)
        return 48;
    fclose(file);

    file = tmpfile();
    if (file == (FILE *)0) return 49;
    if (fwide(file, 7) <= 0 || fputws(L"AB", file) < 0) return 50;
    if (fseek(file, 0, SEEK_SET) != 0 || fgetwc(file) != (wint_t)L'A')
        return 51;
    if (ungetwc(L'A', file) != (wint_t)L'A' ||
        fgetpos(file, &position) != 0 || position != 0) return 52;
    if (fgetwc(file) != (wint_t)L'A' || fsetpos(file, &position) != 0 ||
        fgetwc(file) != (wint_t)L'A') return 53;
    fclose(file);

    file = tmpfile();
    if (file == (FILE *)0) return 54;
    errno = 0;
    if (fputwc((wchar_t)0x100, file) != WEOF || errno != EILSEQ ||
        !ferror(file) || ftell(file) != 0) return 55;
    fclose(file);

    remove("wcrt-t3-orientation.tmp");
    file = fopen("wcrt-t3-orientation.tmp", "w+b");
    if (file == (FILE *)0 || fwide(file, 1) <= 0) return 56;
    if (freopen("wcrt-t3-orientation.tmp", "w+b", file) != file ||
        fwide(file, 0) != 0 || fwide(file, -1) >= 0) return 57;
    fclose(file);
    remove("wcrt-t3-orientation.tmp");
    return 0;
}

/** @brief Runs the complete ABI and state-transition checks. */
int main(void)
{
    int result;

    result = wcrt_test_types();
    if (result != 0) return result;
    result = wcrt_test_single_conversions();
    if (result != 0) return result;
    result = wcrt_test_string_conversions();
    if (result != 0) return result;
    return wcrt_test_orientation();
}
