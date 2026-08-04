# REQ-0028 — C99 general utilities

**Content type:** Project requirement

**Status:** Implemented

**Source:** ISO/IEC 9899:1999 §7.20

## Scope

This requirement extends the C89 `<stdlib.h>` baseline with `_Exit`, `atoll`,
`llabs`, `lldiv`, `strtof`, `strtold`, `strtoll`, and `strtoull`.

## Requirement

- In C99 mode `<stdlib.h>` shall define `lldiv_t` with `long long quot` and
  `long long rem` members in that order and declare every scoped function with
  its C99 signature and applicable `restrict` qualifiers.
- `_Exit` shall terminate the process with the requested status without
  calling functions registered by `atexit` and without performing normal
  stream or temporary-file cleanup.
- `atoll`, `llabs`, and `lldiv` shall provide the C99 long-long counterparts of
  `atol`, `labs`, and `ldiv`; inputs whose mathematical result is not
  representable remain outside the relevant function domain.
- `strtoll` and `strtoull` shall implement the C99 subject-sequence, base,
  prefix, sign, end-pointer, return-value, and range-error rules for base zero
  and bases 2 through 36. Range errors shall set `errno` to `ERANGE` and return
  the prescribed extreme; successful and no-conversion calls shall preserve
  `errno`.
- `strtof` and `strtold` shall accept the decimal floating subject sequences
  controlled by this tranche, including optional white space, sign, decimal
  point, and decimal exponent. They shall implement correct end-pointer and
  exponent rollback behavior, preserve signed zero, return a correctly typed
  result, preserve `errno` on successful and no-conversion calls, and set
  `errno` to `ERANGE` for overflow or underflow.
- On floating overflow the conversion shall return a signed range extreme; on
  underflow it shall return the representable signed result, including signed
  zero. The x86/x64/ARM64 `long double` result follows WCRT's documented
  binary64 Windows ABI.
- C99 hexadecimal floating, infinity, and NaN subject sequences are controlled
  by planned REQ-0039 and are not attributed to this tranche.
- The C89 edition shall retain the existing `<stdlib.h>` declarations and
  behavior without exposing C99-only types or declarations.

## Verification

**Method:** Automated interface, child-process, boundary-value, and behavioral
test

**References:** `TC-0028`

TC-0028 shall verify declarations and expression types, structure layout,
integer bases/prefixes/signs/end pointers/extrema/range errors, decimal floating
grammar/exponent rollback/signed zero/representative values/overflow/underflow,
`errno` preservation, `_Exit` cleanup bypass, C89 isolation and regression,
and ABI agreement on supported targets.

## Relationships

- **Derived from:** C99 Milestone T1
- **Depends on:** REQ-0003, REQ-0004, REQ-0013, REQ-0023, and the documented
  Windows floating model
- **Related to:** REQ-0027 and planned REQ-0039

## Tailoring

Hexadecimal floating syntax and special values are staged with the unified
numeric-text requirement so their rounding and formatted-I/O integration can
be verified together. This staged ownership does not remove them from the C99
1.0.0 completion profile.

## Implementation Record

`include/stdlib.h` exposes the C99 additions only outside the selected C89
surface. `src/stdlib.c` implements checked long-long conversions, decimal
floating parsing shared with `strtod`, typed float/long-double range handling,
the long-long arithmetic utilities, and immediate process termination through
the existing Windows backend. TC-0028 checks declarations, expression and
member types, decimal grammar, exponent rollback, signed zero, normal values,
long leading fractional zeros, integer boundaries, overflow, underflow, end
pointers, errno preservation, `_Exit` callback bypass, C89 isolation, and the
REQ-0013 regression. ARM64 behavior is compile/link verified locally and
requires native CI execution for runtime closure.
