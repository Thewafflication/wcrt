# REQ-0007 — `<math.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.5 Mathematics

## Required files

- `include/math.h` — declarations and `HUGE_VAL`.
- `src/math.c` or `src/math/` — mathematical implementation.
- `tests/c89/math.c` — accuracy, boundary, domain, and range tests.

## Functions

| Draft clause | Required functions |
| --- | --- |
| §4.5.2 Trigonometric | `acos`, `asin`, `atan`, `atan2`, `cos`, `sin`, `tan` |
| §4.5.3 Hyperbolic | `cosh`, `sinh`, `tanh` |
| §4.5.4 Exponential/logarithmic | `exp`, `frexp`, `ldexp`, `log`, `log10`, `modf` |
| §4.5.5 Power | `pow`, `sqrt` |
| §4.5.6 Nearest integer, absolute value, remainder | `ceil`, `fabs`, `floor`, `fmod` |

## Requirements

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

## Acceptance

Tests shall cover ordinary values, identities, quadrant behavior, boundaries,
output pointers, domain/range errors, rounding-sensitive values, and supported
special representations. Accuracy tolerances shall be justified per function.
The shared gates in `REQUIREMENTS.md` apply.
