# Narrow filename enumeration tranche

## Plan

Implement REQ-0096: explicit pointer-sized search handles, a 280-byte record
with 32-bit timestamps/unsigned size, and an opt-in x86 source route.
Depend on existing Windows filename search imports and io.h pointer types.
Estimate 120-180 implementation lines, 200-300 test lines and 60-120 minutes.
Review ownership, narrowing, failure cleanup, record guards and header packing.
Require native/direct/package x86/x64 tests, ARM64 cross-links, source quality,
traceability, Windows 2000 imports and supplementary Microsoft evidence.
Release effect is additive symbols and rebuilt packages. Genuine VC6 and
reference qualification remain Unknown. Actual effort is not yet measured.

## Design

Use versioned WCRT names to keep source selection explicit. Return the Windows
search handle with an ownership entry; callers close successful searches, including
after exhaustion. Convert into a temporary record and commit only on success.
Reject unrepresentable times/sizes rather than truncating. Failed first-record
conversion closes its newly acquired handle; next-record conversion retains
the search after advancing it. No sorting, Unicode or concurrent-handle policy.
Diagnostic full-suite execution reproduced a Windows FindClose access violation
when the native fixture attempted a second close. A linked ownership registry
now rejects unknown/closed handles before invoking opaque Windows APIs. A
successful close removes/frees its entry; failed close retains it for retry.
Allocation failure closes the newly acquired search and returns ENOMEM with
zero output. Concurrent searches and native handle reuse remain unqualified.

## Implementation and review

Microsoft's current [findfirst documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/findfirst-functions)
informs search ownership and the selected error categories. It is not licensed
VC6 reference qualification. This WCRT contract expressly checks signed-long
timestamp bounds and unsigned-long file sizes; no truncation is accepted.
The default ABI is untouched and source aliases require explicit selection.

Review verified pack push/pop, default-cdecl declarations, all record offsets,
temporary-record commit and cleanup error preservation. Providers use handles
with the highest pointer bit set on each target, UINT32_MAX size, INT32_MIN/
MAX timestamps, unavailable timestamps, malformed names and overflow in both
directions. Failed next retains the search; failed first conversion closes it,
including unsuccessful cleanup that overwrites errno. Native tests use a
unique owned wildcard fixture and close after exhaustion before removal.

The first provider build lacked forward declarations for renamed Windows
calls; adding provider prototypes corrected it. The wrong-target test first
used nostdinc without TinyCC's own tccdefs include path; it now isolates the
find32 selector with normal compiler internals and checks the exact diagnostic.
The supplementary consumer initially added a second main to the existing
combined executable; the callable fixture plus standalone wrapper corrected
the test integration. Source quality caught one 82-character comment; fixed.
These were verification integration findings, corrected before closeout.
The full x86 aggregate additionally reproduced incorrect architecture detection
from a diagnostic compiler wrapper's filename. Checking the compiler's reported
target corrects this, matching the existing calendar route runner. Rerun the
affected full aggregate before closeout.

Supplementary MSVC 16 compiled all 23 standalone headers and ten consumer
translation units at /Od and /O2 with /X, then executed both DLL consumers
without host CRT imports. Its evidence retains 84 input hashes. The selected
consumer composes all four narrow selectors and searches/removes an owned file.
Compiler version 12 and reference-runtime qualification remain Unknown.

## Closeout

Native/provider and static/DLL packages passed x86/x64; ARM64 direct/provider
and package fixtures compiled/linked only. Both full native runs passed all
15 C89 and 77 extension cases after the ownership correction. Source quality
passed 356 C/H files with zero violations and traceability passed 95 mappings
before the next tranche was prepared. Windows 2000 imports and diff checks
passed; supplementary consumer input hashes matched the rebuilt runtime.
Ownership allocation failure, failed-close retry, closed-handle validation
and two simultaneous owned searches now have deterministic/native coverage.
Actual effort was not reliably timed; no release/compiler-reference gate is
closed by these results. Registry concurrency and stale reused handles remain
outside the selected contract.

REQ-0100 subsequently corrects failed-next record zeroing to retention after
review of primary Microsoft documentation. The original closeout above is
historical evidence; the revised contract and fixtures are reverified with
the wide-size tranche. First failures still zero output, and the 280-byte
record keeps its existing layout with the explicit _fsize_t field spelling.
