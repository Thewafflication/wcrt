/**
 * @file ctype.c
 * @brief Exercises C89 character handling over the complete byte domain.
 */

#include <ctype.h>

/** @brief Returns whether a value is a C-locale lowercase letter. */
static int expected_lower(int value)
{
    return value >= 'a' && value <= 'z';
}

/** @brief Returns whether a value is a C-locale uppercase letter. */
static int expected_upper(int value)
{
    return value >= 'A' && value <= 'Z';
}

/** @brief Returns whether a value is a C-locale decimal digit. */
static int expected_digit(int value)
{
    return value >= '0' && value <= '9';
}

/**
 * @brief Compares a classification result with an expected truth value.
 * @param actual Value returned by the function under test.
 * @param expected Expected Boolean result.
 * @return Nonzero when both values have the same truth value.
 */
static int same_truth(int actual, int expected)
{
    return (!actual) == (!expected);
}

/**
 * @brief Checks every classification and mapping for one input value.
 * @param value EOF or an unsigned-character value.
 * @return Zero on success or a test-specific failure code.
 */
static int check_value(int value)
{
    int lower = expected_lower(value);
    int upper = expected_upper(value);
    int digit = expected_digit(value);
    int alpha = lower || upper;
    int alnum = alpha || digit;
    int cntrl = (value >= 0 && value <= 0x1f) || value == 0x7f;
    int graph = value >= 0x21 && value <= 0x7e;
    int print = value >= 0x20 && value <= 0x7e;
    int space = value == ' ' || (value >= '\t' && value <= '\r');
    int punct = graph && !alnum;
    int xdigit = digit || (value >= 'a' && value <= 'f') ||
        (value >= 'A' && value <= 'F');
    int mapped_lower = upper ? value + ('a' - 'A') : value;
    int mapped_upper = lower ? value - ('a' - 'A') : value;

    if (!same_truth(isalnum(value), alnum)) return 1;
    if (!same_truth(isalpha(value), alpha)) return 2;
    if (!same_truth(iscntrl(value), cntrl)) return 3;
    if (!same_truth(isdigit(value), digit)) return 4;
    if (!same_truth(isgraph(value), graph)) return 5;
    if (!same_truth(islower(value), lower)) return 6;
    if (!same_truth(isprint(value), print)) return 7;
    if (!same_truth(ispunct(value), punct)) return 8;
    if (!same_truth(isspace(value), space)) return 9;
    if (!same_truth(isupper(value), upper)) return 10;
    if (!same_truth(isxdigit(value), xdigit)) return 11;
    if (tolower(value) != mapped_lower) return 12;
    if (toupper(value) != mapped_upper) return 13;
    return 0;
}

/**
 * @brief Runs full-domain C-locale character handling checks.
 * @return Zero on success or a test-specific failure code.
 */
int main(void)
{
    int value;
    int result = check_value(-1);
    if (result != 0) {
        return result;
    }
    for (value = 0; value <= 255; ++value) {
        result = check_value(value);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}
