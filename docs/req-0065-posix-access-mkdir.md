# REQ-0065 — Selected POSIX path and directory adapters

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017 `access` and `mkdir` contracts

**Compatibility annotation:** Selected POSIX adapters with documented Windows
permission semantics

## Scope

This requirement provides narrow `access`, `mkdir`, `getcwd`, and `realpath`
plus bounded narrow-path constants when `WCRT_POSIX` is selected. Recursive creation,
access-control-list emulation, and wide paths are excluded.

## Requirement

- `<unistd.h>` shall declare `access(const char *, int)` only under
  `WCRT_POSIX`.
- `<unistd.h>` shall declare `getcwd(char *, size_t)` and `<stdlib.h>` shall
  declare `realpath(const char *, char *)` only under `WCRT_POSIX`.
- `<sys/stat.h>` shall declare `mkdir(const char *, mode_t)` only under
  `WCRT_POSIX`.
- `<limits.h>` shall always expose the Microsoft-compatible `MAX_PATH` value
  260 and expose `PATH_MAX` with that value only under `WCRT_POSIX`.
- Selected `<sys/param.h>` shall define `MAXPATHLEN` consistently with
  `PATH_MAX`; strict inclusion shall not expose either POSIX spelling.
- `access` shall provide the existence, read, write, and combined tests already
  defined by the shared Microsoft `_access` operation.
- `mkdir` shall create one directory through `_mkdir`. Windows shall ignore the
  requested POSIX permission bits rather than claim to apply them.
- `getcwd` shall expose the shared narrow Windows current directory. `realpath`
  shall require an existing path and return its absolute narrow Windows form;
  a null output pointer shall request an allocated result. Windows 2000 does
  not provide final-handle path expansion, so reparse points are not promised
  to be canonicalized.
- Existing, missing-parent, null-path, invalid-mode, and inaccessible targets
  shall preserve the deterministic errors of the shared operations.
- Strict inclusion shall expose neither function and WCRT shall not advertise
  `_POSIX_VERSION`.

## Verification

**Method:** Automated declaration, isolation, behavior, and error testing

**References:** TC-0065

Tests compile repeated selected and strict inclusions in C89 and C99, verify
the path-limit spellings, exercise access and directory creation, query the
current directory, resolve existing paths into caller and allocated buffers,
and check invalid, missing, and existing targets.

## Relationships

- **Depends on:** REQ-0050, REQ-0051, REQ-0061, REQ-0063, and REQ-0064
- **Conflicts with:** Claiming that Windows applies POSIX creation mode bits

## Tailoring

These functions are bounded Windows portability adapters, not a POSIX
conformance claim.

## Implementation Record

`include/unistd.h` and `include/sys/stat.h` declare the selected adapters;
`src/platform/windows/posix_files.c` delegates to the controlled Microsoft
operations; and TC-0065 passes on x64 TinyCC. x86 and ARM64 are allocated to CI.
