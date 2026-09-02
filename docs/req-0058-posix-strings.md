# REQ-0058 — POSIX case-insensitive string comparison

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017 `strcasecmp` and `strncasecmp` contracts

**Compatibility annotation:** Selected POSIX interface; not ISO C

## Scope

This requirement provides `<strings.h>`, `strcasecmp`, and `strncasecmp` only
when `WCRT_POSIX` is selected. Other POSIX strings interfaces are excluded.

## Requirement

- WCRT shall provide an independently guarded `<strings.h>` header whose
  declarations are visible only when `WCRT_POSIX` is defined.
- `strcasecmp` and `strncasecmp` shall compare strings after lowercase
  conversion under the active `LC_CTYPE` category and return less than, equal
  to, or greater than zero according to the first difference.
- Comparison shall stop at a terminating null; `strncasecmp` shall inspect no
  more than `count` bytes.
- A zero count shall return zero without accessing either pointer.
- The header shall remain repeatable in C89 and C99 modes and shall not define
  `_POSIX_VERSION` or expose the functions in the strict profile.

## Rationale

These functions are explicitly required by the selected POSIX roadmap and can
share WCRT's established case-comparison behavior without exposing Microsoft
spellings to portable source.

## Verification

**Method:** Automated presence, absence, and behavioral test

**References:** TC-0058

Tests cover repeatable header inclusion, exact declarations, C89/C99 selected
and strict profiles, case-only equality, ordering, bounds, and zero count.

## Relationships

- **Depends on:** REQ-0006, REQ-0014, REQ-0045, and REQ-0050
- **Conflicts with:** Advertising complete POSIX conformance

## Tailoring

Only the two POSIX.1-2017 case-comparison functions are included.

## Implementation Record

`include/strings.h` provides selector-gated declarations and `src/string.c`
adapts the controlled Microsoft comparison core. TC-0058 passes on x64
TinyCC; x86 and ARM64 are allocated to CI.
