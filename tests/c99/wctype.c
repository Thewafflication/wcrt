/**
 * @file wctype.c
 * @brief Exercises C-locale wide classification, mappings, and descriptors.
 */
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <wctype.h>

struct wcrt_class_case {
    const char *name;
    int (*wide)(wint_t);
    int (*narrow)(int);
};

static int wcrt_isblank_narrow(int value)
{
    return value == ' ' || value == '\t';
}

static int wcrt_same_truth(int left, int right)
{
    return (!left) == (!right);
}

static int wcrt_test_classes(void)
{
    static const struct wcrt_class_case cases[] = {
        { "alnum", iswalnum, isalnum }, { "alpha", iswalpha, isalpha },
        { "blank", iswblank, wcrt_isblank_narrow },
        { "cntrl", iswcntrl, iscntrl }, { "digit", iswdigit, isdigit },
        { "graph", iswgraph, isgraph }, { "lower", iswlower, islower },
        { "print", iswprint, isprint }, { "punct", iswpunct, ispunct },
        { "space", iswspace, isspace }, { "upper", iswupper, isupper },
        { "xdigit", iswxdigit, isxdigit }
    };
    size_t index;
    int value;

    for (index = 0; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        wctype_t descriptor = wctype(cases[index].name);
        if (descriptor == (wctype_t)0) return 1;
        for (value = 0; value <= 0xff; ++value) {
            int expected = cases[index].narrow(value);
            if (!wcrt_same_truth(cases[index].wide((wint_t)value), expected))
                return 2;
            if (!wcrt_same_truth(iswctype((wint_t)value, descriptor), expected))
                return 3;
        }
        if (cases[index].wide(WEOF) || iswctype(WEOF, descriptor)) return 4;
        if (cases[index].wide((wint_t)0x100)) return 5;
    }
    return 0;
}

static int wcrt_test_names_and_mappings(void)
{
    wctrans_t lower = wctrans("tolower");
    wctrans_t upper = wctrans("toupper");
    int value;

    if (wctype((const char *)0) || wctype("") || wctype("Alpha") ||
        wctype("unknown")) return 10;
    if (lower == (wctrans_t)0 || upper == (wctrans_t)0) return 11;
    if (wctrans((const char *)0) || wctrans("") || wctrans("ToLower") ||
        wctrans("unknown")) return 12;
    for (value = 0; value <= 0xff; ++value) {
        if (towlower((wint_t)value) != (wint_t)tolower(value)) return 13;
        if (towupper((wint_t)value) != (wint_t)toupper(value)) return 14;
        if (towctrans((wint_t)value, lower) != towlower((wint_t)value))
            return 15;
        if (towctrans((wint_t)value, upper) != towupper((wint_t)value))
            return 16;
    }
    if (towlower(WEOF) != WEOF || towupper(WEOF) != WEOF) return 17;
    if (towlower((wint_t)0x100) != (wint_t)0x100 ||
        towupper((wint_t)0xd800) != (wint_t)0xd800) return 18;
    if (towctrans((wint_t)'A', (wctrans_t)0) != (wint_t)'A') return 19;
    if (iswctype((wint_t)'A', (wctype_t)0) != 0) return 20;
    return 0;
}

/** @brief Runs classification and mapping matrices. */
int main(void)
{
    int result;

    errno = 73;
    result = wcrt_test_classes();
    if (result != 0) return result;
    result = wcrt_test_names_and_mappings();
    if (result != 0) return result;
    if (errno != 73) return 30;
    return 0;
}
