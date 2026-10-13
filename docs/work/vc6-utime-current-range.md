# Explicit-width current-time range correction

## Plan, design and estimate

During VC6 narrow-time coverage review, baseline 9a2564c's _utime32/_utime64
null-times path was found to bypass explicit-width range validation: it copies
the Windows current FILETIME directly. REQ-0048 requires enforced ranges, even
when current time is selected. Estimate: 80-150 provider/implementation lines,
30-75 minutes. Reproduce with a deterministic future clock, then validate
current whole-second Unix time against each existing width before opening or
changing a file. Preserve fractional FILETIME on representable current times.

Review epoch, signed-32-bit last second and successor, 64-bit supported maximum
and successor, null/readonly/directory/missing paths and failures at open/write.
Owned native and packaged tests, mocked clocks and cross-links apply along with
TC-0048, source quality, traceability and Windows 2000 imports. No new public
interface, ABI, dependency or import is planned. Genuine VC6 reference evidence
remains Unknown. Effort will not be fabricated if not reliably measured.

## Defect, review and verification

The deterministic x86 probe reproduced baseline failure at exit code 1: a
clock at the first second beyond signed 32-bit range returned success and
opened/wrote the fake file. The correction checks whole-second bounds before
open/write while preserving fractional FILETIME at the last valid second.
Priority: range-contract defect in REQ-0048, owned and corrected by this
tranche. Existing path/provider error precedence is retained.

On 2026-10-01 native/mock and static/DLL package fixtures passed x86 and x64;
ARM64 direct/package fixtures cross-linked only. Epoch predecessor, both
maximum-successor clocks, fractional last seconds, explicit negative/outside
limits and provider/cleanup failures passed. Native rejected explicit inputs
left existing timestamps unchanged. TC-0051 shared adapters passed both
native targets. TC-0016 passed 328 files, traceability passed 88 mappings and
Windows 2000 x86 imports passed. The preceding descriptor checkpoint's full
70-case aggregates passed x86/x64; this correction has retained targeted gates.

The initial fixture stripped __declspec before the compiler's _mingw prelude,
triggering a macro-redefinition error; removing that override let its renamed
Windows provider definitions link normally. This test-construction defect was
removed before baseline reproduction. A long parameter line was shortened
before quality verification. About 160 fixture/implementation lines were
added, slightly over estimate due to additional explicit-input and cleanup
partitions. Effort was not reliably timed. No clock or large-file mutation
was needed to establish future boundary behavior.
