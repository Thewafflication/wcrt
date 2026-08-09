/**
 * @file wchar_convert.c
 * @brief Implements C-locale restartable wide-character conversion.
 */

#include <errno.h>
#include <stdio.h>
#include <wchar.h>

static mbstate_t wcrt_mbrtowc_state;
static mbstate_t wcrt_mbrlen_state;
static mbstate_t wcrt_wcrtomb_state;
static mbstate_t wcrt_mbsrtowcs_state;
static mbstate_t wcrt_wcsrtombs_state;

static void wcrt_reset_mbstate(mbstate_t *state)
{
    state->__value = 0;
    state->__bytes = 0;
    state->__state = 0;
}

wint_t btowc(int character)
{
    if (character == EOF || character < 0 || character > 0xff) {
        return WEOF;
    }
    return (wint_t)(unsigned char)character;
}

int wctob(wint_t character)
{
    if (character == WEOF || character > 0xffU) {
        return EOF;
    }
    return (int)(unsigned char)character;
}

int mbsinit(const mbstate_t *state)
{
    return state == (const mbstate_t *)0 ||
        (state->__value == 0 && state->__bytes == 0 && state->__state == 0);
}

size_t mbrtowc(wchar_t *destination, const char *source, size_t count,
    mbstate_t *state)
{
    wchar_t ignored;

    if (state == (mbstate_t *)0) {
        state = &wcrt_mbrtowc_state;
    }
    if (source == (const char *)0) {
        source = "";
        count = 1;
        destination = &ignored;
    }
    if (count == 0) {
        return (size_t)-2;
    }
    if (destination != (wchar_t *)0) {
        *destination = (wchar_t)(unsigned char)*source;
    }
    wcrt_reset_mbstate(state);
    return *source == '\0' ? 0 : 1;
}

size_t mbrlen(const char *source, size_t count, mbstate_t *state)
{
    if (state == (mbstate_t *)0) state = &wcrt_mbrlen_state;
    return mbrtowc((wchar_t *)0, source, count, state);
}

size_t wcrtomb(char *destination, wchar_t character, mbstate_t *state)
{
    char ignored;

    if (state == (mbstate_t *)0) {
        state = &wcrt_wcrtomb_state;
    }
    if (destination == (char *)0) {
        destination = &ignored;
        character = 0;
    }
    if ((wint_t)character > 0xffU) {
        errno = EILSEQ;
        wcrt_reset_mbstate(state);
        return (size_t)-1;
    }
    *destination = (char)(unsigned char)character;
    wcrt_reset_mbstate(state);
    return 1;
}

size_t mbsrtowcs(wchar_t *destination, const char **source, size_t count,
    mbstate_t *state)
{
    const char *current = *source;
    size_t converted = 0;

    if (state == (mbstate_t *)0) {
        state = &wcrt_mbsrtowcs_state;
    }
    if (destination == (wchar_t *)0) {
        while (current[converted] != '\0') {
            ++converted;
        }
        wcrt_reset_mbstate(state);
        return converted;
    }
    while (converted < count) {
        unsigned char character = (unsigned char)*current;
        destination[converted] = (wchar_t)character;
        if (character == 0) {
            *source = (const char *)0;
            wcrt_reset_mbstate(state);
            return converted;
        }
        ++converted;
        ++current;
    }
    *source = current;
    wcrt_reset_mbstate(state);
    return converted;
}

size_t wcsrtombs(char *destination, const wchar_t **source, size_t count,
    mbstate_t *state)
{
    const wchar_t *current = *source;
    size_t converted = 0;

    if (state == (mbstate_t *)0) {
        state = &wcrt_wcsrtombs_state;
    }
    if (destination == (char *)0) {
        while (current[converted] != 0) {
            if ((wint_t)current[converted] > 0xffU) {
                errno = EILSEQ;
                wcrt_reset_mbstate(state);
                return (size_t)-1;
            }
            ++converted;
        }
        wcrt_reset_mbstate(state);
        return converted;
    }
    while (converted < count) {
        wchar_t character = *current;
        if ((wint_t)character > 0xffU) {
            errno = EILSEQ;
            *source = current;
            wcrt_reset_mbstate(state);
            return (size_t)-1;
        }
        destination[converted] = (char)(unsigned char)character;
        if (character == 0) {
            *source = (const wchar_t *)0;
            wcrt_reset_mbstate(state);
            return converted;
        }
        ++converted;
        ++current;
    }
    *source = current;
    wcrt_reset_mbstate(state);
    return converted;
}
