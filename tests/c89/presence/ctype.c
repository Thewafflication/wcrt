/* REQ-0002: <ctype.h> presence. Compile only; do not link. */
#include <ctype.h>

typedef int (*wcrt_ctype_fn)(int);

static wcrt_ctype_fn wcrt_ctype_functions[] = {
    (isalnum), (isalpha), (iscntrl), (isdigit), (isgraph), (islower),
    (isprint), (ispunct), (isspace), (isupper), (isxdigit),
    (tolower), (toupper)
};

static int wcrt_ctype_function_count =
    (int)(sizeof(wcrt_ctype_functions) / sizeof(wcrt_ctype_functions[0]));
