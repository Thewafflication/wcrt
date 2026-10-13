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

`include/io.h` provides the guarded, self-contained compatibility header and
grows only with controlled descriptor requirements. TC-0043 verifies
repeatable inclusion, isolation, and rejection of interfaces that remain
deferred after REQ-0067.

REQ-0082/0083 add controlled _setmode and _sopen declarations. TC-0043 now
checks the explicit approved declaration inventory in both directions rather
than retaining an obsolete _setmode absence assertion. Behavioral verification
remains with each owning requirement. This changes the header audit only;
runtime, ISO isolation and existing API contracts are unchanged.

REQ-0088 adds _lseeki64/_telli64/_filelength/_filelengthi64 to the controlled
inventory after implementation and behavioral fixtures. TC-0043 checks their
presence; TC-0088 owns size, position, narrowing and error semantics.
REQ-0089 adds _eof with physical-query behavior verified by TC-0089.
REQ-0090 adds _open_osfhandle; TC-0090 owns its native ownership/access tests.
REQ-0094 adds _chsize and REQ-0096 adds the three versioned search APIs.
TC-0043 audits both underscore and WCRT-prefixed declarations; owning tests
verify resize and enumeration behavior. Existing descriptor ABI is unchanged.
REQ-0098 adds _locking; TC-0098 owns mode/range/retry and state tests.
REQ-0099 adds _creat; TC-0099 owns creation/truncation and forwarding tests.
REQ-0100 adds both versioned narrow-time/wide-size search declarations;
TC-0100 verifies their record layout, boundaries, ownership and retention.
REQ-0101 adds _umask; TC-0101 owns mask values and new-file permission effects.
