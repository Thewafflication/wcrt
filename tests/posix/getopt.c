/** @file getopt.c @brief Verifies selected option parsing. */
#define WCRT_POSIX 1
#include <getopt.h>
#include <string.h>

static int test_short(void)
{
    char *arguments[] = {"tool", "-abvalue", "-c", "next", "operand"};
    optind = 1;
    if (getopt(5, arguments, "ab:c:") != 'a') return 1;
    if (getopt(5, arguments, "ab:c:") != 'b' ||
        strcmp(optarg, "value") != 0) return 2;
    if (getopt(5, arguments, "ab:c:") != 'c' ||
        strcmp(optarg, "next") != 0) return 3;
    if (getopt(5, arguments, "ab:c:") != -1 || optind != 4) return 4;
    return 0;
}

static int test_short_errors(void)
{
    char *arguments[] = {"tool", "-d", "-evalue", "-r"};
    optind = 1;
    if (getopt(4, arguments, ":d::e::r:") != 'd' || optarg != NULL)
        return 5;
    if (getopt(4, arguments, ":d::e::r:") != 'e' ||
        strcmp(optarg, "value") != 0) return 6;
    if (getopt(4, arguments, ":d::e::r:") != ':' || optopt != 'r')
        return 7;
    optind = 1;
    return getopt(4, arguments, ":d::e::r:") != 'd' ? 8 : 0;
}

static int test_long(void)
{
    int flag = 0;
    int index = -1;
    char *arguments[] = {"tool", "--output=file", "--ver", "--flag"};
    const struct option options[] = {
        {"output", required_argument, NULL, 'o'},
        {"verbose", no_argument, NULL, 'v'},
        {"version", no_argument, NULL, 'V'},
        {"flag", no_argument, &flag, 7},
        {NULL, 0, NULL, 0}
    };
    optind = 1;
    if (getopt_long(4, arguments, "", options, &index) != 'o' ||
        strcmp(optarg, "file") != 0 || index != 0) return 10;
    if (getopt_long(4, arguments, "", options, &index) != '?' ||
        optind != 3) return 11;
    if (getopt_long(4, arguments, "", options, &index) != 0 || flag != 7)
        return 12;
    return getopt_long(4, arguments, "", options, &index) != -1 ? 13 : 0;
}

int main(void)
{
    int result = test_short();
    if (result != 0) return result;
    result = test_short_errors();
    return result != 0 ? result : test_long();
}
