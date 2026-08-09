# REQ-0022 — C99 integer types, limits, and constants

**Content type:** Project requirement

**Status:** Implemented

**Source:** ISO/IEC 9899:1999 §7.18

## Scope

This requirement adds `<stdint.h>` for the supported x86, x64, and ARM64
Windows ABIs.

### Required files

- `include/stdint.h` — public types and macros.
- `tests/c99/presence/stdint.c` — compile-time and ABI verification.
- `tests/c99/run-tc-0022.ps1` — automated runner.

## Requirement

- WCRT's self-contained `<stdint.h>` shall define every required exact-width,
  least-width, fast-width, pointer-capable, and greatest-width signed and
  unsigned integer type supported by the selected target representation.
- Exact-width types shall be provided when the target has an integer type of
  that width with no padding bits and a two's-complement representation.
- The header shall define the limits required for its types and the applicable
  `PTRDIFF_*`, `SIG_ATOMIC_*`, `SIZE_MAX`, `WCHAR_*`, and `WINT_*` macros.
- The applicable `INTN_C`, `UINTN_C`, `INTMAX_C`, and `UINTMAX_C` macros shall
  expand to integer constant expressions having the C99-prescribed promoted
  types.
- `intptr_t` and `uintptr_t`, when defined, shall round-trip every object pointer
  value through the corresponding integer type.
- WCRT shall document each optional type or macro omitted on a supported target.
- Selecting the C89 library surface shall not expose `<stdint.h>` declarations
  through a C89 header.

## Rationale

Portable-width integers establish the ABI vocabulary required by
`<inttypes.h>`, formatted I/O, and binary interfaces.

## Verification

**Method:** Automated compile-time and ABI test

**References:** `TC-0022`

TC-0022 shall verify presence, widths, signedness, limits, expression types,
constant-expression use, pointer round trips, self-containment, C89 isolation,
and agreement with each supported target ABI.

## Relationships

- **Derived from:** C99 Milestone 2A
- **Depends on:** REQ-0005, REQ-0011, and verified `long long` support
- **Conflicts with:** Host-dependent typedefs that disagree with the target ABI

## Tailoring

C99 permits exact-width and pointer-capable types to be omitted when no
conforming representation exists; every omission shall be recorded per target.

## Implementation Record

`include/stdint.h` implements the exact-, least-, fast-, pointer-, and
maximum-width types and their applicable limit and constant macros for the
documented Windows ILP32 and LLP64 models. TC-0022 checks widths, signedness,
constant properties, limits, pointer round trips, and C89 isolation on the
selected target.

REQ-0033 now baselines unsigned 32-bit `wint_t`, `WINT_MIN == 0`, and
`WINT_MAX == UINT32_MAX` on x86, x64, and ARM64. T3 adds those macros to the
C99 `<stdint.h>` surface. TC-0022 and TC-0033 pass on x86/x64 and compile/link
for ARM64 while the macros remain absent from the selected C89 surface.
