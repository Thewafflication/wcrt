# REQ-0060 — POSIX process identification

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017 `pid_t` and `getpid` contracts

**Compatibility annotation:** Selected POSIX interface; not ISO C

## Scope

This requirement provides `pid_t`, `<unistd.h>`, and `getpid` only when
`WCRT_POSIX` is selected. Other process, user, and session interfaces are
excluded.

## Requirement

- WCRT shall define `pid_t` as a signed integer type capable of representing
  every unsigned 32-bit Windows process identifier.
- WCRT shall declare `getpid(void)` in `<unistd.h>` only when `WCRT_POSIX` is
  defined.
- `getpid` shall return the positive, stable Windows identifier of the calling
  process without truncation.
- `<unistd.h>` shall be independently guarded and repeatable in C89 and C99
  modes; `pid_t` and `getpid` shall remain absent from the strict profile.
- The selected header shall not define `_POSIX_VERSION` or advertise other
  unimplemented `<unistd.h>` interfaces.
- The implementation shall use `GetCurrentProcessId`, which is within WCRT's
  Windows 2000 x86 import baseline.

## Rationale

Process identification is an explicit, self-contained member of the selected
POSIX roadmap. A signed 64-bit `pid_t` avoids narrowing Windows' DWORD value.

## Verification

**Method:** Automated presence, absence, behavioral, and import testing

**References:** TC-0060

Tests cover the type width, exact declaration, repeatable C89/C99 selected and
strict headers, positive stable results, and the existing x86 import gate.

## Relationships

- **Depends on:** REQ-0050 and the Windows 2000 import contract
- **Conflicts with:** Advertising complete POSIX conformance

## Tailoring

Only `pid_t` and `getpid` are included.

## Implementation Record

`include/sys/types.h` defines the selected type, `include/unistd.h` declares
the selected function, and `src/platform/windows/process.c` adapts
`GetCurrentProcessId`. TC-0060 passes on x64 TinyCC; x86 and ARM64, including
the x86 import audit, are allocated to CI.
