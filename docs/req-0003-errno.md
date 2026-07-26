# REQ-0003 — `<errno.h>`

**Content type:** Project requirement

**Status:** Implemented

**Source:** §4.1.3 Errors

## Scope

### Required files

- `include/errno.h` — public error macros and `errno` lvalue contract.
- `src/errno.c` — runtime storage/accessor.
- `tests/c89/errno.c` — lvalue, value, and library-interaction tests.

### Public surface

- `errno` — modifiable `int` lvalue.
- `EDOM` — positive integer constant expression for domain errors.
- `ERANGE` — positive integer constant expression for range errors.

No public function is introduced by this header.

## Requirement

- `EDOM` and `ERANGE` shall be distinct.
- A program shall be able to read, assign, and take behaviorally correct account
  of `errno` through the public macro/lvalue interface.
- Storage shall not alias unrelated program objects.
- WCRT shall document whether the C89-only build uses process-global or
  per-thread storage. Later thread support must not change the public source
  interface.
- A library function shall set `errno` only where its clause requires or permits
  it. No function shall promise to clear `errno` on success.
- The implementation shall not import host CRT `errno` storage.

## Rationale

WCRT must provide its own `<errno.h>` contract to supply C89-conforming behavior without depending on a host C runtime.

## Verification

**Method:** Automated test and inspection

**References:** `TC-0003`

Tests shall assign both required error values, provoke representative math and
conversion errors, and verify that success is not tested by assuming `errno`
became zero. The shared gates in `REQUIREMENTS.md` apply.

## Relationships

- **Derived from:** §4.1.3 Errors
- **Depends on:** Shared build, ABI, and quality gates in `docs/REQUIREMENTS.md`
- **Conflicts with:** None known

## Tailoring

WCRT groups the related obligations for this C89 conformance unit under one
stable requirement identifier. Each observable obligation remains explicit in
this record and is verified collectively by `TC-0003`.

## Implementation Record

- `include/errno.h` defines distinct positive `EDOM` and `ERANGE` constants and
  exposes `errno` as a modifiable lvalue.
- `src/errno.c` supplies process-global storage without importing host CRT
  error storage. Later threading support may replace the backing expression
  without changing the public macro.
- `tests/c89/errno.c` verifies assignment, reads, stable address, non-aliasing,
  and both required constants.
- Math and conversion producer checks remain integration gates for REQ-0007
  and REQ-0013; those functions do not exist before those requirements.
- `tests/c89/run-tc-0003.ps1` builds and executes TC-0003 with TinyCC.
