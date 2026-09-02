# REQ-0053 — Microsoft in-place narrow-string transformations

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft CRT `_strlwr`, `_strupr`, and `_strrev` compatibility
contracts

**Compatibility annotation:** Microsoft CRT extensions; not ISO C

## Scope

This requirement provides the narrow-character `_strlwr`, `_strupr`, and
`_strrev` interfaces. Secure, locale-object, wide, multibyte, and
non-underscored spellings are excluded.

## Requirement

- WCRT shall declare all three functions in `<string.h>` with the Microsoft C
  signatures and return the original string pointer on success.
- `_strlwr` and `_strupr` shall convert each character in place using the
  active `LC_CTYPE` behavior and preserve the terminating null character.
- `_strrev` shall reverse the bytes preceding the terminating null character
  in place; empty and one-character strings shall remain unchanged.
- A null argument shall return null and set `errno` to `EINVAL` under WCRT's
  Microsoft invalid-parameter continuation policy.
- The functions shall not allocate storage or expose additional aliases.

## Rationale

These common VC-era source-compatibility helpers advance roadmap MS1 without
changing an ISO interface or introducing an allocation ABI.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** TC-0053

Tests cover exact declarations, return identity, mixed case, digits and
punctuation, empty and one-character strings, even and odd reversal, and null
errors.

## Relationships

- **Depends on:** REQ-0003, REQ-0006, and REQ-0014
- **Conflicts with:** Treating these names as ISO C interfaces

## Tailoring

Only the three named narrow Microsoft interfaces are included.

## Implementation Record

`include/string.h` declares the interfaces and `src/string.c` implements them
using unsigned-character classification. TC-0053 passes on x64 TinyCC; x86 and
ARM64 execution is allocated to CI.
