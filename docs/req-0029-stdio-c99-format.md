# REQ-0029 — C99 narrow formatted output

**Content type:** Project requirement

**Status:** Implemented; native ARM64 verification pending

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.19.6.1

## Scope

This requirement extends the REQ-0012 byte-oriented formatted-output
functions with the C99 conversion and length rules. It changes the formatting
stage used by REQ-0019 and REQ-0020 without merging their bounded-output
contracts. Wide formatted-output functions and non-C locales are excluded.

### Required files

- `include/stdio.h` — edition-correct declarations inherited by the family.
- `src/stdio_format.c` — directive parsing, typed argument retrieval, fields,
  conversions, and output sinks.
- `tests/c99/stdio-format.c` — behavioral and boundary verification.

## Requirement

- WCRT shall apply `hh`, `ll`, `j`, `z`, and `t` to every applicable `d`, `i`,
  `o`, `u`, `x`, `X`, and `n` conversion using the exact promoted argument or
  pointed-to type required by C99 on x86, x64, and ARM64.
- WCRT shall implement `a`, `A`, and `F`, including signs, alternate form,
  zero/space padding, field width, precision, case, correctly rounded
  hexadecimal output, and the specified representations of zero, infinity,
  and NaN.
- WCRT shall retrieve an `L` floating argument as `long double`. The existing
  `l` floating modifier shall have the C99-specified effect even though the
  Windows TinyCC `double` and `long double` representations are both binary64.
- In the C locale, WCRT shall implement `lc` and `ls` using the 16-bit Windows
  wide-character ABI recorded by ADR-0002. Encoding failure shall produce a
  negative formatted-output result without writing beyond a destination bound.
- WCRT shall apply the common C99 flag, width, precision, length, conversion,
  and return rules consistently to `fprintf`, `printf`, `sprintf`, `vfprintf`,
  `vprintf`, and `vsprintf`, and to the formatting stage used by `snprintf`,
  `vsnprintf`, `_snprintf`, and `_vsnprintf`.
- WCRT shall retain REQ-0019 null termination, zero-size, length-only, and
  would-have-written behavior and the distinct REQ-0020 exact-limit and
  truncation behavior.
- Caller-controlled width and precision shall not cause an out-of-bounds
  access or unchecked internal count overflow. Bounded functions shall retain
  the logical untruncated character count after storage stops.

## Rationale

The C99 integer types and format macros are not usable until the formatter
executes their modifiers. Hexadecimal floating output provides an exact and
portable representation for the documented binary model. Keeping output-sink
contracts separate prevents C99 bounded semantics from changing Microsoft
compatibility behavior.

## Verification

**Method:** Automated compile-time and behavioral test, analysis, and review

**References:** `TC-0029`

TC-0029 shall verify the complete applicable length/conversion matrix, exact
argument types, integer and field boundaries, floating classification and
rounding, C-locale character conversion, direct and `va_list` entry points,
all output sinks, and regression partitions for REQ-0012, REQ-0019, and
REQ-0020 on every supported target.

## Relationships

- **Derived from:** C99 §7.19.6.1
- **Depends on:** REQ-0012, REQ-0019, REQ-0025, REQ-0027, REQ-0039, and
  ADR-0002
- **Conflicts with:** Merging REQ-0019 and REQ-0020 truncation contracts

## Tailoring

The `l` character forms are limited to WCRT's supported C locale. Full wide
formatted output, restartable conversion APIs, stream orientation, and
additional locales remain outside this requirement.

## Implementation Record

`src/stdio_format.c` implements explicit length-modifier state, exact
promoted-argument selection, integer precision, C-locale `lc`/`ls`, decimal
rounding, and exact binary64 hexadecimal decomposition. Caller precision is
materialized only in checked heap storage; fixed conversion arrays no longer
bound numeric output, and the logical sink count is checked before conversion
to `int`.

The stream, unbounded-string, C99-bounded, and Microsoft-bounded entry points
share conversion logic but retain separate termination and return policies.
TC-0029 and the complete C89/extension aggregates pass on x86 and x64. The
same controlled source and tests compile/link with the ARM64 TinyCC package;
native ARM64 execution remains required before target-complete acceptance.

## T3 Wide-character Implementation Impact

REQ-0033 supersedes ADR-0002's anticipated 16-bit `wint_t` with an unsigned
32-bit type. T3 routes `lc` and `ls` through the accepted C-locale conversion
rules and retrieves `%lc` through the exact promotion-safe `wint_t` variadic
type. The complete narrow character/string and encoding-failure matrix passes
on x86/x64 and compiles/links for ARM64. Wide stream functions remain owned by
REQ-0031 rather than this requirement.
