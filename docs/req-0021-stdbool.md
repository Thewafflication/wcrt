# REQ-0021 — C99 Boolean type and values

**Content type:** Project requirement

**Status:** Implemented

**Source:** ISO/IEC 9899:1999 §6.2.5, §6.3.1.2, and §7.16

## Scope

This requirement adds the C99 `<stdbool.h>` header. The compiler supplies the
`_Bool` language type; WCRT supplies only the standard header interface.

### Required files

- `include/stdbool.h` — public macros.
- `tests/c99/presence/stdbool.c` — compile-time verification.
- `tests/c99/run-tc-0021.ps1` — automated runner.

## Requirement

- In C99 and later modes, WCRT's self-contained `<stdbool.h>` shall define
  `bool` as `_Bool`, `true` as `1`, `false` as `0`, and
  `__bool_true_false_are_defined` as `1`.
- `true`, `false`, and `__bool_true_false_are_defined` shall be usable in
  integer constant expressions.
- Conversion to `_Bool` shall produce zero for a value that compares equal to
  zero and one for every value that compares unequal to zero.
- Repeated inclusion shall not change or duplicate the header interface.
- Selecting the C89 library surface shall not expose `_Bool` or the C99 Boolean
  macros through a C89 header.

## Rationale

The Boolean interface is a dependency of later C99 headers and applications,
while compiler-owned language semantics must remain distinct from runtime work.

## Verification

**Method:** Automated compile-time test

**References:** `TC-0021`

TC-0021 shall verify macro spelling and values, integer-constant-expression
use, scalar conversions, self-containment, repeated inclusion, and C89-mode
isolation on every supported TinyCC target.

## Relationships

- **Derived from:** C99 Milestone 2A
- **Depends on:** TinyCC `_Bool` support and header edition selection
- **Conflicts with:** Exposing C99-only tokens in the C89 surface

## Tailoring

This requirement applies only when the C99 or a later library surface is
selected. WCRT does not implement the compiler's `_Bool` language semantics.

## Implementation Record

`include/stdbool.h` exposes the four standard macros only when the C99 surface
is selected and honors the explicit `WCRT_C89` selector. TC-0021 verifies
self-containment, repeated inclusion, constant-expression use, scalar
conversion, and direct C89 isolation. TinyCC `_Bool` support and the Boolean
width are also recorded by the target capability probe.
