# REQ-0024 — C99 floating-point characteristics

**Content type:** Project requirement

**Status:** Implemented

**Source:** ISO/IEC 9899:1999 §5.2.4.2.2 and §7.7

## Scope

This requirement extends the REQ-0004 `<float.h>` baseline with C99
floating-point evaluation and decimal-conversion characteristics.

## Requirement

- In C99 and later modes, WCRT's `<float.h>` shall define `DECIMAL_DIG`,
  `FLT_EVAL_METHOD`, `FLT_ROUNDS`, and the applicable C99 additions with values
  matching TinyCC code generation and the selected target ABI.
- `DECIMAL_DIG` shall be sufficient for a finite value represented in the
  widest supported floating type to survive a conversion to decimal with that
  many significant digits and back without changing its value.
- `FLT_EVAL_METHOD` shall describe the range and precision used to evaluate
  floating expressions and shall not claim excess precision that the toolchain
  does not retain.
- `FLT_ROUNDS` shall describe the implementation's addition rounding direction;
  when it cannot be determined, WCRT shall use the standard indeterminate value.
- All macros shall be usable in preprocessing conditions where C99 permits it,
  and the additions shall preserve REQ-0004 behavior in C89 mode.

## Rationale

Numeric conversion, formatting, and mathematics depend on an explicit and
testable description of the compiler's floating model.

## Verification

**Method:** Automated test and engineering analysis

**References:** `TC-0024`

TC-0024 shall verify macro presence and ranges, round-trip decimal precision,
observed evaluation behavior, target consistency, C89 isolation, and REQ-0004
regression behavior. The test evidence shall identify any property established
by toolchain analysis rather than execution.

## Relationships

- **Derived from:** C99 Milestone 2A
- **Depends on:** REQ-0004 and a documented TinyCC floating model
- **Conflicts with:** Values copied from a host compiler or CRT

## Tailoring

Target-specific values are permitted, but each supported target shall publish
and verify its selected model.

## Implementation Record

`include/float.h` records the Windows TinyCC binary32 `float`, binary64
`double`, and binary64 `long double` model with `DECIMAL_DIG` 17,
`FLT_EVAL_METHOD` 0, and `FLT_ROUNDS` 1. TC-0024 checks the exact published
model, epsilon behavior, default round-to-nearest behavior, direct C89
isolation, and REQ-0004 regression behavior. The 17-digit decimal round-trip
bound is established from the documented widest precision of 53 binary
digits; execution coverage of the conversion functions themselves remains in
the formatted-I/O and conversion tranches rather than being attributed to
this header test.
