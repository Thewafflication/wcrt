# REQ-0048 — Microsoft explicit-width file-time update

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft Learn documentation for
[`_utime32` and `_utime64`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/utime-utime32-utime64-wutime-wutime32-wutime64?view=msvc-170);
WPM TinyCC compatibility inventory

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement provides `_utime32`, `_utime64`, `struct __utimbuf32`, and
`struct __utimbuf64` in `<sys/utime.h>`. Generic-time, wide-path, and
descriptor-based variants are excluded.

## Requirement

- WCRT shall declare the explicit-width structures and functions with
  Microsoft-compatible field widths, layout, calling convention, and names.
- Each function shall set both the last-access and last-modification time of
  the named filesystem object from the supplied structure and return zero on
  success.
- When `times` is null, each function shall set both times to the current time.
- `_utime32` shall enforce its signed 32-bit time range and `_utime64` shall
  enforce the Microsoft 64-bit supported range.
- On failure, each function shall return `-1`, preserve no false success
  result, and set `errno` to the documented category, including `EACCES`,
  `EINVAL`, `EMFILE`, or `ENOENT` when applicable.
- Invalid parameters shall follow WCRT's Microsoft invalid-parameter policy.
- The implementation shall pass the Windows 2000 x86 import gate.

## Rationale

WPM supplies both functions for WCRT builds and legacy XP compatibility.

## Verification

**Method:** Automated ABI, behavioral, and import-table test

**References:** Planned `TC-0048`

Tests shall verify declarations, structure layout, explicit timestamps,
current-time selection, range boundaries, nonexistent and read-only paths,
null parameters, directory behavior, timestamp round trips, and imports.

## Relationships

- **Derived from:** WPM TinyCC compatibility inventory
- **Depends on:** REQ-0003 error reporting, REQ-0015 time, and filesystem APIs
- **Conflicts with:** Silently mapping both variants to one unchecked range

## Tailoring

The explicit-width narrow-path variants apply on every supported architecture.

## Implementation Record

`include/sys/utime.h` defines the explicit-width ABI and
`src/platform/windows/utime.c` uses baseline Windows file APIs with checked
ranges and error mapping. TC-0048 passes on x86 and x64 TinyCC.
