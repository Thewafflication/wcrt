# Microsoft 64-bit integer conversion tranche

## Plan and estimate

REQ-0078 adds `_i64toa`, `_ui64toa`, `_atoi64`, `_strtoi64`, `_strtoui64`
and the Microsoft `_I64_MIN`, `_I64_MAX`, `_UI64_MAX` limits. These names
extend the VC6-era conversion work; exact VC6 inventory membership and genuine
compiler/reference qualification remain Unknown. Dependencies: existing
integer parser, radix writer, errno and compatibility tests. Estimate: 70-120
implementation/header lines, 100-180 test lines, 30-90 minutes. Additive ABI.

## Design and review plan

Widen the shared radix writer to unsigned long long and preserve all existing
32-bit results. Reuse the ISO parser only after explicit null/base validation;
retain Microsoft error semantics without changing ISO undefined-input rules.
Use compiler-selected 64-bit typedefs (`__int64` under MSVC, long long under
TinyCC). Verify every radix 2-36, signed/unsigned extrema, negative bit patterns,
no digits, base autodetection, stopping pointer, overflow, errors, guards and
errno preservation. Run TC-0056/0028 and new native x86/x64 coverage; ARM64
cross-build, source quality, traceability and packaged consumers. Wide/secure
variants and locale objects are outside scope.

## Closeout

On 2026-10-01 TC-0078 passed native x86/x64 and packaged static/DLL consumers
with explicit WCRT startup and no host CRT imports. ARM64 sources/consumers
compile/link; native execution remains Unknown. TC-0056 and TC-0028 passed.
Exact radix vectors were independently generated with arbitrary-precision
integer arithmetic for `(2^64)-1` in bases 2-36. Review confirmed unsigned
negation of the signed minimum and that the widened writer preserves the
32-bit wrappers' original unsigned magnitudes. Header/implementation size is
about 90 lines, within estimate. No production defect was found by local
tests. Focused effort was not timed reliably. Genuine VC6 inventory and
differential qualification remains Unknown.
