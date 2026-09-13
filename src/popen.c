/** @file popen.c @brief Adapts shell command streams to WCRT stdio. */
#define WCRT_POSIX 1
#include <stdio.h>
#include "internal/stdlib.h"

FILE *_popen(const char *command, const char *mode)
{ return __wcrt_process_popen(command, mode); }
int _pclose(FILE *stream) { return __wcrt_process_pclose(stream); }
FILE *popen(const char *command, const char *mode)
{ return _popen(command, mode); }
int pclose(FILE *stream) { return _pclose(stream); }
