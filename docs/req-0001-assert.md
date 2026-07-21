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
