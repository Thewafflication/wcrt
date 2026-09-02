# REQ-0055 — Microsoft in-place narrow-string character replacement

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft CRT `_strset` and `_strnset` compatibility contracts

**Compatibility annotation:** Microsoft CRT extensions; not ISO C

## Scope

This requirement provides `_strset` and `_strnset`. Secure, locale-object,
wide, multibyte, and non-underscored spellings are excluded.

## Requirement

- WCRT shall declare both interfaces in `<string.h>` and return the original
  string pointer on success.
- `_strset` shall replace every byte before the terminating null with the
  low-byte character value without changing string length or termination.
- `_strnset` shall replace no more than `count` bytes and stop at the first
  terminating null; count zero shall not modify the string.
- Empty strings shall remain unchanged.
- A null string shall return null and set `errno` to `EINVAL`.

## Rationale

These VC-era helpers complete another bounded part of roadmap MS1 using the
existing byte-string substrate.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** TC-0055

Tests cover declarations, full and bounded replacement, zero/exact/excessive
counts, empty strings, termination, return identity, and null errors.

## Relationships

- **Depends on:** REQ-0003 and REQ-0014
- **Conflicts with:** Treating these names as ISO C interfaces

## Tailoring

Only the two narrow Microsoft interfaces are included.

## Implementation Record

`include/string.h` declares both functions and `src/string.c` implements the
bounded operation as the common primitive. TC-0055 passes on x64 TinyCC; x86
and ARM64 are allocated to CI.
