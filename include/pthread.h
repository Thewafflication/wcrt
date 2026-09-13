/** @file pthread.h @brief Declares the WCRT POSIX thread subset. */

#ifndef WCRT_PTHREAD_H
#define WCRT_PTHREAD_H

#if defined(WCRT_POSIX)

#include <stddef.h>

typedef void *pthread_t;

typedef struct {
    volatile long state;
    void *storage[8];
} pthread_mutex_t;

typedef struct {
    volatile long state;
    void *storage[8];
    long waiters;
    unsigned long generation;
    long broadcast_remaining;
    void *semaphore;
    void *waiters_done;
    int broadcasting;
} pthread_cond_t;

typedef struct { int unused; } pthread_mutexattr_t;
typedef struct { int unused; } pthread_condattr_t;
typedef struct { int unused; } pthread_attr_t;

#define PTHREAD_MUTEX_INITIALIZER { 0, { 0 } }
#define PTHREAD_COND_INITIALIZER { 0, { 0 }, 0, 0, 0, NULL, NULL, 0 }

#ifdef __cplusplus
extern "C" {
#endif

int pthread_mutex_init(pthread_mutex_t *mutex,
    const pthread_mutexattr_t *attributes);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_cond_init(pthread_cond_t *condition,
    const pthread_condattr_t *attributes);
int pthread_cond_destroy(pthread_cond_t *condition);
int pthread_cond_wait(pthread_cond_t *condition, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *condition);
int pthread_cond_broadcast(pthread_cond_t *condition);
int pthread_create(pthread_t *thread, const pthread_attr_t *attributes,
    void *(*start_routine)(void *), void *argument);
int pthread_join(pthread_t thread, void **result);
int pthread_detach(pthread_t thread);
void pthread_exit(void *result);
pthread_t pthread_self(void);

#ifdef __cplusplus
}
#endif

#endif
#endif
