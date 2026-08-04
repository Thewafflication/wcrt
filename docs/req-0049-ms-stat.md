# REQ-0049 — Microsoft 64-bit file status and x86 alias

**Content type:** Project requirement

**Status:** Proposed

**Source:** Microsoft Learn documentation for the
[`_stat` family](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions?view=msvc-170);
WPM TinyCC compatibility inventory

**Compatibility annotation:** Microsoft CRT source and ABI compatibility

## Scope

This requirement provides `struct _stat64`, `_stat64`, and the linker-required
x86 `_stat` alias used by WPM's TinyCC build. Other `_stat*`, `_fstat*`, and
wide-path variants are excluded.

## Requirement

- WCRT shall declare `struct _stat64` and `_stat64` in `<sys/stat.h>` with
  Microsoft-compatible member types, ordering, packing, calling convention,
  constants, and architecture-specific symbol decoration.
- `_stat64` shall populate the documented device, inode, mode, link, user,
  group, size, access-time, modification-time, and creation-time fields for a
  representable path and return zero.
- File type and permission bits shall describe regular files and directories
  consistently with Microsoft `_S_IF*` and `_S_I*` constants.
- File size and all three timestamps shall use the `_stat64` 64-bit forms.
- On failure, `_stat64` shall return `-1` and set `errno` to the documented
  error category without exposing partially initialized data as valid.
- On x86, WCRT shall export a linker-compatible `_stat` symbol that accepts
  the layout selected by the TinyCC public header and produces `_stat64`
  behavior. Other architectures shall not acquire this alias unless their ABI
  requires it and a separate compatibility decision records that requirement.
- The implementation shall pass layout, symbol-table, and Windows 2000 import
  inspection.

## Rationale

TinyCC's 32-bit PE linker canonicalizes the `_stat64` reference used by WPM to
`_stat`; WPM currently supplies both the operation and the compatibility alias.

## Verification

**Method:** Automated ABI, behavioral, symbol, and import-table test

**References:** Planned `TC-0049`

Tests shall cover structure offsets and size per architecture, regular files,
directories, empty and large files, read-only state, timestamp conversion,
missing and invalid paths, x86 symbol resolution, absence of unintended aliases
on other architectures, and imports.

## Relationships

- **Derived from:** WPM TinyCC compatibility inventory
- **Depends on:** REQ-0003 error reporting and the Microsoft filesystem profile
- **Conflicts with:** Treating `_stat` as a source-level synonym on every ABI

## Tailoring

The `_stat` alias is required only for the verified x86 TinyCC ABI condition.

## Implementation Record

Not yet implemented in WCRT. WPM currently provides an 8-byte-packed layout,
Windows file-attribute translation, and the x86 alias in
`wpm/tcc_compat/wcrt_stat.c`.
