# REQ-0062 — Microsoft filesystem-name removal

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft CRT `_unlink` compatibility contract

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement provides `_unlink` in `<io.h>`. Directory removal and wide
path variants are excluded.

## Requirement

- WCRT shall declare `_unlink(const char *)` in `<io.h>` and return zero after
  successfully removing the selected filesystem name.
- A missing file or parent path shall return -1 with `errno` set to `ENOENT`.
- A null path shall return -1 with `errno` set to `EINVAL`.
- Access denial, sharing violations, and otherwise unclassified Windows
  deletion failures shall return -1 with `errno` set to `EACCES`.
- The implementation shall share WCRT's hardened file-removal primitive with
  ISO `remove` and selected POSIX `unlink`.
- The implementation shall retain the Windows 2000 x86 import contract.

## Rationale

`_unlink` is part of the roadmap's required VC-era low-level file family. The
shared primitive keeps its Windows error mapping aligned with the existing
POSIX adapter.

## Verification

**Method:** Automated compile-time, behavioral, error, and import test

**References:** TC-0062

Tests cover the exact declaration, successful removal, missing and null paths,
and the existing x86 import gate.

## Relationships

- **Depends on:** REQ-0003, REQ-0012, REQ-0043, and REQ-0061
- **Conflicts with:** Treating this name as an ISO C interface

## Tailoring

Only narrow regular filesystem-name removal is included.

## Implementation Record

`include/io.h` declares `_unlink`; `src/platform/windows/file.c` adapts the
shared removal primitive; and TC-0062 exercises the interface on x64 TinyCC.
x86 and ARM64, including the x86 import audit, are allocated to CI.
