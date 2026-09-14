/** @file thread_pool.c @brief Verifies the WCRT thread-pool declarations. */

#include <wcrt/thread_pool.h>

static void task(void *argument) { (void)argument; }

int main(void)
{
    wcrt_thread_pool *pool = wcrt_thread_pool_create(1);
    return pool == NULL ? 0 : wcrt_thread_pool_submit(pool, task, NULL);
}
