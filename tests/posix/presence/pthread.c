/** @file pthread.c @brief Verifies selected POSIX thread declarations. */

#define WCRT_POSIX 1
#include <pthread.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_t thread;
int main(void) { return pthread_mutex_lock(&mutex); }
