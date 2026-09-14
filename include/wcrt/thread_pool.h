/** @file thread_pool.h @brief Declares the WCRT worker thread pool. */

#ifndef WCRT_THREAD_POOL_H
#define WCRT_THREAD_POOL_H

#include <stddef.h>

/** @brief Opaque fixed-size worker pool. */
typedef struct wcrt_thread_pool wcrt_thread_pool;

/** @brief Work function executed by a pool worker. */
typedef void (*wcrt_thread_pool_task)(void *argument);

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Creates a pool containing worker_count worker threads. */
wcrt_thread_pool *wcrt_thread_pool_create(size_t worker_count);

/** @brief Queues one task for execution. */
int wcrt_thread_pool_submit(wcrt_thread_pool *pool,
    wcrt_thread_pool_task task, void *argument);

/** @brief Waits until all work submitted so far has completed. */
int wcrt_thread_pool_wait(wcrt_thread_pool *pool);

/** @brief Drains work, stops all workers, and joins their threads. */
int wcrt_thread_pool_shutdown(wcrt_thread_pool *pool);

/** @brief Shuts down and releases a pool. */
void wcrt_thread_pool_destroy(wcrt_thread_pool *pool);

#ifdef __cplusplus
}
#endif

#endif
