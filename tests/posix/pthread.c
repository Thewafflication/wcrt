/** @file pthread.c @brief Exercises selected POSIX thread behavior. */

#define WCRT_POSIX 1
#include <errno.h>
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ready_condition = PTHREAD_COND_INITIALIZER;
static pthread_cond_t go_condition = PTHREAD_COND_INITIALIZER;
static int ready;
static int go;
static int completed;
static int try_result;
static int exited;
static int destructed;
static pthread_key_t key;

static void destroy_specific(void *value)
{
    if (value == (void *)123) ++destructed;
}

static void *try_worker(void *argument)
{
    (void)argument;
    try_result = pthread_mutex_trylock(&mutex);
    if (try_result == 0) pthread_mutex_unlock(&mutex);
    pthread_setspecific(key, argument);
    return argument;
}

static void *worker(void *argument)
{
    pthread_mutex_lock(&mutex);
    ++ready;
    pthread_cond_signal(&ready_condition);
    while (!go) pthread_cond_wait(&go_condition, &mutex);
    ++completed;
    pthread_mutex_unlock(&mutex);
    return argument;
}

static void *exit_worker(void *argument)
{
    pthread_mutex_lock(&mutex);
    exited = 1;
    pthread_cond_signal(&ready_condition);
    pthread_mutex_unlock(&mutex);
    pthread_exit(argument);
    return 0;
}

int main(void)
{
    pthread_t first;
    pthread_t second;
    pthread_t try_thread;
    pthread_t exit_thread;
    void *result = (void *)1;
    if (pthread_key_create(&key, destroy_specific) != 0) return 1;
    if (pthread_mutex_lock(&mutex) != 0) return 2;
    if (pthread_create(&try_thread, 0, try_worker, (void *)123) != 0)
        return 3;
    if (pthread_join(try_thread, &result) != 0 || try_result != EBUSY ||
        result != (void *)123 || destructed != 1) return 4;
    if (pthread_create(&first, 0, worker, 0) != 0) return 4;
    if (pthread_create(&second, 0, worker, 0) != 0) return 5;
    while (ready != 2) pthread_cond_wait(&ready_condition, &mutex);
    go = 1;
    if (pthread_cond_broadcast(&go_condition) != 0) return 6;
    pthread_mutex_unlock(&mutex);
    if (pthread_join(first, &result) != 0 || result != 0) return 7;
    if (pthread_join(second, 0) != 0) return 8;
    if (completed != 2) return 9;
    if (pthread_create(&exit_thread, 0, exit_worker, (void *)1) != 0)
        return 10;
    if (pthread_join(exit_thread, &result) != 0 || result != (void *)1)
        return 11;
    pthread_mutex_lock(&mutex);
    exited = 0;
    if (pthread_create(&exit_thread, 0, exit_worker, NULL) != 0) return 12;
    if (pthread_detach(exit_thread) != 0) return 13;
    while (!exited) pthread_cond_wait(&ready_condition, &mutex);
    pthread_mutex_unlock(&mutex);
    if (pthread_cond_destroy(&ready_condition) != 0) return 14;
    if (pthread_cond_destroy(&go_condition) != 0) return 15;
    if (pthread_mutex_destroy(&mutex) != 0) return 16;
    if (pthread_create(0, 0, worker, 0) != EINVAL) return 17;
    if (!pthread_equal(pthread_self(), pthread_self())) return 18;
    if (pthread_setspecific(key, (void *)7) != 0 ||
        pthread_getspecific(key) != (void *)7) return 19;
    if (pthread_setspecific(key, NULL) != 0 ||
        pthread_key_delete(key) != 0) return 20;
    return pthread_self() == 0 ? 21 : 0;
}
