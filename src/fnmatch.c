/** @file fnmatch.c @brief Implements selected POSIX filename matching. */

#define WCRT_POSIX 1

#include <ctype.h>
#include <fnmatch.h>

/** @brief Compares characters with optional ASCII case folding. */
static int wcrt_fn_equal(int left, int right, int flags)
{
    if (flags & FNM_CASEFOLD) {
        left = tolower((unsigned char)left);
        right = tolower((unsigned char)right);
    }
    return left == right;
}

/** @brief Tests one bracket expression and advances its pattern cursor. */
static int wcrt_fn_bracket(const char **cursor, int character, int flags)
{
    const char *pattern = *cursor;
    int negate = *pattern == '!' || *pattern == '^';
    int matched = 0;
    int previous = -1;
    if (negate) ++pattern;
    if (*pattern == ']') {
        matched = character == ']';
        previous = ']';
        ++pattern;
    }
    while (*pattern != '\0' && *pattern != ']') {
        int current = (unsigned char)*pattern++;
        if (current == '\\' && !(flags & FNM_NOESCAPE) && *pattern != '\0')
            current = (unsigned char)*pattern++;
        if (current == '-' && previous >= 0 && *pattern != ']' &&
            *pattern != '\0') {
            int end = (unsigned char)*pattern++;
            int value = character;
            if (flags & FNM_CASEFOLD) {
                previous = tolower((unsigned char)previous);
                end = tolower((unsigned char)end);
                value = tolower((unsigned char)value);
            }
            if (value >= previous && value <= end) matched = 1;
            previous = -1;
        } else {
            if (wcrt_fn_equal(current, character, flags)) matched = 1;
            previous = current;
        }
    }
    if (*pattern != ']') return -1;
    *cursor = pattern + 1;
    return negate ? !matched : matched;
}

/** @brief Recursively matches one pattern suffix. */
static int wcrt_fn_match(const char *pattern, const char *string, int flags,
    int component_start)
{
    while (*pattern != '\0') {
        int token = (unsigned char)*pattern++;
        if (token == '*') {
            while (*pattern == '*') ++pattern;
            if ((flags & FNM_PERIOD) && component_start && *string == '.')
                return 0;
            if (*pattern == '\0') {
                if (!(flags & FNM_PATHNAME)) return 1;
                while (*string != '\0') {
                    if (*string++ == '/') return 0;
                }
                return 1;
            }
            for (;;) {
                if (wcrt_fn_match(pattern, string, flags, component_start))
                    return 1;
                if (*string == '\0' ||
                    ((flags & FNM_PATHNAME) && *string == '/')) break;
                component_start = *string == '/';
                ++string;
            }
            return 0;
        }
        if (*string == '\0') return 0;
        if ((flags & FNM_PERIOD) && component_start && *string == '.' &&
            (token == '?' || token == '[')) return 0;
        if (token == '?') {
            if ((flags & FNM_PATHNAME) && *string == '/') return 0;
            ++string;
        } else if (token == '[') {
            int bracket = wcrt_fn_bracket(&pattern,
                (unsigned char)*string, flags);
            if ((flags & FNM_PATHNAME) && *string == '/') return 0;
            if (bracket < 0) {
                if (*string != '[') return 0;
            } else if (!bracket) return 0;
            ++string;
        } else {
            if (token == '\\' && !(flags & FNM_NOESCAPE) && *pattern != '\0')
                token = (unsigned char)*pattern++;
            if (!wcrt_fn_equal(token, (unsigned char)*string, flags)) return 0;
            ++string;
        }
        component_start = token == '/';
    }
    return *string == '\0';
}

int fnmatch(const char *pattern, const char *string, int flags)
{
    if (pattern == 0 || string == 0) return FNM_NOMATCH;
    return wcrt_fn_match(pattern, string, flags, 1) ? 0 : FNM_NOMATCH;
}
