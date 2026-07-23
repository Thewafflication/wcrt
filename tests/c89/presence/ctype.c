/**
 * @file ctype.c
 * @brief Verifies the compile-time C89 ctype.h interface.
 * @details File-scope declarations are typed API presence probes.
 */
#include <ctype.h>

typedef int (*wcrt_ctype_fn)(int);

static wcrt_ctype_fn wcrt_ctype_functions[] = {
    (isalnum), (isalpha), (iscntrl), (isdigit), (isgraph), (islower),
    (isprint), (ispunct), (isspace), (isupper), (isxdigit),
    (tolower), (toupper)
};

static int wcrt_ctype_function_count =
    (int)(sizeof(wcrt_ctype_functions) / sizeof(wcrt_ctype_functions[0]));
