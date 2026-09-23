/** @file cpu_fallback.c @brief Exercises TC-0075 optional Windows API paths. */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wcrt/cpu.h>

/* Substitute only OS discovery; runtime locking and allocation stay real. */
#undef __declspec
#define __declspec(attribute)
#define GetSystemInfo test_system_info
#define IsProcessorFeaturePresent test_processor_feature
#define GetModuleHandleA test_module_handle
#define GetProcAddress test_proc_address
#define GetCurrentProcess test_current_process
#define GetProcessAffinityMask test_affinity
#include "../../src/platform/windows/cpu.c"

static int modern;
static int malformed;
static unsigned short groups = 1;
static int affinity_ok = 1;

/** @brief Supplies the baseline system count. */
void WCRT_WINAPI test_system_info(struct wcrt_system_info *info)
{
    memset(info, 0, sizeof(*info));
    info->processor_count = 4;
}

/** @brief Disables optional OS instruction support conservatively. */
int WCRT_WINAPI test_processor_feature(unsigned long feature)
{
    (void)feature;
    return 0;
}

/** @brief Supplies a harmless fake kernel handle. */
void *WCRT_WINAPI test_module_handle(const char *name)
{
    (void)name;
    return (void *)1;
}

/** @brief Supplies a harmless fake process handle. */
void *WCRT_WINAPI test_current_process(void)
{
    return (void *)1;
}

/** @brief Supplies a sparse affinity mask to exercise population counting. */
int WCRT_WINAPI test_affinity(void *process, size_t *mask, size_t *system)
{
    (void)process;
    *mask = 0x15;
    *system = 0xff;
    return affinity_ok;
}

/** @brief Tests that logical counts request all groups. */
static unsigned long WCRT_WINAPI test_active_count(unsigned short group)
{
    return group == 0xffffU ? 96 : 48;
}

/** @brief Supplies configurable system group counts. */
static unsigned short WCRT_WINAPI test_group_count(void)
{
    return groups;
}

/** @brief Supplies core records and malformed/failed response variants. */
static int WCRT_WINAPI test_topology(int relation, void *buffer,
    unsigned long *bytes)
{
    struct wcrt_topology_header records[2];
    if (relation != 0) return 0;
    *bytes = sizeof(records);
    if (buffer == NULL) return 0;
    if (malformed == 3) return 0;
    records[0].relationship = records[1].relationship = 0;
    records[0].size = records[1].size = sizeof(records[0]);
    if (malformed == 1) records[0].size = 0;
    if (malformed == 2) records[1].size = 100;
    memcpy(buffer, records, sizeof(records));
    return 1;
}

/** @brief Simulates old Windows missing every modern topology function. */
void *WCRT_WINAPI test_proc_address(void *module, const char *name)
{
    (void)module;
    if (!modern) return NULL;
    if (strcmp(name, "GetActiveProcessorCount") == 0)
        return (void *)test_active_count;
    if (strcmp(name, "GetActiveProcessorGroupCount") == 0)
        return (void *)test_group_count;
    if (strcmp(name, "GetLogicalProcessorInformationEx") == 0)
        return (void *)test_topology;
    return NULL;
}

/** @brief Verifies fallback, group handling and defensive topology parsing. */
int main(void)
{
#if defined(__i386__) || defined(__x86_64__)
    unsigned long flags;
    unsigned long avx_family = WCRT_CPU_AVX | WCRT_CPU_AVX2 |
        WCRT_CPU_FMA | WCRT_CPU_AVX512F;
    unsigned int state;
    /* Every XCR0 combination: XMM/YMM required; AVX512 adds three bits. */
    for (state = 0; state < 256; ++state) {
        flags = wcrt_cpu_decode_x86(~0U, ~0U, ~0U, state, 1, 1);
        if (((flags & WCRT_CPU_AVX) != 0) != ((state & 6U) == 6U))
            return 10;
        if (((flags & WCRT_CPU_AVX512F) != 0) !=
            ((state & 0xe6U) == 0xe6U)) return 11;
        if ((state & 6U) != 6U && (flags & avx_family)) return 12;
    }
    flags = wcrt_cpu_decode_x86(~(1U << 27), ~0U, ~0U, 0xff, 1, 1);
    if (flags & avx_family) return 13;
    flags = wcrt_cpu_decode_x86(~(1U << 26), ~0U, ~0U, 0xff, 1, 1);
    if (flags & avx_family) return 14;
    flags = wcrt_cpu_decode_x86(~(1U << 28), ~0U, ~0U, 0xff, 1, 1);
    if (flags & avx_family) return 15;
    flags = wcrt_cpu_decode_x86(0, 0, 0, 0xff, 1, 1);
    if (flags != 0) return 16;
    flags = wcrt_cpu_decode_x86(~0U, ~0U, 0, 0, 0, 0);
    if (flags != 0) return 17;
    flags = wcrt_cpu_decode_x86(~0U, ~0U, ~0U, 0xff, 1, 1);
    if (flags != 0x3fffUL) return 18;
#endif
    if (wcrt_cpu_logical_count() != 4 || wcrt_cpu_core_count() != 0 ||
        wcrt_cpu_available_count() != 3) return 1;
    if (wcrt_cpu_get_info()->features &
        (WCRT_CPU_MMX | WCRT_CPU_SSE | WCRT_CPU_NEON)) return 2;
    modern = 1;
    /* No threads exist here; reset the private cache to model another OS. */
    wcrt_cpu_ready = 0;
    if (wcrt_cpu_logical_count() != 96 || wcrt_cpu_core_count() != 2)
        return 3;
    groups = 2;
    if (wcrt_cpu_available_count() != 0) return 4;
    groups = 1;
    affinity_ok = 0;
    if (wcrt_cpu_available_count() != 0) return 5;
    for (malformed = 1; malformed <= 3; ++malformed)
        if (wcrt_cpu_core_count() != 0) return 6;
    return 0;
}
