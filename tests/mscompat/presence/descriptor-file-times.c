/** @file descriptor-file-times.c @brief Checks explicit time signatures. */
#include <sys/utime.h>
#include <sys/utime.h>
#ifdef _futime
#error Generic descriptor time route must require explicit selection
#endif
int (__cdecl *descriptor_time32)(int, struct __utimbuf32 *) = _futime32;
int (__cdecl *descriptor_time64)(int, struct __utimbuf64 *) = _futime64;
