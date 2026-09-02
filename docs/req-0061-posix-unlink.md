# REQ-0061 — POSIX filesystem-name removal

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017 `unlink` contract and Windows `DeleteFileA`

**Compatibility annotation:** Selected POSIX interface with documented
Windows semantics; not ISO C

## Scope

This requirement provides `unlink` in `<unistd.h>` only when `WCRT_POSIX` is
selected. Directory removal and Unix open-file deletion semantics are
excluded.

## Requirement

- WCRT shall declare `unlink(const char *)` in `<unistd.h>` only when
  `WCRT_POSIX` is defined.
- A successful call shall remove the selected filesystem name and return zero.
- A missing file or parent path shall return -1 with `errno` set to `ENOENT`.
- A null path shall return -1 with `errno` set to `EINVAL`.
- Windows access denial and sharing violations, including an open file that
  Windows does not permit deleting, shall return -1 with `errno` set to
  `EACCES`; other Windows deletion failures shall also report `EACCES` because
  the shared ISO file primitive does not select POSIX-only error names.
- The declaration shall remain repeatable in C89 and C99 modes and absent from
  the strict profile.
- The implementation shall use Windows 2000-compatible imports and shall not
  claim Unix deletion-of-open-files behavior.

## Rationale

`unlink` is required by the selected POSIX roadmap. Hardening the shared file
removal primitive gives ISO `remove` and this adapter deterministic Windows
error translation without duplicating filesystem logic.

## Verification

**Method:** Automated presence, absence, behavior, error, and import testing

**References:** TC-0061

Tests cover exact declarations, repeatable C89/C99 selected and strict
headers, file creation/removal, missing paths, null paths, and CI import gates.

## Relationships

- **Depends on:** REQ-0003, REQ-0012, REQ-0050, and REQ-0060
- **Conflicts with:** Claiming Unix open-file unlink semantics on Windows

## Tailoring

Only regular filesystem-name removal under Windows semantics is included.

## Implementation Record

`include/unistd.h` declares the selected adapter;
`src/platform/windows/file.c` supplies validation and error translation; and
`src/platform/windows/posix_files.c` supplies the POSIX spelling. TC-0061
passes on x64 TinyCC; x86 and ARM64, including imports, are allocated to CI.
