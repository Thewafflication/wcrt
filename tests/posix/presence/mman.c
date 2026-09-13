/** @file mman.c @brief Verifies selected file mapping declarations. */
#define WCRT_POSIX 1
#include <sys/mman.h>
void *view = MAP_FAILED;
int main(void) { return PROT_READ == 1 && MAP_PRIVATE == 2 ? 0 : 1; }
