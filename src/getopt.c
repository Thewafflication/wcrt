/** @file getopt.c @brief Implements selected command-line option parsing. */

#define WCRT_POSIX 1

#include <getopt.h>
#include <stddef.h>
#include <string.h>

char *optarg;
int optind = 1;
int opterr = 1;
int optopt;

/** @brief Remaining characters in the current short-option cluster. */
static const char *wcrt_option_cursor;

/** @brief Resets parser state when requested through optind. */
static void wcrt_getopt_prepare(void)
{
    if (optind <= 0) optind = 1;
    if (optind == 1) wcrt_option_cursor = NULL;
    optarg = NULL;
}

/** @brief Parses one short option or returns -1 at the first operand. */
static int wcrt_getopt_short(int count, char *const arguments[],
    const char *options)
{
    const char *declaration;
    int option;
    if (wcrt_option_cursor == NULL || *wcrt_option_cursor == '\0') {
        const char *argument;
        if (optind >= count || arguments[optind] == NULL) return -1;
        argument = arguments[optind];
        if (argument[0] != '-' || argument[1] == '\0') return -1;
        if (argument[1] == '-' && argument[2] == '\0') {
            ++optind;
            return -1;
        }
        wcrt_option_cursor = argument + 1;
        ++optind;
    }
    option = (unsigned char)*wcrt_option_cursor++;
    declaration = strchr(options, option);
    if (option == ':' || declaration == NULL) {
        optopt = option;
        if (*wcrt_option_cursor == '\0') wcrt_option_cursor = NULL;
        return '?';
    }
    if (declaration[1] == ':') {
        int optional = declaration[2] == ':';
        if (*wcrt_option_cursor != '\0') {
            optarg = (char *)wcrt_option_cursor;
            wcrt_option_cursor = NULL;
        } else if (!optional && optind < count) {
            optarg = arguments[optind++];
            wcrt_option_cursor = NULL;
        } else if (!optional) {
            optopt = option;
            wcrt_option_cursor = NULL;
            return options[0] == ':' ? ':' : '?';
        } else {
            wcrt_option_cursor = NULL;
        }
    } else if (*wcrt_option_cursor == '\0') {
        wcrt_option_cursor = NULL;
    }
    return option;
}

int getopt(int count, char *const arguments[], const char *options)
{
    if (arguments == NULL || options == NULL) return -1;
    wcrt_getopt_prepare();
    return wcrt_getopt_short(count, arguments, options);
}

/** @brief Locates one exact or uniquely abbreviated long option. */
static int wcrt_long_index(const char *name, size_t length,
    const struct option *options)
{
    int index;
    int match = -1;
    for (index = 0; options[index].name != NULL; ++index) {
        if (strncmp(options[index].name, name, length) != 0) continue;
        if (options[index].name[length] == '\0') return index;
        if (match >= 0) match = -2;
        else if (match == -1) match = index;
    }
    return match;
}

int getopt_long(int count, char *const arguments[], const char *options,
    const struct option *long_options, int *long_index)
{
    const char *argument;
    const char *equal;
    size_t length;
    int index;
    wcrt_getopt_prepare();
    if (arguments == NULL || options == NULL || long_options == NULL)
        return -1;
    if (wcrt_option_cursor != NULL) return wcrt_getopt_short(count,
        arguments, options);
    if (optind >= count || arguments[optind] == NULL) return -1;
    argument = arguments[optind];
    if (argument[0] != '-' || argument[1] != '-' || argument[2] == '\0')
        return wcrt_getopt_short(count, arguments, options);
    equal = strchr(argument + 2, '=');
    length = equal == NULL ? strlen(argument + 2) :
        (size_t)(equal - (argument + 2));
    index = wcrt_long_index(argument + 2, length, long_options);
    ++optind;
    if (index < 0) return '?';
    if (long_index != NULL) *long_index = index;
    if (long_options[index].has_arg == no_argument) {
        if (equal != NULL) return '?';
    } else if (equal != NULL) {
        optarg = (char *)(equal + 1);
    } else if (long_options[index].has_arg == required_argument) {
        if (optind >= count) {
            optopt = long_options[index].val;
            return options[0] == ':' ? ':' : '?';
        }
        optarg = arguments[optind++];
    }
    if (long_options[index].flag != NULL) {
        *long_options[index].flag = long_options[index].val;
        return 0;
    }
    return long_options[index].val;
}
