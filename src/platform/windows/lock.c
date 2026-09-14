/**
 * @file lock.c
 * @brief Implements the internal WCRT lock with Windows 2000 primitives.
 */

#include <stddef.h>

#include "../../internal/lock.h"

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

#define WCRT_INFINITE 0xffffffffUL
#define WCRT_WAIT_OBJECT_0 0UL
#define WCRT_LOCK_READY 2L

__declspec(dllimport) void WCRT_WINAPI InitializeCriticalSection(void *section);
__declspec(dllimport) void WCRT_WINAPI EnterCriticalSection(void *section);
__declspec(dllimport) void WCRT_WINAPI LeaveCriticalSection(void *section);
__declspec(dllimport) void *WCRT_WINAPI CreateMutexA(void *attributes,
    int initial_owner, const char *name);
__declspec(dllimport) int WCRT_WINAPI ReleaseMutex(void *mutex);
__declspec(dllimport) unsigned long WCRT_WINAPI WaitForSingleObject(
    void *handle, unsigned long milliseconds);
__declspec(dllimport) int WCRT_WINAPI CloseHandle(void *handle);
__declspec(dllimport) unsigned long WCRT_WINAPI GetCurrentProcessId(void);

/**
 * @brief Writes the process-scoped initialization mutex name into @p name.
 *
 * The name is composed without formatted output so that the lock remains
 * usable by the stream layer that formatted output itself depends upon.
 *
 * @param name Buffer of at least 32 characters receiving the name.
 */
static void wcrt_lock_initializer_name(char *name)
{
    static const char prefix[] = "WCRT.lock.init.";
    unsigned long value = GetCurrentProcessId();
    char digits[16];
    int count = 0;
    int index = 0;

    while (prefix[index] != '\0') {
        name[index] = prefix[index];
        ++index;
    }
    do {
        digits[count] = (char)('0' + (int)(value % 10UL));
        ++count;
        value /= 10UL;
    } while (value != 0UL);
    while (count > 0) {
        --count;
        name[index] = digits[count];
        ++index;
    }
    name[index] = '\0';
}

/**
 * @brief Acquires the process-wide lock that serializes first use.
 *
 * @return An owned initialization handle, or a null pointer on failure.
 */
static void *wcrt_lock_hold_initializer(void)
{
    char name[32];
    void *guard;

    wcrt_lock_initializer_name(name);
    guard = CreateMutexA(NULL, 0, name);
    if (guard == NULL) {
        return NULL;
    }
    if (WaitForSingleObject(guard, WCRT_INFINITE) != WCRT_WAIT_OBJECT_0) {
        CloseHandle(guard);
        return NULL;
    }
    return guard;
}

/**
 * @brief Releases the initialization handle returned by the holder.
 *
 * @param guard Handle previously returned by wcrt_lock_hold_initializer.
 */
static void wcrt_lock_drop_initializer(void *guard)
{
    ReleaseMutex(guard);
    CloseHandle(guard);
}

/** @copydoc __wcrt_lock_acquire */
int __wcrt_lock_acquire(struct wcrt_lock *lock)
{
    void *guard;

    if (lock == NULL) {
        return -1;
    }
    if (lock->state != WCRT_LOCK_READY) {
        guard = wcrt_lock_hold_initializer();
        if (guard == NULL) {
            return -1;
        }
        if (lock->state != WCRT_LOCK_READY) {
            InitializeCriticalSection(lock->storage);
            lock->state = WCRT_LOCK_READY;
        }
        wcrt_lock_drop_initializer(guard);
    }
    EnterCriticalSection(lock->storage);
    return 0;
}

/** @copydoc __wcrt_lock_release */
void __wcrt_lock_release(struct wcrt_lock *lock)
{
    if (lock == NULL || lock->state != WCRT_LOCK_READY) {
        return;
    }
    LeaveCriticalSection(lock->storage);
}
