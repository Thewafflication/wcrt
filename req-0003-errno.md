# REQ-0003 — `<errno.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.1.3 Errors

## Required files

- `include/errno.h` — public error macros and `errno` lvalue contract.
- `src/errno.c` — runtime storage/accessor.
- `tests/c89/errno.c` — lvalue, value, and library-interaction tests.

## Public surface

- `errno` — modifiable `int` lvalue.
- `EDOM` — positive integer constant expression for domain errors.
- `ERANGE` — positive integer constant expression for range errors.

No public function is introduced by this header.

## Requirements

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

## Acceptance

Tests shall assign both required error values, provoke representative math and
conversion errors, and verify that success is not tested by assuming `errno`
became zero. The shared gates in `REQUIREMENTS.md` apply.
