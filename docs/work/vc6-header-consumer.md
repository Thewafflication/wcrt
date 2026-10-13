# VC6 C consumer header foundation tranche

## Plan and estimate

REQ-0079 removes selected C89/MS header parsing dependencies on TinyCC-only
predefined types, offsetof and varargs when consumed by VC6 x86 C. Existing
WCRT types/layouts remain unchanged; this does not introduce a VC6 32-bit
time_t/stat ABI or Microsoft import-library packaging. Estimate: 80-140 header
and test lines, 45-120 minutes. Dependencies: REQ-0078 64-bit spelling types
and explicit WCRT DLL data linkage. No runtime implementation changes.

## Design and review plan

Select x86 stack varargs only for `_MSC_VER <= 1200` and `_M_IX86`; use
four-byte slots and char-pointer va_list. Select 32-bit size_t/ptrdiff_t,
16-bit wchar_t and address-based offsetof in the same C compiler branch.
Use shared Microsoft integer typedefs for unconditional 64-bit declarations.
DLL standard-stream objects receive the same explicit import selector as
errno. Check width/offset/layout, promotion and mixed varargs, declaration
types, C89 isolation, include order and DLL symbol use. TinyCC x86 exercises
the selected branch as a surrogate; genuine VC6 compilation and differential
execution remains Unknown and is not inferred from it. Rerun existing header,
vararg and ABI tests and package builds.

## Closeout

On 2026-10-01 the 21 selected headers compiled independently and repeatedly
with only WCRT headers and the required TinyCC `tccdefs.h` prelude. MSVC 2010
(compiler major 16) compiled the selected VC6 header branch with `/X /W4 /WX`,
created an x86 Microsoft import library with its librarian, linked the mixed
vararg DLL consumer with `/NODEFAULTLIB`, and executed successfully. Its PE
has no host CRT import. This is supplementary evidence; genuine VC6 compiler,
service pack and reference-runtime qualification remains Unknown.

The explicit verification startup owns Microsoft's link-only `_fltused`
marker. Default-library directives are suppressed/ignored; WCRT supplies the
actual functions and data. Test-only defects corrected: wrong `_stat64` size
expectation (56, not 48), accidentally requesting unimplemented share.h,
capturing compiler progress text as object filenames, and omitted marker.
These were found during header/link review before qualification was recorded.

TC-0016 source quality passed for 292 files before this tranche's final test
fixtures; final fixtures require the subsequent retained run. Traceability
passes for 78 requirements. The simultaneous x86/x64 aggregate attempt
collided in shared build directories (executable overwrite denied) and is not
used as clean evidence; x86 completed passing and x64 is rerun serially.
No ABI migration, new runtime symbol or release packaging is claimed. Focused
effort was not reliably timed; no timing metric is invented.

## Added interface consumer evidence

On 2026-10-01 the supplementary Microsoft compiler major 16 consumer passed
all 22 standalone/repeated headers and an expanded DLL fixture calling
REQ-0076/0078/0080-0083 APIs. This verifies actual Microsoft x86 call sites
for path helpers, 64-bit conversion arguments/returns, variadic creation
permissions, mode translation, shared streams and directory restoration.
The executable linked with no default libraries and no host CRT imports.
Genuine compiler version 12 and reference-runtime qualification remain Unknown.

The macro/type expansions and /Od-/O2 consumer matrix are recorded in
vc6-macro-expansion.md and vc6-system-types.md. These expanded real call sites
found and corrected FLT_MAX literal portability and strict-profile alias
isolation defects. The original closeout counts above describe earlier
snapshots; the later tranche records retain the updated aggregate evidence.
