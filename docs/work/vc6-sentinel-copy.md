# Bounded sentinel-copy tranche

## Plan and design

Implement REQ-0103 _memccpy in string.h and a controlled memory.h forwarding
header. Copy at most count bytes, including the first byte equal to the
unsigned-char conversion of the int sentinel; return the next destination
address on a match and NULL otherwise. Zero count accesses neither buffer.
Nonzero counts require valid nonoverlapping buffers. Preserve errno. No secure
invalid-parameter handler, partial-overlap contract or POSIX alias is added.

Estimate 15-30 implementation/header lines, 80-130 fixture lines and 30-60
minutes. Review count boundaries, embedded zero, all 256 sentinel bytes,
negative/high int conversion, return pointer and source/destination guards.
Reuse one callable checker across native/static/DLL and supplementary Microsoft
/Od and /O2 consumers. Add standalone memory.h checking and include order tests.
Rebuild x86/x64/ARM64 packages; run focused cases, existing string regressions,
full native suites, ARM64 C89 cross builds, quality, traceability and imports.
No new Windows import, ABI record or allocation is planned. All public-header
changes require renewed Microsoft input hashes. Genuine VC6/reference remains
Unknown; actual effort will be untimed. Current Microsoft documentation supports
the signature and stop/return rules; unsigned conversion is WCRT's explicit
selected contract pending reference qualification.

Source: [Microsoft _memccpy reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/memccpy).
Implementation starts after REQ-0102 full-suite closeout.

## Implementation and review

Implemented a byte loop in string.c with decrementing count, inclusive first
sentinel match and unsigned conversion. Added memory.h forwarding string.h;
existing C89 routines and declarations retain their contracts. Tests enumerate
all 256 bytes across 258 count values, inspect every destination guard/tail,
retain the source, compare exact return pointers and cover absent matches,
negative/high sentinel values, INT_MIN/INT_MAX and zero-count NULL inputs.
A shared checker exercises the Microsoft DLL and both header inclusion orders.

Review checked no zero-count dereference or NULL pointer arithmetic, no signed
byte comparison, no count wraparound, unchanged errno and inclusive stop writes.
Nonzero invalid buffers and overlap remain excluded. The implementation adds
16 runtime lines and 8 header lines; fixture growth is about 70 lines, below
estimate. No allocation, native API or host CRT dependency is introduced.
Focused direct and packaged x86/x64 tests pass; ARM64 cases compile/link only.
On 2026-10-13 both sequential native extension aggregates passed all 84 cases.
Supplementary Microsoft major-16 /Od and /O2 consumers passed against the
rebuilt x86 Debug DLL. Their initial C4310 warning-as-error was corrected by
masking INT_MIN/INT_MAX to UCHAR_MAX before narrowing fixture bytes; runtime
sentinel inputs retain the original integer boundaries. TC-0103 passed direct
x86/x64 and packaged x86 static/DLL checks; ARM64 compiled/linked only.
All 15 ARM64 C89 cross-builds, 102 traceability mappings, source quality and
Windows 2000 x86 imports passed. Genuine VC6/reference remains Unknown.
Actual effort is untimed.
