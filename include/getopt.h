/** @file getopt.h @brief Declares selected command-line option parsing. */
#ifndef WCRT_GETOPT_H
#define WCRT_GETOPT_H

#if defined(WCRT_POSIX)
#define no_argument 0
#define required_argument 1
#define optional_argument 2

struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

#ifdef __cplusplus
extern "C" {
#endif

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

int getopt(int count, char *const arguments[], const char *options);
int getopt_long(int count, char *const arguments[], const char *options,
    const struct option *long_options, int *long_index);

#ifdef __cplusplus
}
#endif
#endif
#endif
