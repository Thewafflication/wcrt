# REQ-0067 — Shared descriptor input and output

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft low-level I/O and POSIX.1-2017 descriptor interfaces

**Compatibility annotation:** Shared WCRT descriptor state with distinct
Microsoft and selected POSIX spellings

## Scope

This requirement provides file-backed descriptor creation, closure, byte I/O,
positioning, flushing, terminal detection, stream association, and descriptor
duplication. Pipes, mode changes, and inheritance controls are excluded;
descriptor status is specified by REQ-0049 and REQ-0051.

## Requirement

- `<fcntl.h>` shall define the selected `_O_*` open flags and declare `_open`.
  Under `WCRT_POSIX` it shall expose the corresponding `O_*` names and `open`.
- `<io.h>` shall declare `_close`, `_read`, `_write`, `_lseek`, `_tell`,
  `_commit`, `_isatty`, `_fdopen`, `_dup`, and `_dup2` with
  Microsoft-compatible signatures.
- Selected `<unistd.h>` and `<stdio.h>` shall declare `close`, `read`, `write`,
  `lseek`, `fsync`, `isatty`, `fdopen`, `dup`, and `dup2` with POSIX-facing
  types.
- Descriptor opens shall allocate from the same slots used by WCRT streams.
  Descriptor and stream operations shall therefore observe one native handle
  and one file position.
- Creation, truncation, exclusive creation, append, binary, and text flags
  shall map to their Windows file behaviors. POSIX creation mode bits are
  accepted and ignored on Windows.
- Closed and invalid descriptors shall fail with `EBADF`. Null nonempty I/O
  buffers shall fail with `EINVAL`; broken pipes and full devices shall map to
  `EPIPE` and `ENOSPC`.
- `fdopen` shall return the existing stream object associated with a WCRT
  descriptor; closing either view shall invalidate the shared descriptor.
- Duplicated descriptors shall own independent handles with a shared file
  offset. Closing either descriptor shall leave the other usable. `_dup2`
  shall return zero while POSIX `dup2` shall return its target descriptor.
  Duplicating onto an occupied target shall replace it only after native
  duplication succeeds.
- The implementation shall add only Windows 2000-compatible imports.

## Verification

**Method:** Automated presence, isolation, state, data, error, and build tests

**References:** TC-0067

Tests compile Microsoft, selected POSIX, and strict fixtures in C89 and C99;
exercise exclusive creation, read/write, shared seeks, flushing, handle lookup,
terminal detection, `fdopen`, shared duplicate offsets, independent close
lifetimes, replacement, and errors; and retain the
multi-architecture import and consumer gates.

## Relationships

- **Depends on:** REQ-0012, REQ-0043, REQ-0044, REQ-0050, and REQ-0066
- **Conflicts with:** A descriptor table separate from WCRT stream ownership

## Tailoring

The current descriptor capacity remains `FOPEN_MAX`. Concurrency and pipe
lifetime require a later shared-state extension.

## Implementation Record

`src/stdio.c` owns descriptor allocation and adapters while the Windows file
backend performs flag opens, handle duplication, flushing, terminal detection,
error translation, and native I/O. TC-0067 passes locally on x64; all
architectures remain CI gates.
