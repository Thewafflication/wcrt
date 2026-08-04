/**
 * @file stdlib-c89.c
 * @brief Verifies that C99 general utilities are absent in C89 mode.
 */
#include <stdlib.h>

static int lldiv_t;
static int atoll;
static int llabs;
static int lldiv;
static int strtof;
static int strtold;
static int strtoll;
static int strtoull;
