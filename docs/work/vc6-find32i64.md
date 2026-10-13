# Narrow-time, wide-size enumeration tranche

## Plan and design

Implement REQ-0100 explicit wcrt_findfirst32i64/wcrt_findnext32i64 with a
288-byte record: 32-bit attributes/times, signed 64-bit size at offset 16 and
name at 24. Extend WCRT_VC6_FIND32 with _finddatai64_t/_findfirsti64/_findnexti64
aliases. Publish _fsize_t as unsigned long and use it in the existing narrow
record. Reuse REQ-0096 ownership, matching, time and attribute policy, with the
same findclose API for both record types. No new Windows import is needed.

Estimate 90-150 runtime/header lines, 160-230 fixture lines and 70-110 minutes.
Review signed-size limits, record packing, source alias composition, ownership
after conversion failure, callable function pointers and mixed record searches.
Require native small-file equivalence, deterministic UINT32_MAX+1/INT64_MAX
sizes and overflow, package x86/x64, ARM64 cross-links, supplementary Microsoft
calls and existing search regressions; retain quality/traceability/import gates.
Additive exports/source aliases require rebuilt packages. Genuine VC6 and
reference-runtime qualification remain Unknown; effort is not reliably timed.

Review of Microsoft's [filename search documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/filename-search-functions)
identified that failed findnext leaves the previous record unchanged. REQ-0096
currently zeros it. Correct the selected next-operation contract to retain
caller output on every failure, including exhaustion/overflow/invalid handles,
using temporary conversion/commit. First failure continues to zero its output.
This corrects a review finding in the current uncommitted tranche; native,
provider and Microsoft tests must verify retention rather than zeroing.
The same documentation specifies unsigned-long _fsize_t and signed 64-bit
wide file lengths. Current MSVC 16 header inspection corroborates these types
but does not qualify VC6 layouts or reference behavior.

## Implementation and review

The presence fixture first reproduced the missing wide-size record/interfaces.
Shared first/next ownership adapters take a record converter; narrow metadata
checks are reused by the wide-size conversion with only native high-size
narrowing disabled. The wide record checks its sign bit before reconstructing
the full signed-64-bit value and commits all bytes, including zero padding.
ADR-0010 records the versioned layout/source-route decision.

Focused native/provider and static/DLL package tests passed x86/x64; ARM64
compiled/linked only. Native searches contain two uniquely owned files and
verify both mixed-record directions without assuming enumeration order.
Providers accept UINT32_MAX+1 and INT64_MAX, reject the first signed-size
overflow, retain records on overflow/exhaustion/closed handles, and verify
first-conversion/allocation cleanup and adjacent guards. The baseline narrow
provider is rerun within the wide fixture, retaining its previous partitions.
All four source selectors compose in the supplementary fixture, with explicit
288-byte layout, 24-byte name offset and unsigned four-byte _fsize_t checks.

Review found first-search output zeroing erased a specification stored in that
same output name array. The helper now reads the pattern via FindFirstFileA
before clearing output; native tests cover both record forms with this alias.
Failed-next zeroing is removed and existing native/Microsoft fixtures now
compare the complete previous record. Two 83-character layout assertions were
split before the source-quality gate passed 371 files. Traceability passed
99 mappings. Full native and supplementary evidence renewal remain closeout
gates; no new Windows import or vendor source is introduced.

## Closeout

Both native targets passed 15 C89 and 81 extension cases, including the revised
narrow search and controlled io.h inventory. Static/DLL packages passed
x86/x64; ARM64 compiled/linked only. Supplementary MSVC 16 compiled 24 headers
and eleven source units at /Od and /O2, executed both consumers without host
CRT imports and retained 87 matching input hashes. Source quality passed
371 files with zero violations; traceability passed 99 mappings. Windows 2000
imports and diff checks passed. Runtime/header growth is about 105 lines,
within estimate; fixtures are about 200 lines plus revised narrow checks.
Actual effort was untimed. Generic legacy binary, genuine VC6/reference,
concurrent search and reused-handle qualification remain open.
