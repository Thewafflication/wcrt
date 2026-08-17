# REQ-0040 — Existing-header C99 integration

**Content type:** Project requirement

**Status:** Baselined for T6; implementation verification pending

**Source:** ISO/IEC 9899:1999 as corrected through TC3, clauses 7.1--7.25

## Scope

This requirement closes every C99 change to a header or runtime family that
entered WCRT through the C89 baseline. It covers declarations, types, structure
members, macros, constraints, observable behavior, C89 isolation, and the
interactions among related families. New C99-only headers remain controlled by
their existing requirements and are cross-checked by REQ-0042.

## Requirement

- In C99 mode, `assert` diagnostics shall identify the expression, source file,
  source line, and enclosing function; `<ctype.h>` shall provide `isblank` with
  unsigned-character/EOF behavior; and `<locale.h>` shall provide all C99
  `struct lconv` international monetary members.
- Every C99 addition to `<errno.h>`, `<float.h>`, `<limits.h>`, `<math.h>`,
  `<signal.h>`, `<stdarg.h>`, `<stddef.h>`, `<stdio.h>`, `<stdlib.h>`,
  `<string.h>`, and `<time.h>` shall have the required declaration, expression
  type, constant-expression properties, constraints, and behavior. In
  particular `MB_CUR_MAX` shall have type `size_t`.
- Narrow/wide numeric text, formatted I/O, multibyte state, locale, character,
  string, time, math, and floating-environment behavior shall agree at their
  shared boundaries. Implementation-defined accuracy and locale choices shall
  be documented; recommended practice shall not be presented as mandatory.
- C99 additions shall not alter the selected C89 header surface or the passing
  C89 runtime families. Each correction shall rerun its focused reproducer,
  affected C89 case, dependent C99 cases, target ABI/consumer checks, and the
  x86 Windows 2000 import gate when runtime code changes.
- No function may be satisfied by an undeclared host-CRT dependency.

## Verification

**Method:** Declaration and constant-expression compilation, behavioral
testing, cross-family boundary vectors, C89 regression, and PE import review

**References:** `TC-0040`

TC-0040 shall exercise the specifically changed C99 facilities and validate an
inventory mapping every changed C89 header/runtime family to existing focused
tests. Required native target results are x86, x64, and ARM64; compile/link-only
results do not close native behavior.

## Relationships

- **Derived from:** C99 clauses 7.1--7.25 and corrected library requirements
- **Depends on:** REQ-0001 through REQ-0015 and REQ-0019 through REQ-0039
- **Required by:** REQ-0041 and REQ-0042
- **Conflicts with:** C99 names leaking into the selected C89 surface

## Tailoring

WCRT supports only the C locale for 1.0.0 and documents Windows binary32/
binary64 and UTF-16 choices. Annex F is not claimed. Optional facilities and
implementation-defined choices are classified in the REQ-0041 profile.
