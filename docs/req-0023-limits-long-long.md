# REQ-0023 — C99 `long long` limits

**Content type:** Project requirement

**Status:** Implemented

**Source:** ISO/IEC 9899:1999 §5.2.4.2.1 and §7.10

## Scope

This requirement extends the REQ-0005 `<limits.h>` baseline with the C99
`long long` limits.

## Requirement

- In C99 and later modes, WCRT's `<limits.h>` shall define `LLONG_MIN`,
  `LLONG_MAX`, and `ULLONG_MAX` with values matching the target ABI.
- The macros shall be integer constant expressions with types suitable for
  use wherever the corresponding `long long` type is required.
- Their values shall satisfy `LLONG_MIN == -LLONG_MAX - 1` and the unsigned
  range required by the target representation without overflowing an
  intermediate signed expression.
- The additions shall not alter any macro or behavior required by REQ-0005 and
  shall not expose `long long` tokens in the C89 `<limits.h>` surface.

## Rationale

The limits are required before portable extended-integer conversions and
formatting can be specified against the actual ABI.

## Verification

**Method:** Automated compile-time test

**References:** `TC-0023`

TC-0023 shall verify presence, values, expression types, constant-expression
use, ABI agreement, C89 isolation, and REQ-0005 regression behavior.

## Relationships

- **Derived from:** C99 Milestone 2A
- **Depends on:** REQ-0005 and verified TinyCC `long long` behavior
- **Conflicts with:** Unsuffixed constants whose type cannot represent the value

## Tailoring

This extension applies only in C99 and later modes.

## Implementation Record

`include/limits.h` defines the three two's-complement 64-bit limits only for
the selected C99 surface and honors `WCRT_C89`. TC-0023 checks their values,
expression widths, constant-expression use, direct C89 isolation, and the
complete REQ-0005 regression runner. The target capability probe independently
requires an 8-byte TinyCC `long long` on every supported architecture.
