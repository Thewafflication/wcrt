# REQ-0057 — Microsoft case-insensitive memory comparison

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft CRT `_memicmp` compatibility contract

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement provides the narrow-byte `_memicmp` interface. Locale-object,
wide, and multibyte variants are excluded.

## Requirement

- WCRT shall declare `_memicmp(const void *, const void *, size_t)` in
  `<string.h>`.
- The function shall compare exactly `count` bytes after lowercase conversion
  under the active `LC_CTYPE` category, including bytes after embedded nulls.
- It shall return a value less than, equal to, or greater than zero according
  to the first differing converted byte.
- A zero count shall return zero without accessing or validating either
  pointer.
- A null pointer with a nonzero count shall return `_NLSCMPERROR` and set
  `errno` to `EINVAL`.

## Rationale

The helper completes the bounded memory member of the roadmap's selected
VC-era case-insensitive comparison family.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** TC-0057

Tests cover the exact declaration, case-only equality, ordering, embedded
nulls, high bytes, bounded prefixes, zero length, and invalid pointers.

## Relationships

- **Depends on:** REQ-0003, REQ-0006, and REQ-0014
- **Related to:** REQ-0045
- **Conflicts with:** Treating this name as an ISO C interface

## Tailoring

Only the narrow non-locale interface is included.

## Implementation Record

`include/string.h` declares `_memicmp`; `src/string.c` implements the bounded
comparison with unsigned-byte case conversion. TC-0057 passes on x64 TinyCC;
x86 and ARM64 are allocated to CI.
