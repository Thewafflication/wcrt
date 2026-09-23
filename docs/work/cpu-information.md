# CPU information tranche

## Plan and estimate

Scope: REQ-0075 adds a C89 WCRT CPU query header, immutable capability
discovery, fresh processor-count queries, and optional startup integration.
Dependencies: existing internal lock, Windows baseline APIs, optional modern
topology APIs, and TinyCC architecture support. No DLL loader callback is added.
Estimated size: 400-650 implementation/header lines plus tests and records.
Estimated focused effort: 1-3 hours; topology and compiler differences dominate
uncertainty. Release effect: additive API, unchanged existing ABI and minimum OS.

## Design and verification plan

One initializer publishes immutable identification and usable instruction flags
under the existing internal lock. Startup calls it before application entry;
queries call it as well. Counts are fresh snapshots. Unknown counts are zero.
Modern topology functions are resolved dynamically to retain Windows 2000 x86
imports. Available count describes process affinity, excluding thread affinity,
CPU-set policies, and job quotas; multi-group systems return unknown rather than
a misleading partial machine count.

Review risks: unsupported instruction execution, partial publication, Windows
ABI layout, stale affinity, allocation failure, old-OS fallback, startup linkage.
Verification: deterministic discovery/fallback tests, concurrent first use,
affinity changes, native x86/x64 execution, ARM64 compilation where available,
static/DLL and console/GUI integration, import, source-quality and traceability
checks. Record hardware execution limits rather than claiming cross-builds ran.

## Review and closeout

Implemented the public header and Windows backend, startup calls, build/export
integration, packaged-consumer checks, TC-0075 and requirement traceability.
Review covered CPUID leaf guards, XSAVE/OSXSAVE before XGETBV, AVX/XCR0 gating,
immutable publication under the existing lock, Windows structure widths,
buffer bounds, live affinity and the conservative multi-group contract.

Verification on 2026-09-22/23 with TinyCC 0.9.28rc:

- TC-0075 passed native x86 and x64: sixteen concurrent first callers, feature
  masks, 256 synthetic XCR0 states, disabled XSAVE/OSXSAVE/AVX, live affinity
  restoration, old-OS fallbacks, group-aware totals and malformed topology.
- Counting initializer tests proved console and GUI initialization precedes
  application entry without relying on lazy queries from the application.
- Release static/DLL libraries and startup objects built for x86, x64, ARM64.
- x86/x64 packaged static and DLL consumers passed; TC-0017/TC-0018 startup
  tests passed, including PE inspection and the absence of host CRT imports
  in explicit-startup consumers.
- Windows 2000 x86 runtime import allowlist passed. This is import validation,
  not execution on Windows 2000 hardware.
- TC-0016 passed for 281 C/header files using local Doxygen 1.17.0;
  requirement traceability and whitespace checks passed.
- ARM64 static/DLL consumer links and console/GUI PE inspection passed; both
  CPU test sources cross-compiled. Runtime tests remain for native CI/hardware.
  Cross-compilation is not
  recorded as native execution. No remote CI run or release was requested.

Defects found and corrected during verification: test-only `__declspec` macro
redefinition; two source line-length violations; PowerShell linker-flag
quoting and scalar splatting; simultaneous main/WinMain definitions causing
TinyCC to select GUI startup in the console harness. All were test or style
issues, removed before completion; no production behavior failure was observed.

Actual new header/backend size is about 380 lines, below the 400-650 estimate;
tests and records are additional. Focused effort was not timed reliably across
the session interruption, so no actual-hours claim is made. Process improvement:
use distinct console/GUI entry definitions and explicit argument arrays in
future startup harnesses. Release acceptance still requires native ARM64 CI.
