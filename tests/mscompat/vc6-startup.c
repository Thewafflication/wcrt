/**
 * @file vc6-startup.c
 * @brief Starts an explicit DLL consumer without a Microsoft CRT dependency.
 */

#include <stdlib.h>

/** @brief Link-only floating-point marker required by Microsoft C objects. */
int _fltused;

int main(void);

/** @brief Returns the consumer test result through the WCRT exit function. */
void mainCRTStartup(void)
{
    exit(main());
}
