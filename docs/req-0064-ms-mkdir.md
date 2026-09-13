# REQ-0064 — Microsoft directory and absolute-path adapters

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft CRT `_mkdir` compatibility contract

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement provides narrow `_mkdir` and `_getcwd` in `<direct.h>` and
`_fullpath` in `<stdlib.h>`. Directory removal, drive selection, and wide-path
functions are excluded.

## Requirement

- WCRT shall declare `_mkdir(const char *)` in `<direct.h>`.
- WCRT shall declare `_getcwd(char *, int)` in `<direct.h>` and
  `_fullpath(char *, const char *, size_t)` in `<stdlib.h>`.
- `_mkdir` shall create one directory and return zero on success.
- An existing target shall return -1 with `EEXIST`; a missing parent shall
  return -1 with `ENOENT`; and access denial shall report `EACCES`.
- WCRT shall expose `EEXIST` with the Microsoft-compatible value 17.
- A null path shall return -1 with `errno` set to `EINVAL`.
- Current-directory and absolute-path queries shall write a null-terminated
  result to a sufficiently large caller buffer. A null destination shall
  allocate a result that the caller can release with `free`.
- Insufficient caller buffers shall fail with `ERANGE`; null source paths and
  invalid sizes shall fail with `EINVAL`; allocation failure shall report
  `ENOMEM`.
- The implementation shall use Windows 2000-compatible imports.

## Rationale

`_mkdir` begins the required VC-era `<direct.h>` directory family with a
bounded operation whose Windows behavior maps directly to the Microsoft API.

## Verification

**Method:** Automated compile-time, behavioral, error, and import test

**References:** TC-0064

Tests cover exact declarations, creation errors, current-directory queries,
absolute resolution, caller and allocated buffers, range errors, and the
existing x86 import gate.

## Relationships

- **Depends on:** REQ-0003
- **Conflicts with:** Recursive creation or POSIX mode-bit behavior

## Tailoring

Only narrow one-directory creation is included.

## Implementation Record

`include/direct.h` and `include/stdlib.h` declare the adapters;
`src/platform/windows/directory.c` implements them; and TC-0064 passes on x64
TinyCC. x86 and ARM64, including the x86 import audit, are allocated to CI.
