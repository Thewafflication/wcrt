/** @file cpu.h @brief Declares WCRT processor capability and count queries. */

#ifndef WCRT_CPU_H
#define WCRT_CPU_H

#define WCRT_CPU_ARCH_UNKNOWN 0
#define WCRT_CPU_ARCH_X86 1
#define WCRT_CPU_ARCH_X64 2
#define WCRT_CPU_ARCH_ARM64 3

#define WCRT_CPU_MMX       0x00000001UL
#define WCRT_CPU_SSE       0x00000002UL
#define WCRT_CPU_SSE2      0x00000004UL
#define WCRT_CPU_SSE3      0x00000008UL
#define WCRT_CPU_SSSE3     0x00000010UL
#define WCRT_CPU_SSE41     0x00000020UL
#define WCRT_CPU_SSE42     0x00000040UL
#define WCRT_CPU_AES       0x00000080UL
#define WCRT_CPU_AVX       0x00000100UL
#define WCRT_CPU_AVX2      0x00000200UL
#define WCRT_CPU_FMA       0x00000400UL
#define WCRT_CPU_AVX512F   0x00000800UL
#define WCRT_CPU_BMI1      0x00001000UL
#define WCRT_CPU_BMI2      0x00002000UL
#define WCRT_CPU_NEON      0x00004000UL
#define WCRT_CPU_ARM_CRYPTO 0x00008000UL
#define WCRT_CPU_ARM_CRC32 0x00010000UL
#define WCRT_CPU_ARM_ATOMICS 0x00020000UL

/** @brief Immutable information for the executing process architecture. */
struct wcrt_cpu_info {
    int architecture;        /**< WCRT_CPU_ARCH_* process architecture. */
    unsigned long features;  /**< Usable WCRT_CPU_* instruction flags. */
    char vendor[13];         /**< x86 CPUID vendor, or empty if unknown. */
    char brand[49];          /**< x86 CPUID brand, or empty if unknown. */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes once and returns immutable process-lifetime information.
 * @return Runtime-owned information, or NULL if synchronization fails.
 * @note Thread-safe. Do not call from DllMain. Emulation reports the executing
 *     architecture, not necessarily the native host architecture.
 */
const struct wcrt_cpu_info *wcrt_cpu_get_info(void);

/**
 * @brief Tests whether all requested instruction flags are usable.
 * @param features Nonzero combination of WCRT_CPU_* instruction flags.
 * @return One if every flag is usable; zero for unknown flags, zero input,
 *     unavailable features, or initialization failure.
 */
int wcrt_cpu_has_features(unsigned long features);

/** @brief Returns a fresh system-wide logical processor count, or zero. */
unsigned long wcrt_cpu_logical_count(void);

/**
 * @brief Returns a fresh system-wide physical core count, or zero if unknown.
 * @note Requires optional Windows topology support; never guesses from SMT.
 */
unsigned long wcrt_cpu_core_count(void);

/**
 * @brief Counts the current process affinity mask, or returns zero if unknown.
 * @note Multi-group systems return zero. This is not a count of running
 *     software threads and does not account for thread affinity, CPU sets,
 *     or job quotas. Queries reflect subsequent process-affinity changes.
 */
unsigned long wcrt_cpu_available_count(void);

#ifdef __cplusplus
}
#endif

#endif
