/**
 * @file wctype.c
 * @brief Implements C-locale wide classification and mapping.
 */

#include <ctype.h>
#include <string.h>
#include <wctype.h>

#define WCRT_WCTYPE_ALNUM 1
#define WCRT_WCTYPE_ALPHA 2
#define WCRT_WCTYPE_BLANK 3
#define WCRT_WCTYPE_CNTRL 4
#define WCRT_WCTYPE_DIGIT 5
#define WCRT_WCTYPE_GRAPH 6
#define WCRT_WCTYPE_LOWER 7
#define WCRT_WCTYPE_PRINT 8
#define WCRT_WCTYPE_PUNCT 9
#define WCRT_WCTYPE_SPACE 10
#define WCRT_WCTYPE_UPPER 11
#define WCRT_WCTYPE_XDIGIT 12
#define WCRT_WCTRANS_LOWER 1
#define WCRT_WCTRANS_UPPER 2

static int wcrt_wide_byte(wint_t character)
{
    return character <= 0xffU;
}

int iswalnum(wint_t character)
{
    return wcrt_wide_byte(character) ? isalnum((int)character) : 0;
}

int iswalpha(wint_t character)
{
    return wcrt_wide_byte(character) ? isalpha((int)character) : 0;
}

int iswblank(wint_t character)
{
    return character == (wint_t)' ' || character == (wint_t)'\t';
}

int iswcntrl(wint_t character)
{
    return wcrt_wide_byte(character) ? iscntrl((int)character) : 0;
}

int iswdigit(wint_t character)
{
    return wcrt_wide_byte(character) ? isdigit((int)character) : 0;
}

int iswgraph(wint_t character)
{
    return wcrt_wide_byte(character) ? isgraph((int)character) : 0;
}

int iswlower(wint_t character)
{
    return wcrt_wide_byte(character) ? islower((int)character) : 0;
}

int iswprint(wint_t character)
{
    return wcrt_wide_byte(character) ? isprint((int)character) : 0;
}

int iswpunct(wint_t character)
{
    return wcrt_wide_byte(character) ? ispunct((int)character) : 0;
}

int iswspace(wint_t character)
{
    return wcrt_wide_byte(character) ? isspace((int)character) : 0;
}

int iswupper(wint_t character)
{
    return wcrt_wide_byte(character) ? isupper((int)character) : 0;
}

int iswxdigit(wint_t character)
{
    return wcrt_wide_byte(character) ? isxdigit((int)character) : 0;
}

wctype_t wctype(const char *name)
{
    if (name == (const char *)0) return 0;
    if (strcmp(name, "alnum") == 0) return WCRT_WCTYPE_ALNUM;
    if (strcmp(name, "alpha") == 0) return WCRT_WCTYPE_ALPHA;
    if (strcmp(name, "blank") == 0) return WCRT_WCTYPE_BLANK;
    if (strcmp(name, "cntrl") == 0) return WCRT_WCTYPE_CNTRL;
    if (strcmp(name, "digit") == 0) return WCRT_WCTYPE_DIGIT;
    if (strcmp(name, "graph") == 0) return WCRT_WCTYPE_GRAPH;
    if (strcmp(name, "lower") == 0) return WCRT_WCTYPE_LOWER;
    if (strcmp(name, "print") == 0) return WCRT_WCTYPE_PRINT;
    if (strcmp(name, "punct") == 0) return WCRT_WCTYPE_PUNCT;
    if (strcmp(name, "space") == 0) return WCRT_WCTYPE_SPACE;
    if (strcmp(name, "upper") == 0) return WCRT_WCTYPE_UPPER;
    if (strcmp(name, "xdigit") == 0) return WCRT_WCTYPE_XDIGIT;
    return 0;
}

int iswctype(wint_t character, wctype_t descriptor)
{
    switch (descriptor) {
    case WCRT_WCTYPE_ALNUM: return iswalnum(character);
    case WCRT_WCTYPE_ALPHA: return iswalpha(character);
    case WCRT_WCTYPE_BLANK: return iswblank(character);
    case WCRT_WCTYPE_CNTRL: return iswcntrl(character);
    case WCRT_WCTYPE_DIGIT: return iswdigit(character);
    case WCRT_WCTYPE_GRAPH: return iswgraph(character);
    case WCRT_WCTYPE_LOWER: return iswlower(character);
    case WCRT_WCTYPE_PRINT: return iswprint(character);
    case WCRT_WCTYPE_PUNCT: return iswpunct(character);
    case WCRT_WCTYPE_SPACE: return iswspace(character);
    case WCRT_WCTYPE_UPPER: return iswupper(character);
    case WCRT_WCTYPE_XDIGIT: return iswxdigit(character);
    default: return 0;
    }
}

wint_t towlower(wint_t character)
{
    return wcrt_wide_byte(character) ? (wint_t)tolower((int)character) :
        character;
}

wint_t towupper(wint_t character)
{
    return wcrt_wide_byte(character) ? (wint_t)toupper((int)character) :
        character;
}

wctrans_t wctrans(const char *name)
{
    if (name == (const char *)0) return 0;
    if (strcmp(name, "tolower") == 0) return WCRT_WCTRANS_LOWER;
    if (strcmp(name, "toupper") == 0) return WCRT_WCTRANS_UPPER;
    return 0;
}

wint_t towctrans(wint_t character, wctrans_t descriptor)
{
    if (descriptor == WCRT_WCTRANS_LOWER) return towlower(character);
    if (descriptor == WCRT_WCTRANS_UPPER) return towupper(character);
    return character;
}
