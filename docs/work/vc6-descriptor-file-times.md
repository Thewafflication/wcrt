# Descriptor file-time update tranche

## Plan, design and estimate

Add selected explicit-width _futime32/_futime64 and the opt-in x86 _futime
source route. Depend on REQ-0048 conversion/ranges, REQ-0067 descriptors and
REQ-0091 source selection. Estimate: 80-140 runtime/header lines, 180-280
fixture lines, 60-120 minutes. Share checked timestamp conversion with path
updates; borrow the existing disk handle without closing it, reopening a path,
repositioning, flushing or clearing FILE state. Retain existing exports/ABI.

Validate descriptor/type before timestamp ranges. Reject valid pipe/character
handles with EINVAL; invalid descriptors/handles use EBADF. Native OS write
attribute denial maps to EACCES. Success preserves errno. Explicit ranges and
null-current fractional ticks match REQ-0048. No binary generic _futime export,
calendar ABI change, wide path, inherited-descriptor or threading claim.

Review handle ownership, read-only handles, independent fields, boundary/error
paths and FILE-state preservation. Require native/provider/package x86/x64,
ARM64 cross-links, optional Microsoft consumer, TC-0048/0091 regressions,
quality, traceability and Windows 2000 import gates. Genuine VC6 compiler and
reference qualification remains Unknown. Actual effort will not be fabricated.

## Implementation and review

The shared helper retains path attribute-error precedence and validates current
whole seconds before mutation. Descriptor updates query the live handle/type,
convert times and call SetFileTime directly with unchanged creation time; no
close, reopen, position or FILE mutation occurs. Existing GetFileType import
is reused. Public explicit declarations and the source selector are additive.

Native fixtures retain the complete FILE snapshot, pushback and shared position
across duplicate updates, reject read-only native handles/pipe/NUL/closed
descriptors and leave metadata unchanged after bad explicit ranges. Provider
fixtures check future/current boundaries, invalid handle types, no updates on
failure and no close/open even after failed SetFileTime. The Microsoft fixture
checks both the selected narrow descriptor call and explicit wide calls.

Initial integration included the internal FILE header with Windows headers,
exposing conflicting host time_t/wint_t definitions. The backend requires only
the public descriptor query and Windows GetFileType, so the internal include
was removed. An 81-character update-call line was wrapped after quality review.
Both defects were removed before final gates; no extra dependency/import arose.

## Verification and closeout

On 2026-10-01 native/provider, static and DLL consumers passed x86/x64.
ARM64 direct/provider and packaged consumers compiled/linked only. TC-0048
shared-helper regressions and TC-0091 default/source/wrong-target gates passed.
Current native timestamps were read back against the measured clock interval;
file size, FILE state, pushback and position remained intact. Explicit narrow
and wide exports exist in all packages; the generic binary export is absent.

The supplementary Microsoft major-16 /Od and /O2 consumers passed selected
_futime and explicit _futime64 calls, with retained input/executable hashes
verified independently and no host CRT import. Both full native aggregates
passed all 74 cases. TC-0016 passed 341 files, traceability passed 92 mappings,
Windows 2000 x86 imports and diff whitespace checks passed. Runtime/header
growth was about 80 lines; fixtures grew about 220 lines, within estimates.
Actual effort was not reliably timed. Generic binary legacy exports, full
calendar ABI and genuine VC6/reference qualification remain open.

Final disk-handle review found that GetFileType alone cannot distinguish a
directory. Added the existing _fstat64 regular-file check and provider fixtures
for directory metadata/query failure before clock/update. This closes a review
finding without adding imports or changing handle ownership.
The metadata-check build then passed TC-0093/0048 direct/provider and packaged
x86/x64 gates, ARM64 cross-links, Microsoft /Od-/O2, all 341 quality files,
92 traceability mappings and Windows 2000 imports. The prior full 74-case
aggregates remain the broad baseline, supplemented by these affected gates.
Compilation caught that the existing MS header has no _S_IFMT spelling; the
private adapter uses the existing 0xf000 file-type encoding without expanding
the public header inventory.
