# REQ-0009 — `<signal.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.7 Signals

## Required files

- `include/signal.h` — public types, macros, and declarations.
- `src/signal.c` — handler registration and signal generation.
- `src/platform/windows/signal.c` — Windows exception/control-event mapping.
- `tests/c89/signal.c` — registration, delivery, reset, and error tests.

## Public surface

| Draft clause | Functions |
| --- | --- |
| §4.7.1.1 | `signal` |
| §4.7.2.1 | `raise` |

The header shall define `sig_atomic_t`, `SIG_DFL`, `SIG_ERR`, `SIG_IGN`, and
`SIGABRT`, `SIGFPE`, `SIGILL`, `SIGINT`, `SIGSEGV`, and `SIGTERM`.

## Requirements

- `signal` shall install permitted default, ignore, or user-handler actions and
  return the previous action or `SIG_ERR` on failure.
- `raise` shall deliver the requested supported signal and report success or
  failure using the specified return convention.
- Handler invocation, reset policy, re-raising behavior, and access to
  `volatile sig_atomic_t` shall follow §4.7.
- WCRT shall document how required C signals map to Windows facilities, which
  events can arrive asynchronously, and which operations are safe in handlers.
- Unsupported signal numbers shall fail without corrupting handler state.

## Acceptance

Tests shall cover every required signal where safely inducible, handler return,
ignore/default behavior, previous-handler returns, `raise`, and invalid signal
numbers. Destructive default actions shall run in child processes. Shared gates
in `REQUIREMENTS.md` apply.
