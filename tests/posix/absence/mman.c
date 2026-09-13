/** @file mman.c @brief Verifies strict-profile mapping isolation. */
#include <sys/mman.h>
#ifdef MAP_FAILED
#error mapping declarations must require WCRT_POSIX
#endif
int main(void) { return 0; }
