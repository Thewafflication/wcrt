/** @file popen.c @brief Checks strict command-stream isolation. */
#include <stdio.h>
static int popen;
static int pclose;
int main(void) { return popen + pclose; }
