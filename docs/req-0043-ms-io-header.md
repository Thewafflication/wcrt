# REQ-0043 — Microsoft low-level I/O header baseline

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft Learn documentation for
[required and optional CRT headers](https://learn.microsoft.com/en-us/cpp/c-runtime-library/required-and-optional-header-files?view=msvc-170)
and the [`_read`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/read?view=msvc-170)
low-level I/O interface

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement establishes a minimal, self-contained `<io.h>` so programs
that select WCRT's Microsoft compatibility surface can include the documented
low-level I/O header. It does not prematurely expose the complete descriptor
API planned for roadmap milestone MS2.

### Required files

- `include/io.h` — public Microsoft-compatibility header.
- `tests/mscompat/presence/io.c` — header presence and isolation verification.

## Requirement

- WCRT shall provide `<io.h>` when its Microsoft compatibility surface is
  selected.
- `<io.h>` shall be safe for repeated inclusion and self-contained when it is
  the first project header included by a C or C++ translation unit.
- The initial header shall expose only interfaces assigned to implemented,
  controlled Microsoft-compatibility requirements; it shall not advertise the
  unimplemented MS2 descriptor family.
- `_fileno` shall remain declared by `<stdio.h>`, as Microsoft documents, and
  shall not require inclusion of `<io.h>`.
- Including `<io.h>` shall not alter any ISO C declaration or behavior.

## Rationale

Microsoft assigns low-level descriptor interfaces such as `_read` to
`<io.h>`. Providing the header as an independently controlled baseline removes
an immediate source-compatibility failure while allowing its API to grow only
as the corresponding functions and tests are implemented.

## Verification

**Method:** Automated compile-time and header-inspection test

**References:** `TC-0043`

TC-0043 shall verify header presence, self-containment, repeated inclusion,
C and C++ parsing, ISO-header isolation, and absence of declarations for
unimplemented MS2 interfaces.

## Relationships

- **Derived from:** Phase 0 Priority 0 and Microsoft Learn header assignments
- **Depends on:** Microsoft compatibility-mode selection policy
- **Conflicts with:** Declaring the complete MS2 descriptor API before its
  implementations and behavioral tests exist

## Tailoring

This requirement covers header availability and controlled growth only.
Individual low-level I/O routines require separate behavioral requirements.

## Implementation Record

`include/io.h` provides the guarded, self-contained compatibility header
without advertising the deferred MS2 descriptor family. TC-0043 verifies
repeatable inclusion, isolation, and the controlled initial surface.
