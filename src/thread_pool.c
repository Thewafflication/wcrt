/** @file thread_pool.c @brief Implements the WCRT worker thread pool. */

#define WCRT_POSIX 1

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <wcrt/thread_pool.h>

/** @brief One queued unit of work. */
struct wcrt_thread_pool_item {
    wcrt_thread_pool_task task;
    void *argument;
    struct wcrt_thread_pool_item *next;
};

struct wcrt_thread_pool {
    pthread_t *threads;
    size_t thread_count;
    struct wcrt_thread_pool_item *head;
    struct wcrt_thread_pool_item *tail;
    size_t queued;
    size_t active;
    int accepting;
    int stopping;
    int joining;
    int joined;
    pthread_mutex_t mutex;
    pthread_cond_t work_available;
    pthread_cond_t idle;
};

static void *wcrt_thread_pool_worker(void *argument)
{
    wcrt_thread_pool *pool = (wcrt_thread_pool *)argument;
    for (;;) {
        struct wcrt_thread_pool_item *item;
        pthread_mutex_lock(&pool->mutex);
        while (pool->head == NULL && !pool->stopping)
            pthread_cond_wait(&pool->work_available, &pool->mutex);
        if (pool->head == NULL && pool->stopping) {
            pthread_mutex_unlock(&pool->mutex);
            return NULL;
        }
        item = pool->head;
        pool->head = item->next;
        if (pool->head == NULL) pool->tail = NULL;
        --pool->queued;
        ++pool->active;
        pthread_mutex_unlock(&pool->mutex);

        item->task(item->argument);
        free(item);

        pthread_mutex_lock(&pool->mutex);
        --pool->active;
        if (pool->queued == 0 && pool->active == 0)
            pthread_cond_broadcast(&pool->idle);
        pthread_mutex_unlock(&pool->mutex);
    }
}

static void wcrt_thread_pool_stop_created(wcrt_thread_pool *pool,
    size_t created)
{
    size_t index;
    pthread_mutex_lock(&pool->mutex);
    pool->accepting = 0;
    pool->stopping = 1;
    pthread_cond_broadcast(&pool->work_available);
    pthread_mutex_unlock(&pool->mutex);
    for (index = 0; index < created; ++index)
        pthread_join(pool->threads[index], NULL);
}

wcrt_thread_pool *wcrt_thread_pool_create(size_t worker_count)
{
    wcrt_thread_pool *pool;
    size_t created;
    if (worker_count == 0) {
        errno = EINVAL;
        return NULL;
    }
    pool = (wcrt_thread_pool *)calloc(1, sizeof(*pool));
    if (pool == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    pool->threads = (pthread_t *)calloc(worker_count,
        sizeof(*pool->threads));
    if (pool->threads == NULL) {
        free(pool);
        errno = ENOMEM;
        return NULL;
    }
    pool->thread_count = worker_count;
    pool->accepting = 1;
    if (pthread_mutex_init(&pool->mutex, NULL) != 0) {
        free(pool->threads);
        free(pool);
        errno = ENOMEM;
        return NULL;
    }
    if (pthread_cond_init(&pool->work_available, NULL) != 0) {
        pthread_mutex_destroy(&pool->mutex);
        free(pool->threads);
        free(pool);
        errno = ENOMEM;
        return NULL;
    }
    if (pthread_cond_init(&pool->idle, NULL) != 0) {
        pthread_cond_destroy(&pool->work_available);
        pthread_mutex_destroy(&pool->mutex);
        free(pool->threads);
        free(pool);
        errno = ENOMEM;
        return NULL;
    }
    for (created = 0; created < worker_count; ++created) {
        if (pthread_create(&pool->threads[created], NULL,
            wcrt_thread_pool_worker, pool) != 0) {
            wcrt_thread_pool_stop_created(pool, created);
            pthread_cond_destroy(&pool->idle);
            pthread_cond_destroy(&pool->work_available);
            pthread_mutex_destroy(&pool->mutex);
            free(pool->threads);
            free(pool);
            errno = EAGAIN;
            return NULL;
        }
    }
    return pool;
}

int wcrt_thread_pool_submit(wcrt_thread_pool *pool,
    wcrt_thread_pool_task task, void *argument)
{
    struct wcrt_thread_pool_item *item;
    if (pool == NULL || task == NULL) return EINVAL;
    item = (struct wcrt_thread_pool_item *)malloc(sizeof(*item));
    if (item == NULL) return ENOMEM;
    item->task = task;
    item->argument = argument;
    item->next = NULL;
    pthread_mutex_lock(&pool->mutex);
    if (!pool->accepting) {
        pthread_mutex_unlock(&pool->mutex);
        free(item);
        return EBUSY;
    }
    if (pool->tail == NULL) pool->head = item;
    else pool->tail->next = item;
    pool->tail = item;
    ++pool->queued;
    pthread_cond_signal(&pool->work_available);
    pthread_mutex_unlock(&pool->mutex);
    return 0;
}

int wcrt_thread_pool_wait(wcrt_thread_pool *pool)
{
    if (pool == NULL) return EINVAL;
    pthread_mutex_lock(&pool->mutex);
    while (pool->queued != 0 || pool->active != 0)
        pthread_cond_wait(&pool->idle, &pool->mutex);
    pthread_mutex_unlock(&pool->mutex);
    return 0;
}

int wcrt_thread_pool_shutdown(wcrt_thread_pool *pool)
{
    size_t index;
    if (pool == NULL) return EINVAL;
    pthread_mutex_lock(&pool->mutex);
    if (pool->joined) {
        pthread_mutex_unlock(&pool->mutex);
        return 0;
    }
    if (pool->joining) {
        while (!pool->joined)
            pthread_cond_wait(&pool->idle, &pool->mutex);
        pthread_mutex_unlock(&pool->mutex);
        return 0;
    }
    pool->joining = 1;
    pool->accepting = 0;
    while (pool->queued != 0 || pool->active != 0)
        pthread_cond_wait(&pool->idle, &pool->mutex);
    pool->stopping = 1;
    pthread_cond_broadcast(&pool->work_available);
    pthread_mutex_unlock(&pool->mutex);
    for (index = 0; index < pool->thread_count; ++index)
        pthread_join(pool->threads[index], NULL);
    pthread_mutex_lock(&pool->mutex);
    pool->joined = 1;
    pthread_cond_broadcast(&pool->idle);
    pthread_mutex_unlock(&pool->mutex);
    return 0;
}

void wcrt_thread_pool_destroy(wcrt_thread_pool *pool)
{
    if (pool == NULL) return;
    wcrt_thread_pool_shutdown(pool);
    pthread_cond_destroy(&pool->idle);
    pthread_cond_destroy(&pool->work_available);
    pthread_mutex_destroy(&pool->mutex);
    free(pool->threads);
    free(pool);
}
