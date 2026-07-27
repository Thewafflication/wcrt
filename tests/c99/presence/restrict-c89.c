/**
 * @file restrict-c89.c
 * @brief Verifies that C99 restrict contracts disappear in C89 mode.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WCRT_STRING_INNER(value) #value
#define WCRT_STRING(value) WCRT_STRING_INNER(value)

typedef char wcrt_restrict_is_empty[
    sizeof(WCRT_STRING(WCRT_RESTRICT)) == 1 ? 1 : -1];
