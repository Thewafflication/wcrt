/** @file pthread.c @brief Verifies strict-profile pthread isolation. */

#include <pthread.h>
#ifdef PTHREAD_MUTEX_INITIALIZER
#error pthread declarations must require WCRT_POSIX
#endif
int main(void) { return 0; }
