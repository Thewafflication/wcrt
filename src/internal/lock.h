/**
 * @file lock.h
 * @brief Declares the internal WCRT serialization primitive.
 */

#ifndef WCRT_INTERNAL_LOCK_H
#define WCRT_INTERNAL_LOCK_H

/**
 * @brief Mutual-exclusion lock initialized on first acquisition.
 *
 * The structure is designed for objects with static storage duration, whose
 * zero initialization is a valid unacquired state. No explicit initialization
 * call is required and none is provided. The lock is compiled in every WCRT
 * configuration and does not depend on the selected POSIX thread surface, so
 * library code below `<pthread.h>` may use it.
 *
 * The lock is not recursive. A thread that already holds the lock shall not
 * acquire it again.
 */
struct wcrt_lock {
    volatile long state;  /**< Nonzero once the backing section exists. */
    void *storage[8];     /**< Opaque storage for the backing section. */
};

/**
 * @brief Acquires @p lock, initializing it on first use.
 *
 * Blocks until the calling thread owns the lock. First use allocates the
 * backing synchronization object; concurrent first uses initialize exactly
 * once.
 *
 * @param lock Lock to acquire. A null pointer is rejected.
 * @return Zero when the lock is held, or -1 when the backing object could not
 *     be created. Acquisition cannot fail once it has succeeded once for a
 *     given lock, so a release path that follows a successful acquisition
 *     never observes failure.
 */
int __wcrt_lock_acquire(struct wcrt_lock *lock);

/**
 * @brief Releases @p lock previously acquired by the calling thread.
 *
 * @param lock Lock to release. Releasing a lock that is not held by the
 *     calling thread is undefined.
 */
void __wcrt_lock_release(struct wcrt_lock *lock);

#endif
