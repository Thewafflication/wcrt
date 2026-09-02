# REQ-0056 — Microsoft integer-to-string conversions

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft CRT `_itoa`, `_ltoa`, and `_ultoa` compatibility
contracts

**Compatibility annotation:** Microsoft CRT extensions; not ISO C

## Scope

This requirement provides the narrow `_itoa`, `_ltoa`, and `_ultoa`
interfaces. Secure, wide, and 64-bit variants are excluded.

## Requirement

- WCRT shall declare all three interfaces in `<stdlib.h>` and return the
  caller-provided buffer on success.
- Radices from 2 through 36 shall produce lowercase digits `0` through `9`
  and `a` through `z`, followed by a terminating null.
- `_itoa` and `_ltoa` shall prefix a minus sign for negative values only when
  the radix is 10. Other radices shall convert the corresponding unsigned
  bit pattern.
- `_ultoa` shall convert the complete unsigned-long range without a sign.
- Radices outside 2 through 36 and null buffers shall return null and set
  `errno` to `EINVAL`.
- The caller shall provide enough storage; these legacy interfaces do not
  receive a buffer-size argument.

## Rationale

These VC-era conversions satisfy the roadmap's selected numeric conversion
helper family without conflating their Microsoft contract with ISO parsing.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** TC-0056

Tests cover exact declarations, return identity, zero, signed and unsigned
boundaries, decimal sign handling, binary and maximum-radix output, lowercase
digits, invalid radices, and null buffers.

## Relationships

- **Depends on:** REQ-0003 and REQ-0013
- **Conflicts with:** Treating these names as ISO C interfaces

## Tailoring

Only the three unbounded narrow 32-bit interfaces are included.

## Implementation Record

`include/stdlib.h` declares the interfaces and `src/stdlib.c` implements a
shared checked-radix conversion core. TC-0056 passes on x64 TinyCC; x86 and
ARM64 are allocated to CI.
