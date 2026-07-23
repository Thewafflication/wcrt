# REQ-0001 — `<assert.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.2 Diagnostics; §4.2.1.1 `assert`

## Required files

- `include/assert.h` — public macro.
- `src/assert.c` — assertion reporting and abnormal termination helper.
- `tests/c89/assert.c` — enabled, disabled, side-effect, and diagnostic tests.

## Public surface

- `assert(expression)` shall be a macro.
- `NDEBUG` controls whether `assert` is active; it is supplied by the program,
  not defined unconditionally by WCRT.

## Requirements

- With `NDEBUG` undefined, `assert` shall evaluate its expression exactly once.
- A nonzero expression shall have no further effect.
- A zero expression shall emit a diagnostic containing the expression text,
  source filename, and source line, then call `abort`.
- With `NDEBUG` defined, `assert` shall expand to a void expression and shall
  not evaluate its argument.
- Re-including the header after changing `NDEBUG` shall select the new behavior;
  an include guard shall not freeze the macro definition.
- The implementation shall use WCRT `stderr` and `abort`, not a host CRT.

## Acceptance

The shared gates in `REQUIREMENTS.md` apply. Tests shall additionally validate both
`NDEBUG` states in separate translation units and capture the failure exit and
diagnostic fields.

## Implementation record

- `include/assert.h` implements the repeatable C89 macro definition.
- `src/assert.c` reports through `stderr`, flushes it, and calls `abort`.
- `tests/c89/presence/assert.c` verifies the standalone public surface.
- `tests/c89/assert.c` and `tests/c89/assert_disabled.c` verify enabled,
  disabled, repeated-inclusion, diagnostic, and termination behavior.
- `tests/c89/run-tc-0001.ps1` runs TC-0001 with TinyCC, beginning with the
  standalone presence check and then executing the behavioral checks.

The assertion implementation is complete. Its current behavioral harness uses
TinyCC's runtime to supply `stderr`, formatted output, flushing, and `abort`.
The final no-host-CRT integration gate will be re-run when REQ-0012 and
REQ-0013 supply those dependencies from WCRT.
