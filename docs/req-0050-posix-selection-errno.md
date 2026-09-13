# REQ-0050 — POSIX compatibility selection and error names

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017
[`<errno.h>`](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html);
Microsoft Learn
[`errno` constants](https://learn.microsoft.com/en-us/cpp/c-runtime-library/errno-constants);
WPM TinyCC compatibility inventory

**Compatibility annotation:** Explicitly selected POSIX source compatibility
with Microsoft/TinyCC numeric ABI values

## Scope

This requirement defines the selector and error-name foundation needed by the
retired WPM compatibility layer. It applies to TinyCC C89/C99 consumers on all
supported Windows architectures. It does not assert POSIX conformance or add
`_POSIX_VERSION`.

## Requirement

- WCRT shall expose new POSIX declarations and constants only when the
  consumer defines `WCRT_POSIX` before including the affected header.
- `<errno.h>` shall expose `EAGAIN`, `EFBIG`, `EINTR`, `EIO`, `ENOSYS`,
  `ENXIO`, and `EPERM` when `WCRT_POSIX` is selected.
- `<errno.h>` shall expose the Microsoft-compatible filesystem and descriptor
  names `EBADF`, `EBUSY`, `EEXIST`, `ENOTDIR`, `EISDIR`, `ENOSPC`, `ESPIPE`,
  `EPIPE`, and `ENOTEMPTY` to every compatibility profile.
- The constants shall have the selected Microsoft/TinyCC Windows numeric
  values 9, 16, 17, 20, 21, 28, 29, 32, and 41, respectively, in addition to
  the selected POSIX values 11, 27, 4, 5, 40, 6, and 1.
- Strict ISO and Microsoft-only inclusion shall not expose those newly added
  names.
- WCRT shall not define `_POSIX_VERSION` for this bounded compatibility
  profile.
- `<ctype.h>` shall expose `isascii` only under `WCRT_POSIX`, backed by the
  Microsoft-compatible `_isascii` spelling, and classify exactly 0 through
  127 as ASCII.
- `<time.h>` shall define `_TIME_T_DEFINED` after declaring WCRT's `time_t` so
  subsequent Microsoft/TinyCC headers do not redeclare the type.
- All affected headers shall remain self-contained, repeatable, and valid in
  the supported C89 and C99 compiler modes.

## Rationale

WPM formerly supplied these definitions to every WCRT-backed TinyCC build.
Moving selected source compatibility into WCRT removes application-local CRT
shims while preserving strict ISO isolation and Windows numeric compatibility.

## Verification

**Method:** Automated positive/negative compile and constant-value test

**References:** `TC-0050`

The test compiles repeated-inclusion fixtures with and without `WCRT_POSIX`,
checks every numeric value, the time-type guard, and the selected `isascii`
declaration, and proves that the strict profile contains neither the
POSIX-selected names nor `_POSIX_VERSION`.

## Relationships

- **Derived from:** WPM TinyCC compatibility inventory and ADR-0007
- **Depends on:** REQ-0003 error reporting and REQ-0015 time
- **Conflicts with:** Unconditional POSIX exposure or WPM's `ENOSYS` value 38

## Tailoring

The profile is a bounded Windows portability layer. Symbolic POSIX names use
the selected Microsoft/TinyCC numeric ABI where POSIX specifies no number.

## Implementation Record

`include/errno.h` gates the POSIX-only names with `WCRT_POSIX`, exposes the
shared Microsoft-compatible filesystem and descriptor names, and
`include/time.h` publishes the interoperation guard after `time_t`.
`include/ctype.h` gates `isascii` while retaining `_isascii` for Microsoft
compatibility.
