# ADR-0007: POSIX Compatibility Selection and File Adapters

**Content type:** Architecture decision record

**Status:** Accepted

**Date:** 2026-08-25

## Context

WPM's retired TinyCC compatibility layer mapped the POSIX spellings `stat`,
`struct stat`, `utime`, and `struct utimbuf` directly to WCRT's Microsoft
`_stat64` and `_utime64` interfaces. It also supplied POSIX error names missing
from WCRT. Those mappings made selected dependencies compile, but they did not
separate the Microsoft ABI from the POSIX source contract.

The WCRT 1.1.0 roadmap requires explicit selection of new POSIX names, strict
ISO isolation, separate Microsoft and POSIX contracts, and no unsupported
POSIX conformance claim.

## Decision Drivers

- Retire the remaining WPM-owned C-runtime compatibility definitions.
- Preserve the public Microsoft ABI shipped by WCRT 1.0.0.
- Make the origin and observable contract of each compatibility name explicit.
- Keep new POSIX names out of strict ISO builds.
- Reuse tested Windows filesystem operations without equating unlike public
  structures or standards contracts.

## Considered Options

1. Select POSIX declarations with `WCRT_POSIX` and use typed adapter functions.
2. Copy WPM's preprocessor aliases into WCRT public headers.
3. Expose POSIX names unconditionally.
4. Leave the aliases and error definitions in each consuming application.

## Decision

`WCRT_POSIX` selects WCRT's bounded POSIX source-compatibility profile. The
selector exposes only names backed by controlled requirements and tests. WCRT
does not define `_POSIX_VERSION` because this bounded Windows portability layer
does not satisfy all obligations associated with a POSIX conformance version.

`stat` and `utime` are real public functions with POSIX-facing structures.
They adapt to the existing `_stat64` and `_utime64` implementations. The
adapters keep source types distinct and copy values with checked, documented
widths. The Microsoft declarations, layouts, and x86 `_stat` compatibility
symbol remain unchanged.

`struct stat` provides the POSIX.1-2017 `st_atim`, `st_mtim`, and `st_ctim`
members at one-second resolution. It also provides direct `st_atime`,
`st_mtime`, and `st_ctime` second fields as an explicit WPM migration
extension. Both representations contain the same seconds.

WCRT uses Microsoft/TinyCC-compatible numeric values for error macros whose
symbolic names are selected from POSIX. In particular, `ENOSYS` is 40; WPM's
historical local value 38 is not retained because it conflicts with the
Windows compiler and CRT header ABI.

The implementation maps Windows creation time to `st_ctim`, reports one link,
uses zero for unavailable inode and ownership identities, and reports only the
regular-file, directory, and owner-permission bits it can derive. These are
documented Windows compatibility limits, not claims of native Unix metadata.

## Rationale

Typed adapters prevent a POSIX selector from rewriting Microsoft identifiers
or changing structure member access in combined-profile code. An explicit
selector protects the existing ISO surface. Reusing the tested Microsoft
operations keeps Windows error mapping and Windows 2000 imports consistent,
while separate POSIX tests make the intentional metadata limits visible.

## Consequences

### Positive

- WPM-style consumers no longer need local runtime or errno shims for this
  family.
- Strict ISO and Microsoft-only source remain unchanged.
- POSIX and Microsoft structures can evolve under separate requirements.
- No new Windows API import is needed.

### Negative

- The profile exposes only second-resolution timestamps.
- Windows cannot supply native POSIX ownership, inode, link, or change-time
  semantics through the current shared operation.
- Consumers must define `WCRT_POSIX` before including affected headers.

### Follow-up

- Extend the selector and POSIX manifest only through new controlled
  requirements.
- Replace placeholder metadata only when the shared descriptor/path substrate
  can provide it without breaking the 1.0 Microsoft ABI.
- Run the POSIX aggregate natively on ARM64 before claiming target completion.

## References

- [WPM compatibility inventory](wpm-tcc-compatibility-inventory.md)
- [REQ-0050](req-0050-posix-selection-errno.md)
- [REQ-0051](req-0051-posix-stat-utime.md)
- [WCRT 1.1.0 compatibility roadmap](WCRT-1.1.0-ROADMAP.md)
- [POSIX.1-2017 `stat`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/stat.html)
- [POSIX.1-2017 `utime`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/utime.html)
