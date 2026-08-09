/**
 * @file stdio-t2-c89.c
 * @brief Verifies that C99 formatted-I/O additions are absent in C89 mode.
 */

#include <stdio.h>

static int snprintf;
static int vsnprintf;
static int vfscanf;
static int vscanf;
static int vsscanf;

/** @brief References the absence-probe objects. */
static void wcrt_use_absence_probes(void)
{
    int *addresses[] = {
        &snprintf, &vsnprintf, &vfscanf, &vscanf, &vsscanf
    };
    (void)addresses;
}
