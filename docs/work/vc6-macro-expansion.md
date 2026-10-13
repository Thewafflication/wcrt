# VC6 macro expansion and floating-limit correction

## Plan and design

Extend REQ-0079 beyond standalone declarations to selected macro expansions
in real x86 consumer code, compiled unoptimized and optimized. Scope: assertion
re-inclusion/side effects, offsetof, limits, locale/ctype, signal delivery and
setjmp/longjmp. Estimate: 80-140 fixture/runner lines and 30-90 minutes.
Risks: macro literal portability, compiler warnings in constant checks and
fixture assumptions about non-local control flow. Reuse the existing DLL probe
without host CRT imports and keep genuine VC6 qualification separate.

## Defect and review

Baseline 9a2564c spells FLT_MAX and HUGE_VALF as 3.40282347e+38F. Expanding
FLT_MAX with installed Microsoft compiler major 16 produced C2177 (constant
too large); TinyCC accepted the literal and rounded it to binary32 maximum.
This is a header portability defect under REQ-0004/0035, owned by this
tranche, discovered during verification. Replace both spellings with the
exact decimal value of binary32 maximum, preserving intended numeric behavior.
Guard its 0x7f7fffff representation independently in C89/C99 and Microsoft
consumer fixtures. No floating model, export or runtime ABI change occurs.

The fixture's initial constant if expressions triggered /W4 C4127 and were
replaced with compile-time constraints and volatile floating values. The
TinyCC native fixture initially omitted the setjmp assembly implementation;
the shared helper now accepts explicit additional sources. Both are test
construction defects, removed during verification. setjmp is used as a valid
controlling expression and modified local state is volatile. Signal handler
state is sig_atomic_t and its prior handler is restored.

## Verification and postmortem

On 2026-10-01 TC-0004/0024 and TC-0035 passed natively on x86 and x64.
The major-16 selected-VC6-branch DLL consumer passed /Od and /O2, including
mixed varargs, narrow metadata and macro fixtures, with no host CRT imports.
Genuine compiler version 12/service-pack/reference qualification is Unknown.
Additional ARM64 compile/link and package/source-quality gates are recorded
with the next header tranche. Roughly 100 macro fixture lines were added;
effort was not reliably timed. Future consumer tests should expand public
constants and macros rather than relying solely on standalone header parsing.
