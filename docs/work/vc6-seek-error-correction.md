# Shared seek error correction

## Plan, design and impact

REQ-0067's Microsoft seek family shares the stdio/POSIX Windows backend.
Review of baseline 9a2564c found that SetFilePointer failure was inferred from
the high word rather than the Windows last-error result. An invalid origin
can therefore appear successful or produce an unrelated range error, while a
negative seek fails without setting errno. This is a correctness defect in
the existing required family, not a new API. Estimate: 25-40 implementation
lines and 80-120 regression lines. Genuine VC6 comparison remains Unknown.

Validate origins, reject nonseeking handles using GetFileType, and inspect
GetLastError only when the low return word is 0xffffffff. Preserve valid
large positions whose low word equals the failure sentinel. Map negative and
invalid-parameter failures to EINVAL; nonseeking devices to ESPIPE; retain
existing mappings for other OS failures. On failure leave output position and
stream indicators unchanged. No new imports are required because GetFileType
is already part of the runtime/Windows 2000 allowlist. The successful disk
seek path and descriptor widths remain unchanged.

## Verification plan

Add native origin/negative/pipe tests and 64-bit sentinel-position seeks that
do not write or extend the fixture. Add deterministic OS-substitute tests for
high-word/error combinations, indicator/output preservation and file-type
errors. Run TC-0067 x86/x64, ARM64 cross-links, stdio regressions, packaged
consumers, source quality, traceability and legacy imports.

## Closeout

On 2026-10-01 the added native negative-seek regression reproduced the defect
on x86 (test result 50) before correction. TC-0067 then passed on x86/x64,
including deterministic OS partitions; static/DLL consumers passed on both
architectures with no host CRT imports. ARM64 behavior/error fixtures and
packaged consumers cross-linked; native execution remains Unknown. TC-0012
passed its 41-function stdio suite on x86/x64. Source quality passed for 304
files, traceability passed for 82 requirements and Windows 2000 imports passed.

Review replaced signed high-word shifting with an unsigned reconstruction,
kept output position untouched on failure, and rejected pipe/character handles
before invoking an undefined Windows seek operation. Native boundary tests
seek above 4/8 GB without writing and confirm the file remains eight bytes.
The defect was present in baseline 9a2564c; removal occurred during the
implementation/review phase. Existing fixed-name TC-0067 fixtures could survive
a failed test; the runner now uses unique owned names with finally cleanup.
Implementation and regression sizes are within estimate; effort was not
reliably measured. Genuine VC6 qualification remains Unknown.
