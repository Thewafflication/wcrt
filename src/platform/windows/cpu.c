/** @file cpu.c @brief Implements processor discovery outside the DLL loader. */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wcrt/cpu.h>

#include "../../internal/lock.h"

#if defined(__TINYC__) || defined(__GNUC__)
#define WCRT_WINAPI __attribute__((stdcall))
#else
#define WCRT_WINAPI __stdcall
#endif

/** @brief Windows SYSTEM_INFO layout, including pointer-sized masks. */
struct wcrt_system_info {
    unsigned short architecture;
    unsigned short reserved;
    unsigned long page_size;
    void *minimum_address;
    void *maximum_address;
    size_t active_mask;
    unsigned long processor_count;
    unsigned long processor_type;
    unsigned long allocation_granularity;
    unsigned short processor_level;
    unsigned short processor_revision;
};

/** @brief Common prefix of variable-length Windows topology records. */
struct wcrt_topology_header {
    unsigned long relationship;
    unsigned long size;
};

/** @cond WCRT_WINDOWS_IMPORTS */
__declspec(dllimport) void WCRT_WINAPI GetSystemInfo(
    struct wcrt_system_info *info);
__declspec(dllimport) int WCRT_WINAPI IsProcessorFeaturePresent(
    unsigned long feature);
__declspec(dllimport) void *WCRT_WINAPI GetModuleHandleA(const char *name);
__declspec(dllimport) void *WCRT_WINAPI GetProcAddress(void *module,
    const char *name);
__declspec(dllimport) void *WCRT_WINAPI GetCurrentProcess(void);
__declspec(dllimport) int WCRT_WINAPI GetProcessAffinityMask(void *process,
    size_t *process_mask, size_t *system_mask);
/** @endcond */

typedef unsigned long (WCRT_WINAPI *wcrt_active_count_fn)(unsigned short);
typedef unsigned short (WCRT_WINAPI *wcrt_group_count_fn)(void);
typedef int (WCRT_WINAPI *wcrt_topology_fn)(int, void *, unsigned long *);

static struct wcrt_lock wcrt_cpu_lock;
static struct wcrt_cpu_info wcrt_cpu_cached;
static int wcrt_cpu_ready;
static wcrt_active_count_fn wcrt_active_count;
static wcrt_group_count_fn wcrt_group_count;
static wcrt_topology_fn wcrt_topology;

#if defined(__i386__) || defined(__x86_64__)
/** @brief Tests CPUID availability without executing it on pre-CPUID x86. */
static int wcrt_has_cpuid(void)
{
#if defined(__i386__)
    unsigned int before;
    unsigned int after;
    __asm__ volatile (
        "pushfl\n\tpopl %0\n\tmovl %0,%1\n\txorl $0x200000,%1\n\t"
        "pushl %1\n\tpopfl\n\tpushfl\n\tpopl %1\n\tpushl %0\n\tpopfl"
        : "=&r" (before), "=&r" (after) : : "cc");
    return ((before ^ after) & 0x200000U) != 0;
#else
    return 1;
#endif
}

/** @brief Reads one supported CPUID leaf and subleaf. */
static void wcrt_cpuid(unsigned int leaf, unsigned int subleaf,
    unsigned int output[4])
{
    __asm__ volatile ("cpuid"
        : "=a" (output[0]), "=b" (output[1]),
          "=c" (output[2]), "=d" (output[3])
        : "a" (leaf), "c" (subleaf));
}

/** @brief Reads low XCR0 bits only after CPUID confirms OSXSAVE. */
static unsigned int wcrt_xcr0(void)
{
    unsigned int low;
    unsigned int high;
    __asm__ volatile (".byte 0x0f,0x01,0xd0"
        : "=a" (low), "=d" (high) : "c" (0));
    return low;
}

/** @brief Decodes CPUID flags gated by the operating system state. */
static unsigned long wcrt_cpu_decode_x86(unsigned int ecx, unsigned int edx,
    unsigned int extended, unsigned int state, int xmm, int mmx)
{
    unsigned long features = 0;
    if ((edx & (1U << 23)) && mmx)
        features |= WCRT_CPU_MMX;
    if ((edx & (1U << 25)) && xmm) {
        features |= WCRT_CPU_SSE;
        if (edx & (1U << 26)) features |= WCRT_CPU_SSE2;
        if (ecx & 1U) features |= WCRT_CPU_SSE3;
        if (ecx & (1U << 9)) features |= WCRT_CPU_SSSE3;
        if (ecx & (1U << 19)) features |= WCRT_CPU_SSE41;
        if (ecx & (1U << 20)) features |= WCRT_CPU_SSE42;
        if (ecx & (1U << 25)) features |= WCRT_CPU_AES;
    }
    if ((ecx & 0x0c000000U) != 0x0c000000U) state = 0;
    if ((ecx & (1U << 28)) && (state & 6U) == 6U) {
        features |= WCRT_CPU_AVX;
        if (ecx & (1U << 12)) features |= WCRT_CPU_FMA;
    }
    if (extended & (1U << 3)) features |= WCRT_CPU_BMI1;
    if (extended & (1U << 8)) features |= WCRT_CPU_BMI2;
    if (features & WCRT_CPU_AVX) {
        if (extended & (1U << 5)) features |= WCRT_CPU_AVX2;
        if ((extended & (1U << 16)) && (state & 0xe6U) == 0xe6U)
            features |= WCRT_CPU_AVX512F;
    }
    return features;
}

/** @brief Collects x86 identification and OS-usable instruction flags. */
static void wcrt_cpu_discover_x86(void)
{
    unsigned int words[4];
    unsigned int maximum;
    unsigned int ecx;
    unsigned int edx;
    unsigned int extended = 0;
    unsigned int state = 0;
    unsigned int leaf;

    if (!wcrt_has_cpuid()) return;
    wcrt_cpuid(0, 0, words);
    maximum = words[0];
    memcpy(wcrt_cpu_cached.vendor, &words[1], 4);
    memcpy(wcrt_cpu_cached.vendor + 4, &words[3], 4);
    memcpy(wcrt_cpu_cached.vendor + 8, &words[2], 4);
    wcrt_cpuid(0x80000000U, 0, words);
    if (words[0] >= 0x80000004U) {
        for (leaf = 0; leaf < 3; ++leaf) {
            wcrt_cpuid(0x80000002U + leaf, 0, words);
            memcpy(wcrt_cpu_cached.brand + leaf * 16, words, 16);
        }
    }
    if (maximum < 1) return;
    wcrt_cpuid(1, 0, words);
    ecx = words[2];
    edx = words[3];
    if ((ecx & 0x0c000000U) == 0x0c000000U) state = wcrt_xcr0();
    if (maximum >= 7) {
        wcrt_cpuid(7, 0, words);
        extended = words[1];
    }
    wcrt_cpu_cached.features = wcrt_cpu_decode_x86(ecx, edx, extended,
        state, IsProcessorFeaturePresent(6), IsProcessorFeaturePresent(3));
}
#endif

/** @brief Publishes capabilities and optional API addresses while locked. */
static void wcrt_cpu_discover(void)
{
    void *kernel = GetModuleHandleA("kernel32.dll");

#if defined(__i386__)
    wcrt_cpu_cached.architecture = WCRT_CPU_ARCH_X86;
#elif defined(__x86_64__)
    wcrt_cpu_cached.architecture = WCRT_CPU_ARCH_X64;
#elif defined(__aarch64__)
    wcrt_cpu_cached.architecture = WCRT_CPU_ARCH_ARM64;
#endif
#if defined(__i386__) || defined(__x86_64__)
    wcrt_cpu_discover_x86();
#elif defined(__aarch64__)
    if (IsProcessorFeaturePresent(19))
        wcrt_cpu_cached.features |= WCRT_CPU_NEON;
    if (IsProcessorFeaturePresent(30))
        wcrt_cpu_cached.features |= WCRT_CPU_ARM_CRYPTO;
    if (IsProcessorFeaturePresent(31))
        wcrt_cpu_cached.features |= WCRT_CPU_ARM_CRC32;
    if (IsProcessorFeaturePresent(34))
        wcrt_cpu_cached.features |= WCRT_CPU_ARM_ATOMICS;
#endif
    if (kernel != NULL) {
        wcrt_active_count = (wcrt_active_count_fn)GetProcAddress(kernel,
            "GetActiveProcessorCount");
        wcrt_group_count = (wcrt_group_count_fn)GetProcAddress(kernel,
            "GetActiveProcessorGroupCount");
        wcrt_topology = (wcrt_topology_fn)GetProcAddress(kernel,
            "GetLogicalProcessorInformationEx");
    }
}

const struct wcrt_cpu_info *wcrt_cpu_get_info(void)
{
    if (__wcrt_lock_acquire(&wcrt_cpu_lock) != 0) return NULL;
    if (!wcrt_cpu_ready) {
        wcrt_cpu_discover();
        wcrt_cpu_ready = 1;
    }
    __wcrt_lock_release(&wcrt_cpu_lock);
    return &wcrt_cpu_cached;
}

int wcrt_cpu_has_features(unsigned long features)
{
    const struct wcrt_cpu_info *info = wcrt_cpu_get_info();
    return info != NULL && features != 0 &&
        (info->features & features) == features;
}

unsigned long wcrt_cpu_logical_count(void)
{
    struct wcrt_system_info info;
    if (wcrt_cpu_get_info() == NULL) return 0;
    if (wcrt_active_count != NULL) return wcrt_active_count(0xffffU);
    GetSystemInfo(&info);
    return info.processor_count;
}

unsigned long wcrt_cpu_available_count(void)
{
    size_t process_mask;
    size_t system_mask;
    unsigned long count = 0;
    if (wcrt_cpu_get_info() == NULL) return 0;
    if (wcrt_group_count != NULL && wcrt_group_count() != 1) return 0;
    if (!GetProcessAffinityMask(GetCurrentProcess(), &process_mask,
        &system_mask)) return 0;
    process_mask &= system_mask;
    while (process_mask != 0) {
        process_mask &= process_mask - 1;
        ++count;
    }
    return count;
}

unsigned long wcrt_cpu_core_count(void)
{
    unsigned long bytes = 0;
    unsigned long capacity;
    unsigned long offset = 0;
    unsigned long count = 0;
    unsigned char *buffer;
    struct wcrt_topology_header header;

    if (wcrt_cpu_get_info() == NULL || wcrt_topology == NULL) return 0;
    wcrt_topology(0, NULL, &bytes);
    if (bytes == 0) return 0;
    capacity = bytes;
    buffer = (unsigned char *)malloc(capacity);
    if (buffer == NULL) return 0;
    if (!wcrt_topology(0, buffer, &bytes) || bytes > capacity) {
        free(buffer);
        return 0;
    }
    while (offset < bytes) {
        if (bytes - offset < sizeof(header)) break;
        memcpy(&header, buffer + offset, sizeof(header));
        if (header.size < sizeof(header) || header.size > bytes - offset)
            break;
        if (header.relationship == 0) ++count;
        offset += header.size;
    }
    free(buffer);
    return offset == bytes ? count : 0;
}
