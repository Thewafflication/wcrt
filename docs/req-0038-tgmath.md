# REQ-0038 — C99 type-generic mathematics

**Content type:** Project requirement

**Status:** Implementation baselined; TinyCC 1441 x86/x64 verification passes
and ARM64 compile verification passes; native ARM64 review remains open

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.22

## Scope

This requirement controls `<tgmath.h>` and its 60 type-generic macro names.
The selected targets remain requirements of REQ-0035 and REQ-0037 rather than
being numerically reimplemented here.

## Requirement

- `<tgmath.h>` shall include WCRT `<math.h>` and `<complex.h>` and define all
  17 shared real/complex names, 38 real-only names, and five complex-only names
  prescribed by C99. `modf` and `nan` shall not be generic macros.
- A long-double generic argument shall select the `l` target; otherwise a
  double or integer generic argument shall select the unsuffixed target;
  otherwise a float argument shall select the `f` target.
- A complex generic argument shall select the complex target for a shared
  family. Mixed generic parameters shall follow their common semantic type.
  `nexttoward` shall dispatch from its first parameter because its second
  parameter is always `long double`.
- The selected expression shall have the target's prescribed result type,
  including real results from `cabs`, `carg`, `cimag`, and `creal`, and integer
  results from inquiry and rounding families.
- Each run-time macro argument shall be evaluated exactly once. Type selection
  shall not evaluate its controlling expression.
- Parenthesizing a standard function name shall suppress the function-like
  macro and preserve ordinary function access.
- No type-generic addition shall be visible in selected C89 mode.

## Rationale

Dispatch is a compile-time contract layered on independently verified targets.
TinyCC's `_Generic` extension supplies the dispatch mechanism. WCRT uses
explicit nested selections for mixed parameters because TinyCC 1441 gives
`float + long double` type `double` in a controlling expression on Windows.

## Verification

**Method:** Complete macro inventory, compile-time result-type assertions,
selected-target sentinels, mixed-type matrices, side-effect instrumentation,
macro suppression, and C89 isolation

**References:** `TC-0038`

TC-0038 shall prove the 60-name map and every semantic type. While the selected
TinyCC emits ADR-0005's exact type/parser and imaginary-literal diagnostics,
the test is `ExpectedFail`. Once both complex capability probes succeed, any
dispatch failure is a normal release-blocking failure.

## Relationships

- **Derived from:** C99 §7.22
- **Depends on:** REQ-0035, REQ-0037, and ADR-0005
- **Conflicts with:** Multiple evaluation or a real-only header represented as
  complete C99 type-generic mathematics

## Implementation Record

`include/tgmath.h` contains the complete 60-name `_Generic` dispatch map. The
macros choose a function designator in an unevaluated controlling expression
and invoke it once with the original arguments. TinyCC 1441 passes TC-0038
natively on x86 and x64 and compiles it for ARM64; native ARM64 execution and
independent review remain open.
