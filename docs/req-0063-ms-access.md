# REQ-0063 — Microsoft path accessibility

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft CRT `_access` compatibility contract

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement provides narrow `_access` in `<io.h>`. Wide-path and secure
variants are excluded.

## Requirement

- WCRT shall declare `_access(const char *, int)` in `<io.h>`.
- Mode 0 shall test existence; modes 2, 4, and 6 shall respectively test the
  selected write, read, and combined permissions represented by WCRT metadata.
- Success shall return zero. A missing path shall return -1 with `ENOENT`, and
  an unavailable requested permission shall return -1 with `EACCES`.
- Null paths and modes containing bits outside 2 and 4 shall return -1 with
  `errno` set to `EINVAL`.
- The implementation shall share `_stat64` path handling, metadata mapping,
  and Windows 2000-compatible imports.

## Rationale

`_access` is part of the required VC-era low-level file family. Reusing the
controlled metadata adapter keeps permission interpretation consistent.

## Verification

**Method:** Automated compile-time, behavioral, error, and import test

**References:** TC-0063

Tests cover the exact declaration, all four valid modes, invalid mode bits,
missing and null paths, and the existing x86 import gate.

## Relationships

- **Depends on:** REQ-0003, REQ-0043, REQ-0049, and REQ-0062
- **Conflicts with:** Treating Windows attributes as complete POSIX permissions

## Tailoring

Only the four Microsoft mode combinations are included.

## Implementation Record

`include/io.h` declares `_access`; `src/platform/windows/stat.c` implements it
through `_stat64`; and TC-0063 passes on x64 TinyCC. x86 and ARM64, including
the x86 import audit, are allocated to CI.
