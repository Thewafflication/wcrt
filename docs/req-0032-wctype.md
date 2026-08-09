# REQ-0032 — C99 wide-character classification and mapping

**Content type:** Project requirement

**Status:** Implemented locally; native ARM64 verification pending

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.25

## Scope

This requirement supplies the complete `<wctype.h>` surface for WCRT's C
locale, including named descriptors. Its scalar ABI and encoding behavior are
controlled by REQ-0033.

### Required files

- `include/wctype.h` — self-contained public declarations and C89 isolation.
- `src/wctype.c` — classification, descriptors, and mappings.
- `tests/c99/wctype.c` and header-presence tests — complete matrix verification.

## Requirement

- `<wctype.h>` shall define `wint_t`, `wctrans_t`, and `wctype_t`, expose
  `WEOF`, be self-contained and repeatable, and expose no C99 additions in the
  selected C89 surface.
- WCRT shall implement `iswalnum`, `iswalpha`, `iswblank`, `iswcntrl`,
  `iswdigit`, `iswgraph`, `iswlower`, `iswprint`, `iswpunct`, `iswspace`,
  `iswupper`, and `iswxdigit` for every `unsigned char` value converted to
  `wint_t` and for `WEOF`. Results shall agree with the corresponding C-locale
  narrow classification where one exists; `iswblank` shall recognize space
  and horizontal tab.
- `wctype` shall recognize exactly the standard class names and return zero
  for null, empty, unknown, or case-mismatched names. `iswctype` shall apply a
  valid descriptor and return zero for descriptor zero.
- WCRT shall implement `towlower` and `towupper`; values without a C-locale
  mapping, including `WEOF` and values outside the byte repertoire, shall be
  returned unchanged.
- `wctrans` shall recognize exactly `tolower` and `toupper` and return zero for
  null, empty, unknown, or case-mismatched names. `towctrans` shall apply a
  valid descriptor and return its input unchanged for descriptor zero.
- Descriptor representations shall be target-invariant values, not pointers
  to process-lifetime mutable state. Calls shall not alter `errno` for ordinary
  false classifications, unknown names, `WEOF`, or unmapped values.
- Every declaration and descriptor ABI shall compile for x86, x64, and ARM64,
  and existing `<ctype.h>` behavior shall remain unchanged.

## Rationale

The supported C locale has a finite, deterministic mapping. Stable integral
descriptors avoid pointer-size ABI differences and make every classification
and mapping decision directly testable across architectures.

## Verification

**Method:** Automated exhaustive byte-repertoire test, boundary test, ABI
inspection, regression, and review

**References:** `TC-0032`

TC-0032 shall compare all twelve predicates over `0x00` through `0xff`, verify
`WEOF` and representative out-of-repertoire values, exercise every standard
and invalid descriptor name, apply both mappings directly and through
descriptors, confirm target sizes and signedness, and rerun narrow `<ctype.h>`
tests. Native ARM64 behavior evidence is required for target completion.

## Relationships

- **Derived from:** C99 §7.25
- **Depends on:** REQ-0002, REQ-0006, REQ-0033, and ADR-0003
- **Conflicts with:** Locale-dependent mutable descriptors or classifying
  arbitrary UTF-16 code units as if they were C-locale bytes

## Tailoring

Only the C locale is implemented. Values outside its byte repertoire have no
classification and no case mapping. This is an implementation-defined locale
choice, not a claim of Unicode property support.

## Implementation Record

`include/wctype.h` owns the target-invariant unsigned 16-bit descriptor types.
`src/wctype.c` implements the twelve predicates, two direct mappings, the
standard class and mapping name lookups, and descriptor application. Integral
descriptor values are stable across x86, x64, and ARM64 and require no mutable
process-lifetime registry.

TC-0032 exhaustively compares the complete byte repertoire with the C-locale
narrow classifications, covers `WEOF`, UTF-16 and other out-of-repertoire
values, all valid and invalid names, and direct/descriptor mapping
equivalence. It passes natively on x86 and x64 and compiles/links for ARM64.
Native ARM64 execution, immutable exact-revision CI evidence, and independent
review remain required before target-complete acceptance.
