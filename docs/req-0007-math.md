# REQ-0007 — `<math.h>`

**Content type:** Project requirement

**Status:** Implemented

**Source:** §4.5 Mathematics

## Scope

### Required files

- `include/math.h` — declarations and `HUGE_VAL`.
- `src/math.c` or `src/math/` — mathematical implementation.
- `tests/c89/math.c` — accuracy, boundary, domain, and range tests.

### Functions

| Draft clause | Required functions |
| --- | --- |
| §4.5.2 Trigonometric | `acos`, `asin`, `atan`, `atan2`, `cos`, `sin`, `tan` |
| §4.5.3 Hyperbolic | `cosh`, `sinh`, `tanh` |
| §4.5.4 Exponential/logarithmic | `exp`, `frexp`, `ldexp`, `log`, `log10`, `modf` |
| §4.5.5 Power | `pow`, `sqrt` |
| §4.5.6 Nearest integer, absolute value, remainder | `ceil`, `fabs`, `floor`, `fmod` |

## Requirement

- The header shall define `HUGE_VAL` with the type and use required by §4.5.1.
- Each function shall implement its specified principal result, special cases,
  and domain/range errors.
- Domain errors shall set `errno` to `EDOM` where required. Range errors shall
  set `errno` to `ERANGE` and return the specified value.
- `frexp` and `modf` shall write through their output pointers correctly;
  `ldexp` shall detect representational range errors.
- Accuracy goals, floating representation, intermediate x87 precision, signed
  zero behavior, and non-C89 NaN/infinity extensions shall be documented.
- Implementation shall not delegate claimed functions to a host CRT.

## Rationale

WCRT must provide its own `<math.h>` contract to supply C89-conforming behavior without depending on a host C runtime.

## Verification

**Method:** Automated test and inspection

**References:** `TC-0007`

Tests shall cover ordinary values, identities, quadrant behavior, boundaries,
output pointers, domain/range errors, rounding-sensitive values, and supported
special representations. Accuracy tolerances shall be justified per function.
The shared gates in `REQUIREMENTS.md` apply.

## Relationships

- **Derived from:** §4.5 Mathematics
- **Depends on:** Shared build, ABI, and quality gates in `docs/REQUIREMENTS.md`
- **Conflicts with:** None known

## Tailoring

WCRT groups the related obligations for this C89 conformance unit under one
stable requirement identifier. Each observable obligation remains explicit in
this record and is verified collectively by `TC-0007`.

## Implementation Record

- `include/math.h` declares all 22 C89 functions and a finite binary64
  `HUGE_VAL`.
- `src/math.c` is self-contained and does not call host CRT math functions. It
  uses exact binary64 splitting, range reduction, convergent series, and Newton
  iteration.
- Ordinary finite arguments target approximately 12 significant decimal
  digits. TC-0007 uses scaled tolerances from `1e-12` to `1e-10`, depending on
  accumulated range-reduction and composition error.
- Large trigonometric arguments can lose reduction precision. C89 finite-value
  behavior is primary; NaN, infinity, and signed-zero handling are supported
  where they naturally follow binary64 operations but are not claimed as C99
  Annex F conformance.
- Domain failures return zero except `log`, which returns `-HUGE_VAL`, and set
  `EDOM`. Overflow returns signed `HUGE_VAL`; underflow returns signed zero;
  both set `ERANGE`.
- `tests/c89/math.c` contains 35 ordinary, identity, quadrant, pointer, domain,
  and range checks. Microsoft UCRT differs by reporting `ERANGE` for `log(0)`.