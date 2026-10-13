# VC6-era path components tranche

## Plan and estimate

Scope: REQ-0076 narrow `_makepath`, `_splitpath` and legacy buffer constants.
Dependencies: existing byte-string primitives and compatibility runner.
Estimated size: 80-120 implementation lines plus tests and controlled records.
Estimated effort: 30-90 minutes. Completion criteria: behavior and declarations
pass native x86/x64, ARM64 cross-build, static/DLL packaging, source quality,
traceability and relevant regression. Genuine VC6 qualification remains Unknown
without a licensed reference toolchain. Release effect: additive interface.

## Design and review plan

Pure lexical construction/splitting; no filesystem calls or global state.
Reuse C89-compatible string operations and caller buffers. Review separator,
drive, empty-string and period boundaries, unsigned lengths, omitted outputs,
termination, errno preservation, and source/build inventory. Legacy functions
have no output size; callers own capacity and nonoverlap. Extended code-page,
wide and secure behavior needs separate requirements and is not claimed.

## Verification and closeout

On 2026-10-01, TC-0076 passed native x86/x64 using TinyCC package
0.9.28-rc.1448+72495402. Release static/DLL libraries built for x86, x64 and
ARM64. Explicit-startup packaged static/DLL consumers passed on x86/x64;
their PE images contain no host CRT import. ARM64 libraries cross-built;
native execution remains Unknown. Windows 2000 x86 DLL import audit passed.
Traceability passed for 75 requirements. Initial source-quality TC-0016 passed;
the follow-up selector change is included in the regression run.

Package testing found an existing missing DLL-data import annotation in
`errno.h`. Added the opt-in `WCRT_DLL` consumer selector and documented its
REQ-0003 impact and link usage. Static storage and ABI remain unchanged;
packaged path tests verify both reading and writing the runtime's actual
storage through DLL calls. Defect type: existing interface/linkage; removed
during integration review. Estimated implementation size was met (about 80
lines); focused effort was not timed reliably and is not invented.

No release, remote CI or genuine VC6 qualification is claimed. Remaining
evidence gates are native ARM64 and a licensed VC6 compiler/reference runtime.
