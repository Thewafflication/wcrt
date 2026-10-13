/** @file descriptor-create.c @brief Checks the legacy creation signature. */
#include <io.h>
#include <io.h>
int (*create_descriptor)(const char *, int) = _creat;
