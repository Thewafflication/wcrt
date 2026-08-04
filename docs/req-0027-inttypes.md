# REQ-0027 — Greatest-width integer interfaces

**Content type:** Project requirement

**Status:** Implemented

**Source:** ISO/IEC 9899:1999 §7.8

## Scope

This requirement adds the C99-mode `<inttypes.h>` interface for the x86, x64,
and ARM64 Windows ABIs.

## Requirement

- The self-contained header shall include `<stdint.h>`, expose `intmax_t` and
  `uintmax_t`, and define `imaxdiv_t` with `intmax_t quot` and `intmax_t rem`
  members in that order.
- It shall declare `imaxabs`, `imaxdiv`, `strtoimax`, and `strtoumax` with the
  C99 signatures and `restrict` contracts.
- It shall define every `PRId*`, `PRIi*`, `PRIo*`, `PRIu*`, `PRIx*`, `PRIX*`,
  `SCNd*`, `SCNi*`, `SCNo*`, `SCNu*`, `SCNx*`, and pointer-width macro
  applicable to every exact-, least-, fast-, maximum-, and pointer-width type
  supplied by REQ-0022.
- Each macro shall expand to a string-literal preprocessing token whose length
  modifier matches the promoted argument or pointed-to object type on the
  selected Windows ABI. Repeated inclusion and string-literal concatenation
  shall be valid.
- `imaxabs` and `imaxdiv` shall follow the C99 integer arithmetic semantics;
  inputs for which the mathematical result is unrepresentable remain outside
  the function domain.
- `strtoimax` and `strtoumax` shall follow the C99 subject-sequence, base,
  prefix, sign, end-pointer, return-value, and range-error rules. On range
  error they shall set `errno` to `ERANGE` and return the appropriate extreme;
  successful and no-conversion calls shall not alter `errno`.
- Selecting the C89 library surface shall not expose `<inttypes.h>` types,
  macros, or declarations through a C89 header.

## Verification

**Method:** Automated compile-time, ABI, boundary-value, and behavioral test

**References:** `TC-0027`

TC-0027 shall verify header self-containment, repeated inclusion, type identity,
structure layout, all applicable macro values and expression forms, arithmetic
functions, all supported integer bases and prefixes, signs, end pointers,
exact extrema, overflow/underflow, `errno`, no-conversion behavior, C89
isolation, and target ABI agreement.

## Relationships

- **Derived from:** C99 Milestone T1
- **Depends on:** REQ-0003, REQ-0022, and REQ-0023
- **Conflicts with:** Host CRT format modifiers or typedefs that disagree with
  the selected target ABI

## Tailoring

No optional macro family is omitted because REQ-0022 supplies every associated
typedef. `wchar_t`/`wint_t` integer conversion facilities are not part of C99
§7.8. Invalid bases are outside the standard function domain; the defensive
WCRT result is documented in the T1 work record and is not a conformance claim.

## Implementation Record

`include/inttypes.h` supplies the complete macro families for every typedef
provided by REQ-0022 and selects pointer modifiers from the Windows ILP32/LLP64
model. `src/stdlib.c` supplies greatest-width arithmetic and conversions using
the same checked long-long conversion core as REQ-0028. TC-0027 checks every
macro family for presence, exact/least/fast/max/pointer type groupings,
architecture-dependent pointer spellings, arithmetic, bases 2–36 and base-zero
prefixes, end pointers, exact extrema, range errors, errno preservation, and
C89 isolation. ARM64 behavior is compile/link verified locally and requires
native CI execution for runtime closure.
