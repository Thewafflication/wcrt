# REQ-0046 — Microsoft secure narrow stdio

**Content type:** Project requirement

**Status:** Proposed

**Source:** Microsoft Learn documentation for
[`fopen_s`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s?view=msvc-170)
and
[`sscanf_s`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/sscanf-s-sscanf-s-l-swscanf-s-swscanf-s-l?view=msvc-170);
WPM TinyCC compatibility inventory

**Compatibility annotation:** Microsoft secure CRT contract; Annex K support
is not implied

## Scope

This requirement provides the narrow-character `fopen_s` and `sscanf_s`
interfaces required by WPM. Wide, locale-specific, stream-scanning, and other
secure stdio families are excluded.

## Requirement

- WCRT shall define the Microsoft-compatible `errno_t` type and declare
  `fopen_s(FILE **, const char *, const char *)` in `<stdio.h>`.
- On success, `fopen_s` shall return zero and store the opened stream through
  its first argument. On an open failure it shall return the applicable error
  number, set `errno`, and store a null pointer when the output pointer is
  valid. Invalid-parameter cases shall preserve the output as documented by
  Microsoft.
- `fopen_s` shall support every filename and mode accepted by WCRT's `fopen`.
- WCRT shall declare `sscanf_s(const char *, const char *, ...)` in
  `<stdio.h>` and shall support the same conversions and assignment-count
  rules as `sscanf`, subject to the additional size arguments required by the
  Microsoft contract for character, string, and scanset destinations.
- `sscanf_s` shall validate each required destination size before writing that
  destination and shall apply the documented Microsoft failure and return
  behavior when a size is absent, zero, or insufficient.
- Invalid parameters shall invoke WCRT's Microsoft invalid-parameter policy;
  continuation behavior shall return the documented error result and set
  `errno` where the Microsoft contract requires it.
- These interfaces shall not be presented as evidence of C11 Annex K
  conformance.

## Rationale

WPM currently supplies reduced substitutes. In particular, its `sscanf_s`
fallback only recognizes the format `%llu%c`; WCRT needs a documented public
contract rather than preserving that application-specific limitation.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** Planned `TC-0046`

Tests shall cover successful and failed opens, output-pointer clearing, error
codes, every WCRT-supported scan conversion, assignment suppression, matching
and input failures, secure size arguments, invalid parameters, and the WPM
`%llu%c` use case.

## Relationships

- **Derived from:** WPM TinyCC compatibility inventory
- **Depends on:** REQ-0003 error reporting and REQ-0012 stdio
- **Conflicts with:** Claiming Annex K solely from Microsoft-compatible names

## Tailoring

Only the two named narrow interfaces are included.

## Implementation Record

Not yet implemented in WCRT. WPM supplies local reduced implementations in
`wpm/tcc_compat/secure_crt.c`.
