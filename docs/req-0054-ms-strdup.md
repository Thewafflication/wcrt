# REQ-0054 — Microsoft allocated narrow-string duplication

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft CRT `_strdup` compatibility contract

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement provides `_strdup(const char *)`. Wide, multibyte, debug-heap,
and non-underscored aliases are excluded.

## Requirement

- WCRT shall declare `_strdup` in `<string.h>`.
- On success it shall allocate `strlen(source) + 1` bytes with WCRT `malloc`,
  copy the complete terminated string, and return independently owned storage
  releasable with WCRT `free`.
- Empty strings shall produce a distinct allocated empty string.
- A null source shall return null and set `errno` to `EINVAL`.
- Allocation failure shall return null and set `errno` to `ENOMEM`.

## Rationale

Allocated duplication is a common VC-era source-compatibility helper and
advances roadmap MS1 while keeping ownership within WCRT's heap boundary.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** TC-0054

Tests cover the declaration, content, distinct storage, source/duplicate
independence, empty strings, release with `free`, and null errors.

## Relationships

- **Depends on:** REQ-0003, REQ-0013, and REQ-0014
- **Conflicts with:** Exposing the POSIX `strdup` alias in strict ISO mode

## Tailoring

Only the narrow Microsoft spelling is included.

## Implementation Record

`include/string.h` declares `_strdup`; `src/string.c` allocates and copies with
WCRT heap primitives. TC-0054 passes on x64 TinyCC; x86 and ARM64 are allocated
to CI.
