/** @file pthread.c @brief Implements the selected POSIX thread API. */

#define WCRT_POSIX 1

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_INFINITE 0xffffffffUL
#define WCRT_WAIT_OBJECT_0 0UL
#define WCRT_SYNC_READY 2L

__declspec(dllimport) void WCRT_WINAPI Sleep(unsigned long milliseconds);
__declspec(dllimport) void WCRT_WINAPI InitializeCriticalSection(void *section);
__declspec(dllimport) void WCRT_WINAPI DeleteCriticalSection(void *section);
__declspec(dllimport) void WCRT_WINAPI EnterCriticalSection(void *section);
__declspec(dllimport) int WCRT_WINAPI TryEnterCriticalSection(void *section);
__declspec(dllimport) void WCRT_WINAPI LeaveCriticalSection(void *section);
__declspec(dllimport) void *WCRT_WINAPI CreateSemaphoreA(void *attributes,
    long initial_count, long maximum_count, const char *name);
__declspec(dllimport) void *WCRT_WINAPI CreateEventA(void *attributes,
    int manual_reset, int initial_state, const char *name);
__declspec(dllimport) void *WCRT_WINAPI CreateMutexA(void *attributes,
    int initial_owner, const char *name);
__declspec(dllimport) int WCRT_WINAPI ReleaseSemaphore(void *semaphore,
    long release_count, long *previous_count);
__declspec(dllimport) int WCRT_WINAPI SetEvent(void *event);
__declspec(dllimport) int WCRT_WINAPI ReleaseMutex(void *mutex);
__declspec(dllimport) unsigned long WCRT_WINAPI WaitForSingleObject(
    void *handle, unsigned long milliseconds);
__declspec(dllimport) int WCRT_WINAPI CloseHandle(void *handle);
__declspec(dllimport) void *WCRT_WINAPI CreateThread(void *attributes,
    size_t stack_size, unsigned long (WCRT_WINAPI *entry)(void *),
    void *argument, unsigned long flags, unsigned long *identifier);
__declspec(dllimport) void WCRT_WINAPI ExitThread(unsigned long status);
__declspec(dllimport) void *WCRT_WINAPI GetCurrentThread(void);
__declspec(dllimport) unsigned long WCRT_WINAPI GetCurrentProcessId(void);

struct wcrt_thread_start {
    void *(*routine)(void *);
    void *argument;
};

static void *wcrt_section(void *storage) { return storage; }

static void *wcrt_acquire_initialization_lock(void)
{
    char name[64];
    void *lock;
    sprintf(name, "WCRT.pthread.init.%lu", GetCurrentProcessId());
    lock = CreateMutexA(NULL, 0, name);
    if (lock == NULL) return NULL;
    if (WaitForSingleObject(lock, WCRT_INFINITE) != WCRT_WAIT_OBJECT_0) {
        CloseHandle(lock);
        return NULL;
    }
    return lock;
}

static void wcrt_release_initialization_lock(void *lock)
{
    ReleaseMutex(lock);
    CloseHandle(lock);
}

static int wcrt_ensure_mutex(pthread_mutex_t *mutex)
{
    void *lock;
    if (mutex == NULL) return EINVAL;
    if (mutex->state == WCRT_SYNC_READY) return 0;
    lock = wcrt_acquire_initialization_lock();
    if (lock == NULL) return ENOMEM;
    if (mutex->state != WCRT_SYNC_READY) {
        InitializeCriticalSection(wcrt_section(mutex->storage));
        mutex->state = WCRT_SYNC_READY;
    }
    wcrt_release_initialization_lock(lock);
    return 0;
}

static int wcrt_ensure_condition(pthread_cond_t *condition)
{
    void *lock;
    if (condition == NULL) return EINVAL;
    if (condition->state == WCRT_SYNC_READY) return 0;
    lock = wcrt_acquire_initialization_lock();
    if (lock == NULL) return ENOMEM;
    if (condition->state != WCRT_SYNC_READY) {
        condition->waiters = 0;
        condition->generation = 0;
        condition->broadcast_remaining = 0;
        condition->broadcasting = 0;
        condition->semaphore = CreateSemaphoreA(NULL, 0, 0x7fffffffL, NULL);
        condition->waiters_done = CreateEventA(NULL, 0, 0, NULL);
        if (condition->semaphore == NULL || condition->waiters_done == NULL) {
            if (condition->semaphore != NULL) CloseHandle(condition->semaphore);
            if (condition->waiters_done != NULL)
                CloseHandle(condition->waiters_done);
            condition->semaphore = NULL;
            condition->waiters_done = NULL;
            condition->state = 0;
            wcrt_release_initialization_lock(lock);
            return ENOMEM;
        }
        InitializeCriticalSection(wcrt_section(condition->storage));
        condition->state = WCRT_SYNC_READY;
    }
    wcrt_release_initialization_lock(lock);
    return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex,
    const pthread_mutexattr_t *attributes)
{
    (void)attributes;
    if (mutex == NULL) return EINVAL;
    mutex->state = 0;
    return wcrt_ensure_mutex(mutex);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    if (mutex == NULL || mutex->state != WCRT_SYNC_READY) return EINVAL;
    DeleteCriticalSection(wcrt_section(mutex->storage));
    mutex->state = 0;
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    int status = wcrt_ensure_mutex(mutex);
    if (status != 0) return status;
    EnterCriticalSection(wcrt_section(mutex->storage));
    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    int status = wcrt_ensure_mutex(mutex);
    if (status != 0) return status;
    return TryEnterCriticalSection(wcrt_section(mutex->storage)) ? 0 : EBUSY;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    if (mutex == NULL || mutex->state != WCRT_SYNC_READY) return EINVAL;
    LeaveCriticalSection(wcrt_section(mutex->storage));
    return 0;
}

int pthread_cond_init(pthread_cond_t *condition,
    const pthread_condattr_t *attributes)
{
    (void)attributes;
    if (condition == NULL) return EINVAL;
    condition->state = 0;
    return wcrt_ensure_condition(condition);
}

int pthread_cond_destroy(pthread_cond_t *condition)
{
    if (condition == NULL || condition->state != WCRT_SYNC_READY)
        return EINVAL;
    DeleteCriticalSection(wcrt_section(condition->storage));
    CloseHandle(condition->semaphore);
    CloseHandle(condition->waiters_done);
    condition->state = 0;
    return 0;
}

int pthread_cond_wait(pthread_cond_t *condition, pthread_mutex_t *mutex)
{
    int status = wcrt_ensure_condition(condition);
    int last_waiter;
    unsigned long generation;
    if (status != 0) return status;
    status = wcrt_ensure_mutex(mutex);
    if (status != 0) return status;
    EnterCriticalSection(wcrt_section(condition->storage));
    ++condition->waiters;
    generation = condition->generation;
    LeaveCriticalSection(wcrt_section(condition->storage));
    LeaveCriticalSection(wcrt_section(mutex->storage));
    for (;;) {
        if (WaitForSingleObject(condition->semaphore, WCRT_INFINITE) !=
            WCRT_WAIT_OBJECT_0) return EINVAL;
        EnterCriticalSection(wcrt_section(condition->storage));
        if (generation != condition->generation) break;
        ReleaseSemaphore(condition->semaphore, 1, NULL);
        LeaveCriticalSection(wcrt_section(condition->storage));
        Sleep(0);
    }
    --condition->waiters;
    last_waiter = condition->broadcasting &&
        --condition->broadcast_remaining == 0;
    LeaveCriticalSection(wcrt_section(condition->storage));
    if (last_waiter) SetEvent(condition->waiters_done);
    EnterCriticalSection(wcrt_section(mutex->storage));
    return 0;
}

int pthread_cond_signal(pthread_cond_t *condition)
{
    int status = wcrt_ensure_condition(condition);
    int have_waiters;
    if (status != 0) return status;
    EnterCriticalSection(wcrt_section(condition->storage));
    have_waiters = condition->waiters > 0;
    if (have_waiters) ++condition->generation;
    LeaveCriticalSection(wcrt_section(condition->storage));
    if (have_waiters && !ReleaseSemaphore(condition->semaphore, 1, NULL))
        return EINVAL;
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *condition)
{
    int status = wcrt_ensure_condition(condition);
    long waiters;
    if (status != 0) return status;
    EnterCriticalSection(wcrt_section(condition->storage));
    waiters = condition->waiters;
    if (waiters == 0) {
        LeaveCriticalSection(wcrt_section(condition->storage));
        return 0;
    }
    condition->broadcasting = 1;
    condition->broadcast_remaining = waiters;
    ++condition->generation;
    if (!ReleaseSemaphore(condition->semaphore, waiters, NULL)) {
        condition->broadcasting = 0;
        LeaveCriticalSection(wcrt_section(condition->storage));
        return EINVAL;
    }
    LeaveCriticalSection(wcrt_section(condition->storage));
    if (WaitForSingleObject(condition->waiters_done, WCRT_INFINITE) !=
        WCRT_WAIT_OBJECT_0) return EINVAL;
    EnterCriticalSection(wcrt_section(condition->storage));
    condition->broadcasting = 0;
    LeaveCriticalSection(wcrt_section(condition->storage));
    return 0;
}

static unsigned long WCRT_WINAPI wcrt_thread_entry(void *parameter)
{
    struct wcrt_thread_start start = *(struct wcrt_thread_start *)parameter;
    free(parameter);
    start.routine(start.argument);
    return 0;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attributes,
    void *(*start_routine)(void *), void *argument)
{
    struct wcrt_thread_start *start;
    (void)attributes;
    if (thread == NULL || start_routine == NULL) return EINVAL;
    start = (struct wcrt_thread_start *)malloc(sizeof(*start));
    if (start == NULL) return ENOMEM;
    start->routine = start_routine;
    start->argument = argument;
    *thread = CreateThread(NULL, 0, wcrt_thread_entry, start, 0, NULL);
    if (*thread == NULL) {
        free(start);
        return EAGAIN;
    }
    return 0;
}

int pthread_join(pthread_t thread, void **result)
{
    if (thread == NULL) return EINVAL;
    if (WaitForSingleObject(thread, WCRT_INFINITE) != WCRT_WAIT_OBJECT_0)
        return EINVAL;
    if (result != NULL) *result = NULL;
    return CloseHandle(thread) ? 0 : EINVAL;
}

int pthread_detach(pthread_t thread)
{
    if (thread == NULL) return EINVAL;
    return CloseHandle(thread) ? 0 : EINVAL;
}

void pthread_exit(void *result)
{
    (void)result;
    ExitThread(0);
}

pthread_t pthread_self(void) { return GetCurrentThread(); }
