# REQ-0059 — POSIX string duplication

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017 `strdup` and `strndup` contracts

**Compatibility annotation:** Selected POSIX interface; not ISO C

## Scope

This requirement provides `strdup` and `strndup` in `<string.h>` only when
`WCRT_POSIX` is selected. Wide and multibyte variants are excluded.

## Requirement

- WCRT shall declare `strdup(const char *)` and `strndup(const char *, size_t)`
  in `<string.h>` only when `WCRT_POSIX` is defined.
- The function shall allocate enough WCRT-managed storage for the source and
  its terminating null, copy the complete string, and return the new pointer.
- `strndup` shall copy at most the requested number of source bytes, stop at an
  earlier null, and always append a terminating null to successful output.
- The returned object shall be independent of the source and releasable with
  WCRT `free`.
- An allocation failure shall return null and set `errno` to `ENOMEM`.
- The declaration shall remain repeatable in C89 and C99 modes, remain absent
  from the strict profile, and shall not imply `_POSIX_VERSION`.

## Rationale

`strdup` is explicitly required by the selected POSIX roadmap. Sharing the
controlled `_strdup` implementation preserves one allocator and ownership
contract while keeping portable source spelling selector-gated.

## Verification

**Method:** Automated presence, absence, and behavioral test

**References:** TC-0059

Tests cover repeatable header inclusion, exact declaration, C89/C99 selected
and strict profiles, complete and bounded content, allocation independence,
zero limits, empty strings, and WCRT `free` ownership.

## Relationships

- **Depends on:** REQ-0013, REQ-0014, REQ-0050, and REQ-0054
- **Conflicts with:** Exposing POSIX names in strict ISO profiles

## Tailoring

Only the narrow POSIX duplication functions are included.

## Implementation Record

`include/string.h` provides the selector-gated declarations and `src/string.c`
implements them with WCRT allocation. TC-0059 passes on x64 TinyCC; x86 and
ARM64 are allocated to CI.
