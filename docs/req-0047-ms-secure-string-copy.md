# REQ-0047 — Microsoft secure narrow-string copy

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft Learn documentation for
[`strcpy_s`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strcpy-s-wcscpy-s-mbscpy-s?view=msvc-170)
and
[`strncpy_s`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strncpy-s-strncpy-s-l-wcsncpy-s-wcsncpy-s-l-mbsncpy-s-mbsncpy-s-l?view=msvc-170);
WPM TinyCC compatibility inventory

**Compatibility annotation:** Microsoft secure CRT contract; Annex K support
is not implied

## Scope

This requirement provides `strcpy_s` and `strncpy_s` for narrow strings. Wide,
multibyte, locale-specific, concatenation, and C++ template overloads are
excluded.

## Requirement

- WCRT shall define Microsoft-compatible `errno_t` and `rsize_t` types and
  declare the two functions in `<string.h>` with the Microsoft C signatures.
- `strcpy_s` shall copy the complete source including its terminating null
  character when it fits and shall return zero.
- `strncpy_s` shall copy no more than `count` source characters, shall null
  terminate every successful result, and shall implement the Microsoft
  `_TRUNCATE` behavior when that constant is selected.
- On a null source or an invalid or insufficient destination size, each
  function shall return the documented `EINVAL` or `ERANGE` value and set
  `destination[0]` to null when the destination and its size permit access.
- A null destination shall not be accessed. Overlapping source and destination
  ranges are outside the supported contract.
- Constraint failures shall invoke WCRT's Microsoft invalid-parameter policy.
- These interfaces shall not be presented as evidence of C11 Annex K
  conformance.

## Rationale

WPM currently carries local versions to compile and run with TinyCC/WCRT.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** Planned `TC-0047`

Tests shall cover exact-fit, spare capacity, empty source, count zero, count
shorter and longer than the source, `_TRUNCATE`, insufficient capacity, null
arguments, destination clearing, return codes, and excluded overloads.

## Relationships

- **Derived from:** WPM TinyCC compatibility inventory
- **Depends on:** REQ-0003 error reporting and REQ-0014 string handling
- **Conflicts with:** Inferring complete Annex K support

## Tailoring

Only the two named narrow C interfaces are included.

## Implementation Record

`include/string.h` defines the compatibility types and constants and
`src/string.c` implements both functions, including `_TRUNCATE`. TC-0047
passes on x86 and x64 TinyCC.
