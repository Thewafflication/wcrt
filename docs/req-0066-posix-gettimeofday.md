# REQ-0066 — POSIX wall-clock time

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017 `gettimeofday` and `<sys/time.h>`

**Compatibility annotation:** Selected POSIX interface on the Windows FILETIME
epoch

## Scope

This requirement provides `struct timeval`, `suseconds_t`, and `gettimeofday`
when `WCRT_POSIX` is selected. Obsolete timezone reporting and time setters are
excluded.

## Requirement

- `<sys/time.h>` shall expose `struct timeval` and `gettimeofday` only under
  `WCRT_POSIX`; `<sys/types.h>` shall provide signed `suseconds_t` there.
- Seconds shall use WCRT's 64-bit `time_t`; microseconds shall be in the range
  zero through 999999.
- `gettimeofday` shall derive both components from one Windows FILETIME sample
  using only Windows 2000-compatible APIs.
- The ignored timezone argument shall not affect the result.
- A null result shall return -1 and set `errno` to `EINVAL`.

## Verification

**Method:** Automated declaration, isolation, range, wall-clock, and error test

**References:** TC-0066

## Relationships

- **Depends on:** REQ-0003, REQ-0015, and REQ-0050
- **Conflicts with:** Narrowing epoch seconds to a 32-bit `long`

## Tailoring

The interface is selected source compatibility, not a POSIX conformance claim.

## Implementation Record

`include/sys/time.h` declares the selected surface and the Windows clock backend
converts one FILETIME sample. TC-0066 passes on x64; other targets remain CI
gates.
