# REQ-0044 — Microsoft stream file descriptor

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft Learn documentation for
[`_fileno`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fileno?view=msvc-170)

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement provides `_fileno`, the selected POSIX alias `fileno`, and
`_get_osfhandle`, which bridge an open WCRT stream, its integer descriptor, and
the underlying native Windows handle. The remaining low-level descriptor
family is excluded.

### Required files

- `include/stdio.h` — public `_fileno` declaration.
- `include/io.h` — public `_get_osfhandle` declaration.
- `src/stdio.c` — stream-to-descriptor implementation.
- `tests/mscompat/presence/fileno.c` — declaration-presence verification.
- `tests/mscompat/fileno.c` — behavioral and boundary verification.

## Requirement

- WCRT's Microsoft compatibility surface shall declare `_fileno` in
  `<stdio.h>` as `int _fileno(FILE *stream);`.
- When `WCRT_POSIX` is selected, `<stdio.h>` shall declare `fileno` with the
  same signature and behavior. Strict inclusion shall not expose the alias.
- `<io.h>` shall declare `_get_osfhandle` with a pointer-width signed return
  type and shall return the native handle for a currently open WCRT descriptor.
- `_get_osfhandle` shall return -1 and set `errno` to `EBADF` for an invalid or
  closed descriptor.
- `_fileno` shall return the descriptor currently associated with a valid,
  open stream without changing its buffering, position, error indicators,
  mode, or ownership.
- `_fileno(stdin)`, `_fileno(stdout)`, and `_fileno(stderr)` shall return `0`,
  `1`, and `2`, respectively, when those streams are attached.
- When `stdout` or `stderr` is not attached to an output stream, `_fileno`
  shall return `-2`, matching the documented current Microsoft distinction
  between an unattached standard stream and an invalid parameter.
- Calling `_fileno(NULL)` shall invoke WCRT's invalid-parameter policy. Until
  that policy is implemented, the defined continuation behavior shall return
  `-1` and set `errno` to `EINVAL`.
- The result is undefined when a nonnull argument does not identify a valid,
  open WCRT stream, matching the Microsoft contract.

## Rationale

Windows-oriented code uses `_fileno` to bridge buffered `FILE` streams and
integer-descriptor APIs. The WSP C logger also needs this mapping for automatic
terminal detection when built against WCRT.

## Verification

**Method:** Automated compile-time, behavioral, and integration test

**References:** `TC-0044`

TC-0044 shall verify the declarations, selected alias isolation, standard
descriptor values, descriptors for `fopen` streams, lack of stream-state
changes, the
unattached-output sentinel where constructible, null continuation behavior,
native-handle lookup and invalidation, and compilation of the WSP C logger.

## Relationships

- **Derived from:** Phase 0 Priority 0 and Microsoft Learn `_fileno`
- **Depends on:** REQ-0003 error reporting, REQ-0012 streams, and WCRT's
  descriptor-allocation model
- **Conflicts with:** Declaring `_fileno` only in `<io.h>`, returning `-1` for
  an unattached output stream, or exposing `fileno` as an ISO C interface

## Tailoring

This requirement implements the current Microsoft spelling and documented
sentinels. It does not require compatibility with private UCRT stream layouts
or global-state partitioning.

## Implementation Record

`include/stdio.h` declares `_fileno` and selected `fileno`; `<io.h>` declares
`_get_osfhandle`; WCRT streams retain stable descriptors
through their open lifetime and successful `freopen` operations. Standard
streams use descriptors 0 through 2, dynamic stream slots use descriptors 3
and above, and null input follows the documented `EINVAL` continuation path.
TC-0044 verifies the interfaces, behavior, state preservation, handle bridge,
strict isolation, and WSP logger integration with terminal detection disabled.
