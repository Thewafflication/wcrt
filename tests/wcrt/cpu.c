/** @file cpu.c @brief Verifies TC-0075 first use, capabilities and affinity. */

#define WCRT_POSIX 1
#include <pthread.h>
#include <stddef.h>
#include <string.h>
#include <wcrt/cpu.h>

#define TEST_WINAPI __attribute__((stdcall))
__declspec(dllimport) void *TEST_WINAPI GetCurrentProcess(void);
__declspec(dllimport) int TEST_WINAPI GetProcessAffinityMask(void *process,
    size_t *process_mask, size_t *system_mask);
__declspec(dllimport) int TEST_WINAPI SetProcessAffinityMask(void *process,
    size_t process_mask);

static pthread_mutex_t gate_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t gate_condition = PTHREAD_COND_INITIALIZER;
static int gate_open;
static const struct wcrt_cpu_info *observations[16];
static struct wcrt_cpu_info snapshots[16];

/** @brief Releases simultaneous first queries from a common gate. */
static void *query_worker(void *argument)
{
    int index = *(int *)argument;
    pthread_mutex_lock(&gate_mutex);
    while (!gate_open) pthread_cond_wait(&gate_condition, &gate_mutex);
    pthread_mutex_unlock(&gate_mutex);
    observations[index] = wcrt_cpu_get_info();
    if (observations[index] != NULL)
        snapshots[index] = *observations[index];
    return NULL;
}

/** @brief Tests publication, architecture, feature flags and live counts. */
int main(void)
{
    pthread_t threads[16];
    int indices[16];
    int index;
    int affinity_result;
    size_t original;
    size_t system;
    size_t single;
    unsigned long logical;
    unsigned long cores;
    unsigned long available;
    unsigned long bit;
    const struct wcrt_cpu_info *info;

    for (index = 0; index < 16; ++index) {
        indices[index] = index;
        if (pthread_create(&threads[index], NULL, query_worker,
            &indices[index]) != 0) return 1;
    }
    pthread_mutex_lock(&gate_mutex);
    gate_open = 1;
    pthread_cond_broadcast(&gate_condition);
    pthread_mutex_unlock(&gate_mutex);
    for (index = 0; index < 16; ++index)
        if (pthread_join(threads[index], NULL) != 0) return 2;
    info = wcrt_cpu_get_info();
    if (info == NULL) return 3;
    for (index = 0; index < 16; ++index)
        if (observations[index] != info ||
            snapshots[index].architecture != info->architecture ||
            snapshots[index].features != info->features ||
            strcmp(snapshots[index].vendor, info->vendor) != 0 ||
            strcmp(snapshots[index].brand, info->brand) != 0) return 4;
#if defined(__i386__)
    if (info->architecture != WCRT_CPU_ARCH_X86) return 5;
#elif defined(__x86_64__)
    if (info->architecture != WCRT_CPU_ARCH_X64) return 5;
    if (!wcrt_cpu_has_features(WCRT_CPU_SSE2) ||
        strlen(info->vendor) != 12) return 16;
#elif defined(__aarch64__)
    if (info->architecture != WCRT_CPU_ARCH_ARM64) return 5;
#endif
    if (info->vendor[12] != '\0' || info->brand[48] != '\0') return 6;
    if (wcrt_cpu_has_features(0) ||
        wcrt_cpu_has_features(0x80000000UL)) return 7;
    for (bit = WCRT_CPU_MMX; bit <= WCRT_CPU_ARM_ATOMICS; bit <<= 1)
        if (wcrt_cpu_has_features(bit) != ((info->features & bit) != 0))
            return 8;
    if (info->features && !wcrt_cpu_has_features(info->features)) return 9;
    if ((info->features & (WCRT_CPU_AVX2 | WCRT_CPU_FMA |
        WCRT_CPU_AVX512F)) && !(info->features & WCRT_CPU_AVX)) return 10;
    logical = wcrt_cpu_logical_count();
    cores = wcrt_cpu_core_count();
    available = wcrt_cpu_available_count();
    if (!logical || cores > logical || available > logical) return 11;
    if (available != 0) {
        if (!GetProcessAffinityMask(GetCurrentProcess(), &original,
            &system) || original == 0) return 12;
        single = original & (~original + 1);
        if (!SetProcessAffinityMask(GetCurrentProcess(), single)) return 13;
        affinity_result = wcrt_cpu_available_count() == 1;
        if (!SetProcessAffinityMask(GetCurrentProcess(), original)) return 14;
        if (!affinity_result || wcrt_cpu_available_count() != available)
            return 15;
    }
    pthread_cond_destroy(&gate_condition);
    pthread_mutex_destroy(&gate_mutex);
    return 0;
}
