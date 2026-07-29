# REQ-0044 — Microsoft stream file descriptor

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft Learn documentation for
[`_fileno`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fileno?view=msvc-170)

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement provides `_fileno`, which obtains the integer file descriptor
associated with an open WCRT stream. The deprecated POSIX alias `fileno` and
the remaining Microsoft low-level descriptor family are excluded.

### Required files

- `include/stdio.h` — public `_fileno` declaration.
- `src/stdio.c` — stream-to-descriptor implementation.
- `tests/mscompat/presence/fileno.c` — declaration-presence verification.
- `tests/mscompat/fileno.c` — behavioral and boundary verification.

## Requirement

- WCRT's Microsoft compatibility surface shall declare `_fileno` in
  `<stdio.h>` as `int _fileno(FILE *stream);`.
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
- WCRT shall not expose the deprecated non-underscored `fileno` alias under
  this requirement.

## Rationale

Windows-oriented code uses `_fileno` to bridge buffered `FILE` streams and
integer-descriptor APIs. The WSP C logger also needs this mapping for automatic
terminal detection when built against WCRT.

## Verification

**Method:** Automated compile-time, behavioral, and integration test

**References:** `TC-0044`

TC-0044 shall verify the declaration in `<stdio.h>`, standard descriptor
values, descriptors for `fopen` streams, lack of stream-state changes, the
unattached-output sentinel where constructible, null continuation behavior,
absence of the `fileno` alias, and compilation of the WSP C logger.

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

`include/stdio.h` declares `_fileno`; WCRT streams retain stable descriptors
through their open lifetime and successful `freopen` operations. Standard
streams use descriptors 0 through 2, dynamic stream slots use descriptors 3
and above, and null input follows the documented `EINVAL` continuation path.
TC-0044 verifies the interface, behavior, state preservation, and WSP logger
integration with terminal detection disabled.
