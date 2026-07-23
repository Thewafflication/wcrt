/**
 * @file stdlib.h
 * @brief Declares Windows allocation and process backends.
 */

#ifndef WCRT_INTERNAL_STDLIB_H
#define WCRT_INTERNAL_STDLIB_H

#include <stddef.h>

void *__wcrt_heap_allocate(size_t size, int zeroed);
void *__wcrt_heap_reallocate(void *memory, size_t size);
void __wcrt_heap_free(void *memory);
void __wcrt_process_exit(unsigned int status);
char *__wcrt_process_getenv(const char *name);
int __wcrt_process_system(const char *command);

#endif
