# REQ-0075: Processor capabilities and counts

WCRT shall provide `<wcrt/cpu.h>` without requiring `WCRT_POSIX`. The C89
interface shall expose immutable process-architecture identification and usable
instruction flags, plus fresh logical processor, physical core, and process
affinity counts. CPU information is not an ISO C interface.

`wcrt_cpu_get_info()` shall initialize exactly once under the runtime lock and
return a runtime-owned `const struct wcrt_cpu_info *` valid for the runtime's
lifetime. Synchronization failure returns null and permits a subsequent retry.
Console and GUI startup shall invoke this query before `main` or `WinMain`,
terminating with status 127 if initialization fails. Ordinary static consumers,
DLL consumers and alternate startup code shall initialize on their first query.
Discovery shall not run from a DLL loader callback; callers must not invoke
these APIs from `DllMain`.

`architecture` identifies the executing binary: `WCRT_CPU_ARCH_X86`,
`WCRT_CPU_ARCH_X64`, `WCRT_CPU_ARCH_ARM64`, or `WCRT_CPU_ARCH_UNKNOWN`.
Under emulation it does not identify the native host. `vendor[13]` and
`brand[49]` are terminated CPUID strings on x86/x64, or empty if unavailable.
ARM64 identification strings are empty in this version.

`features` contains the declared `WCRT_CPU_*` instruction flags. x86/x64
supports MMX, SSE, SSE2, SSE3, SSSE3, SSE41, SSE42, AES, AVX, AVX2, FMA,
AVX512F, BMI1 and BMI2. CPUID leaf availability shall be checked. The SSE
family requires OS support for XMM state. AVX and FMA require XSAVE, OSXSAVE,
and XCR0 XMM/YMM state; AVX2 additionally requires its CPUID bit. AVX512F
also requires XCR0 opmask and ZMM state. XGETBV shall never execute without
XSAVE and OSXSAVE. ARM64 supports NEON, ARM_CRYPTO (the Windows crypto
instruction group), ARM_CRC32 and ARM_ATOMICS through Windows feature flags.
Unknown or unreported features remain clear. Flags describe the process's
execution environment at initialization, assuming Windows exposes a consistent
instruction set across its eligible processors; they are not per-core probes.

`wcrt_cpu_has_features(mask)` shall return one only for a nonzero mask whose
every bit is present. Zero, unknown flags, unavailable features and
initialization failure return zero.

Count queries shall return zero on unknown information or failure:

- `wcrt_cpu_logical_count()` uses all active processor groups where supported,
  with `GetSystemInfo` as the old-Windows fallback.
- `wcrt_cpu_core_count()` counts system-wide core topology records. Missing
  modern topology support, failed allocation/query, or malformed results
  return zero. A topology change during retrieval may also return zero; callers
  may retry. SMT ratios are never guessed.
- `wcrt_cpu_available_count()` counts the current process affinity mask on
  single-group systems, and returns zero on multi-group systems rather than
  reporting a partial count. It reflects changes in process affinity. It does
  not incorporate per-thread affinity, CPU-set policy, or job CPU quotas.

Counts are independent snapshots, not a transaction. No count is a software
thread count or a guaranteed optimal thread-pool size. Optional newer Windows
APIs shall be resolved dynamically; x86 shall retain Windows 2000 imports.

## Verification and impact

TC-0075 covers concurrent first queries, structure lifetime and termination,
architecture, feature masks, OS-state feature gating, counts, live affinity,
missing optional APIs, multiple groups and malformed topology. TC-0017 and
TC-0018 cover console/GUI integration and the startup-link contract. Package
builds include the new object in static and shared runtimes. The change is
additive and preserves existing public ABI, startup-object separation and
minimum Windows versions. No new service, dependency or privilege is required.

References: [Windows feature queries](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-isprocessorfeaturepresent),
[process affinity](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getprocessaffinitymask),
and [Intel architecture manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html).
