# REQ-0051 — POSIX file status and timestamp compatibility

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017
[`stat`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/stat.html)
and
[`utime`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/utime.html);
WPM TinyCC compatibility inventory

**Compatibility annotation:** Selected POSIX source and behavioral
compatibility with documented Windows metadata deviations

## Scope

This requirement provides the `WCRT_POSIX`-selected `stat` and `utime`
interfaces, their required public structures, the status types needed by this
family, and regular-file/directory mode tests. Descriptor-based functions,
nanosecond setters, links, native Unix ownership, and other file types are
excluded.

## Requirement

- `<sys/types.h>`, `<sys/stat.h>`, `<time.h>`, and `<utime.h>` shall declare
  the selected types and interfaces only when `WCRT_POSIX` is defined.
- WCRT shall provide 64-bit signed `off_t` and `time_t`, 64-bit unsigned
  `ino_t`, and documented Windows-width `dev_t`, `mode_t`, `nlink_t`, `uid_t`,
  and `gid_t` types.
- `struct stat` shall expose device, inode, mode, link, owner, group, special
  device, size, and `st_atim`/`st_mtim`/`st_ctim` timestamp members. It shall
  also expose coherent direct seconds fields for WPM migration.
- `stat` shall return zero and fully initialize `struct stat` for a
  representable regular file or directory. It shall report 64-bit size,
  second-resolution times, supported file type and owner-permission bits, one
  link, and zero for unavailable inode and ownership identities.
- On Windows, `st_ctim` and `st_ctime` shall contain creation time rather than
  native POSIX metadata-change time; nanoseconds shall be zero.
- `utime` shall set last-access and last-modification seconds from `struct
  utimbuf`, or set both to the current time when `times` is null.
- The adapter shall use the representable `_utime64` range. Negative or
  otherwise unsupported values shall fail rather than wrap or truncate.
- Failures shall return `-1`, set WCRT `errno` consistently with the shared
  Microsoft operation, and not expose partial output as a successful result.
- The functions shall add no Windows API imports beyond those already verified
  for `_stat64` and `_utime64`, and shall preserve the 1.0 Microsoft ABI.
- Strict ISO and Microsoft-only inclusion shall not expose the POSIX types,
  structures, mode macros, or function declarations.

## Rationale

WPM used preprocessor aliases for this family. Typed adapters preserve the
portable source spellings without rewriting or conflating the already shipped
Microsoft interfaces.

## Verification

**Method:** Automated header isolation, type, behavioral, boundary, filesystem,
and error test

**References:** `TC-0051`

Tests compile positive and negative header fixtures, check type widths and
function signatures, query regular-file metadata, set explicit/current times,
exercise missing and invalid arguments, and verify the documented Windows
metadata mapping.

## Relationships

- **Derived from:** WPM TinyCC compatibility inventory and ADR-0007
- **Depends on:** REQ-0003, REQ-0015, REQ-0048, REQ-0049, and REQ-0050
- **Conflicts with:** Preprocessor aliasing of POSIX and Microsoft structures

## Tailoring

This requirement is a bounded Windows portability contract, not a claim that
WCRT or Windows conforms to POSIX.1-2017. Native ARM64 execution remains a
required target gate before the release-level profile can be reported complete.

## Implementation Record

`include/sys/types.h`, `include/sys/stat.h`, `include/time.h`, and
`include/utime.h` define the selected source surface.
`src/platform/windows/posix_files.c` adapts it to the controlled Microsoft
filesystem operations without adding Windows imports.
