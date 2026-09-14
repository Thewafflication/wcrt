/** @file thread_pool.c @brief Stress-tests the WCRT worker pool. */

#define WCRT_POSIX 1

#include <errno.h>
#include <pthread.h>
#include <wcrt/thread_pool.h>

#define WORKERS 4
#define TASKS 512

static pthread_mutex_t test_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ready_condition = PTHREAD_COND_INITIALIZER;
static pthread_cond_t gate_condition = PTHREAD_COND_INITIALIZER;
static int ready;
static int released;
static int completed;
static int initialized;
static pthread_once_t once_control = PTHREAD_ONCE_INIT;

static void initialize_once(void) { ++initialized; }

static void blocking_task(void *argument)
{
    (void)argument;
    pthread_mutex_lock(&test_mutex);
    ++ready;
    pthread_cond_signal(&ready_condition);
    while (!released)
        pthread_cond_wait(&gate_condition, &test_mutex);
    pthread_mutex_unlock(&test_mutex);
}

static void counting_task(void *argument)
{
    (void)argument;
    pthread_once(&once_control, initialize_once);
    pthread_mutex_lock(&test_mutex);
    ++completed;
    pthread_mutex_unlock(&test_mutex);
}

int main(void)
{
    wcrt_thread_pool *pool;
    int index;
    pool = wcrt_thread_pool_create(WORKERS);
    if (pool == NULL) return 1;
    for (index = 0; index < WORKERS; ++index)
        if (wcrt_thread_pool_submit(pool, blocking_task, NULL) != 0)
            return 2;
    pthread_mutex_lock(&test_mutex);
    while (ready != WORKERS)
        pthread_cond_wait(&ready_condition, &test_mutex);
    released = 1;
    pthread_cond_broadcast(&gate_condition);
    pthread_mutex_unlock(&test_mutex);
    for (index = 0; index < TASKS; ++index)
        if (wcrt_thread_pool_submit(pool, counting_task, NULL) != 0)
            return 3;
    if (wcrt_thread_pool_wait(pool) != 0 || completed != TASKS ||
        initialized != 1) return 4;
    if (wcrt_thread_pool_shutdown(pool) != 0) return 5;
    if (wcrt_thread_pool_shutdown(pool) != 0) return 6;
    if (wcrt_thread_pool_submit(pool, counting_task, NULL) != EBUSY)
        return 7;
    wcrt_thread_pool_destroy(pool);
    errno = 0;
    if (wcrt_thread_pool_create(0) != NULL || errno != EINVAL) return 8;
    pthread_cond_destroy(&gate_condition);
    pthread_cond_destroy(&ready_condition);
    pthread_mutex_destroy(&test_mutex);
    return 0;
}
