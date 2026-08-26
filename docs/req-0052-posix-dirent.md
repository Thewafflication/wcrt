# REQ-0052 — POSIX directory stream compatibility

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017 `opendir`, `readdir`, `rewinddir`, and `closedir`

**Compatibility annotation:** Selected POSIX source and behavioral
compatibility with documented Windows directory-entry deviations

## Scope

This requirement provides the `WCRT_POSIX`-selected `<dirent.h>` directory
stream family. Directory mutation, descriptor-backed streams, seeking by
location, and thread-safe entry retrieval are excluded.

## Requirement

- `<dirent.h>` shall expose its types, constants, and functions only when
  `WCRT_POSIX` is defined and shall not advertise `_POSIX_VERSION`.
- `DIR` shall be opaque. `struct dirent` shall provide `d_ino`, `d_type`, and
  a null-terminated `d_name` capable of holding a 255-byte Windows component.
- `opendir` shall open an existing directory or return null with `errno` set.
- `readdir` shall return successive entries using stream-owned storage. At end
  of stream it shall return null without changing `errno`.
- `rewinddir` shall reposition a valid stream to its first entry, and
  `closedir` shall release both the Windows search handle and WCRT storage.
- Windows directory and non-directory entries shall map to `DT_DIR` and
  `DT_REG`; unavailable inode values shall be zero.
- The implementation shall use Windows 2000-compatible find APIs and preserve
  the existing ISO and Microsoft ABI surfaces.

## Verification

**Method:** Automated header isolation, interface, filesystem, restart,
end-of-stream, and error testing

**References:** `TC-0052`

## Relationships

- **Derived from:** WCRT 1.1.0 bounded POSIX profile and ADR-0007
- **Depends on:** REQ-0003, REQ-0013, and REQ-0050
- **Conflicts with:** Unconditional exposure of POSIX directory names

## Tailoring

The fixed public name capacity and `d_type` values are portability extensions;
they do not constitute a complete POSIX conformance claim.

## Implementation Record

`include/dirent.h` defines the selected source surface, and
`src/platform/windows/dirent.c` adapts Windows find handles to directory
streams without depending on host CRT declarations.
