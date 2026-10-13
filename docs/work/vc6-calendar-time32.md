# Legacy calendar-time source route tranche

## Plan, scope and estimate

Implement REQ-0095/ADR-0009: six explicit WCRT narrow adapters and an opt-in
x86 generic time_t/function source route. Depend on REQ-0015 time/calendar,
REQ-0079 x86 headers and existing explicit metadata/file-time selectors.
Estimate: 160-240 source/header lines, 250-380 fixture lines, 90-180 minutes.
Retain the shipped wide runtime ABI and reject POSIX source-layout mixing.

Review pointer/result widths, static buffers, errno, normalization and UTC/local
epoch boundaries; test signed numeric differences without overflow. Narrow
mktime uses checked local calendar arithmetic and a copied input before calling
the existing wide implementation. Current time and UTC conversions use guarded
four-byte input/output objects. Independent wide consumers prove ABI retention.

Inspection found backend FILETIME-to-Unix subtraction performed unsigned before
division, wrapping local times before the epoch into large positive timestamps.
The narrow epoch route needs signed whole-second reconstruction. Verify with
deterministic provider clocks/offsets and retain current wide-time regressions.
No system-clock/timezone mutation or new Windows API/import is planned.

Require controlled requirements/specifications/manifests, native/package x86
source-selection tests, native explicit-adapter x64 and ARM64 cross-links,
supplementary Microsoft /Od-/O2, TC-0015/0051/0091/0093 regressions, quality,
traceability and Windows 2000 imports. New interfaces/selectors require rebuilt
packages. Generic binary legacy exports and exact VC6 timezone/DST behavior
remain open. Actual effort will not be invented.

## Defect reproduction, implementation and review

The deterministic x86 provider first reproduced the unchanged baseline clock
failure at exit code 1: a clock one FILETIME tick before the Unix epoch did
not reconstruct -1. Absolute-tick division followed by signed epoch subtraction
corrects wall, UTC/local and inverse conversions, including fractional values
before zero. No system state is changed. The selected calendar route now
round-trips local epoch dates correctly instead of unsigned wrapped dates.

Normalization arithmetic is shared, widening tm_year before adding 1900. Narrow
mktime checks its local-second calculation within the existing explicit file-
time backend window before calling wide conversion on a copy; UTC range failure
does not commit caller fields. The six versioned functions use four-byte long
arguments/results, and numeric differences convert before subtraction. Generic
header routing is x86-only; POSIX rejection sits outside the include guard so
late profile mixing after a repeated include is also rejected.

Header/fixture review shortened five lines before quality verification. One
patch request duplicated an operation on the same file; it was rejected without
edits and resubmitted as one combined file operation. The new Microsoft source
includes a shared calendar fixture; that transitive source was explicitly added
to retained hashes. Wchar-first Microsoft and time-first TinyCC selected include
orders compose all three legacy source selectors while keeping fpos/explicit
wide file-time records unchanged.

## Verification and closeout

On 2026-10-01 native explicit/provider x86/x64 and selected x86 checks passed;
static/DLL packages passed their applicable source/explicit consumers. ARM64
explicit/provider and packages cross-linked only; x64/ARM64 selectors and early/
late POSIX mixing were rejected with the expected diagnostics. Both full native
C89 suites passed all 15 cases, and both extension suites passed all 76 cases.
The optional Microsoft major-16 /Od and /O2 consumers passed narrow function
pointers, UTC/local epoch/2038, guards, normalization and source composition.
Its nine source units and shared fixture dependency are covered by 83 retained
input hashes, independently matched alongside executable hashes/no host CRT.

TC-0016 passed 350 files; traceability passed 94 mappings, Windows 2000 x86
imports and diff checks passed. Current-time provider tests retain fractional
last-second success and first-tick failure beyond 2038, with four-byte failure
stores and unchanged neighboring guards. Input normalization failure leaves the
entire calendar unchanged. Runtime/header growth was about 140 lines, below
estimate; fixtures grew about 280 lines, within estimate. Actual effort was not
reliably timed. Exact VC6 reference semantics and generic binary legacy time
exports remain open; default wide consumers retain their ABI.
