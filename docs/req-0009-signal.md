# REQ-0009 — `<signal.h>`

**Content type:** Project requirement

**Status:** Implemented

**Source:** §4.7 Signals

## Scope

### Required files

- `include/signal.h` — public types, macros, and declarations.
- `src/signal.c` — handler registration and signal generation.
- `src/platform/windows/signal.c` — Windows exception/control-event mapping.
- `tests/c89/signal.c` — registration, delivery, reset, and error tests.

### Public surface

| Draft clause | Functions |
| --- | --- |
| §4.7.1.1 | `signal` |
| §4.7.2.1 | `raise` |

The header shall define `sig_atomic_t`, `SIG_DFL`, `SIG_ERR`, `SIG_IGN`, and
`SIGABRT`, `SIGFPE`, `SIGILL`, `SIGINT`, `SIGSEGV`, and `SIGTERM`.

## Requirement

- `signal` shall install permitted default, ignore, or user-handler actions and
  return the previous action or `SIG_ERR` on failure.
- `raise` shall deliver the requested supported signal and report success or
  failure using the specified return convention.
- Handler invocation, reset policy, re-raising behavior, and access to
  `volatile sig_atomic_t` shall follow §4.7.
- WCRT shall document how required C signals map to Windows facilities, which
  events can arrive asynchronously, and which operations are safe in handlers.
- Unsupported signal numbers shall fail without corrupting handler state.

## Rationale

WCRT must provide its own `<signal.h>` contract to supply C89-conforming behavior without depending on a host C runtime.

## Verification

**Method:** Automated test and inspection

**References:** `TC-0009`

Tests shall cover every required signal where safely inducible, handler return,
ignore/default behavior, previous-handler returns, `raise`, and invalid signal
numbers. Destructive default actions shall run in child processes. Shared gates
in `REQUIREMENTS.md` apply.

## Relationships

- **Derived from:** §4.7 Signals
- **Depends on:** Shared build, ABI, and quality gates in `docs/REQUIREMENTS.md`
- **Conflicts with:** None known

## Tailoring

WCRT groups the related obligations for this C89 conformance unit under one
stable requirement identifier. Each observable obligation remains explicit in
this record and is verified collectively by `TC-0009`.

## Implementation Record

WCRT assigns the Microsoft-compatible values 2, 4, 8, 11, 15, and 22 to
`SIGINT`, `SIGILL`, `SIGFPE`, `SIGSEGV`, `SIGTERM`, and `SIGABRT`. A call to
`raise` delivers the signal synchronously in the calling thread. Before a user
handler is invoked, its disposition is reset to `SIG_DFL`; the handler may
install another disposition explicitly.

The initial C89 implementation does not translate Windows console control
events or structured exceptions into C signals, so no signal arrives
asynchronously. Only assignment to an object declared `volatile sig_atomic_t`
is promised to be handler-safe. A default action terminates the process with
`ExitProcess(128 + signal_number)`, an API present on Windows 2000 and later.
