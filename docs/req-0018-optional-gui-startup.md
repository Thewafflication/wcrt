# REQ-0018 — Optional GUI process startup

**Content type:** Project requirement

**Status:** Proposed

**Source:** ADR-0001 and project static-link integration requirement; not
specified by C89

## Scope

This requirement applies to statically linked Windows GUI applications on the
supported x86, x64, and ARM64 targets. It covers the ANSI `WinMain` application
interface for the C89 milestone. DLL startup, console startup, `wWinMain`, and
C++ static constructors are excluded.

`wWinMain` is deliberately deferred until the C99 milestone defines and
verifies WCRT's `wchar_t` ABI and Windows UTF-16 interoperability. The ANSI
startup object shall not export, alias, or claim support for `wWinMain`.

### Required files

- `wcrt-startup-gui.o` — separately selectable GUI startup object.
- `tests/c89/run-tc-0018.ps1` — TC-0018 test runner.

## Requirement

- Each WCRT static distribution shall provide `wcrt-startup-gui.o` separately
  from `libwcrt.a` and `wcrt-startup-console.o`.
- The startup object shall define the PE entry point needed to start a Windows
  GUI-subsystem application without a toolchain-provided C runtime startup.
- The startup object shall call `WinMain` with the current module instance, a
  null previous-instance value, the process command line excluding the program
  name, and the applicable Windows initial-show value.
- The startup object shall pass the value returned by `WinMain` to `exit`.
- The GUI startup object shall be selected only by an explicit consumer link
  action and shall not be selected by an ordinary link against `libwcrt.a`.
- The startup path and the WCRT objects it selects shall not depend on a host C
  runtime library.
- WCRT shall document the supported TinyCC GUI link command, entry symbol,
  command-line parsing rules, show-state mapping, and unsupported startup
  facilities for each supported architecture.

## Rationale

A separate GUI startup object supports native Windows applications without
forcing a GUI entry contract or subsystem on console and ordinary static
library consumers.

## Verification

**Method:** Automated integration test and binary inspection

**References:** `TC-0018`

TC-0018 shall link and inspect a GUI-subsystem application on every supported
architecture, reject host-CRT dependencies, and verify the `WinMain` arguments,
normal termination, and `atexit` processing through a controlled child test.

## Relationships

- **Derived from:** ADR-0001
- **Depends on:** REQ-0013 process termination and REQ-0017 separation of
  general runtime from optional startup
- **Conflicts with:** Premature `wWinMain` exposure before the C99 wide-character
  ABI and UTF-16 prerequisites are verified

## Tailoring

Native execution is required on supported release architectures. A controlled
cross-link and PE inspection may be used as interim ARM64 evidence only while
native ARM64 execution is unavailable; it does not satisfy the final release
gate.

## Implementation Record

No GUI startup object is implemented. `tests/c89/run-tc-0018.ps1` records the
planned test as Not run until the required artifact and executable test
procedure are implemented.
