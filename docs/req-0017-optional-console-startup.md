# REQ-0017 — Optional console process startup

**Content type:** Project requirement

**Status:** Implemented

**Source:** ADR-0001 and project bootstrap and static-link integration
requirement; not specified by C89

## Scope

This requirement applies to statically linked Windows console applications on
the supported x86, x64, and ARM64 targets. It covers an explicit WCRT-owned PE
startup path for applications linked without the TinyCC runtime startup. It
does not apply to DLL startup, GUI-subsystem startup, or compiler extensions
such as C++ static constructors.

### Required files

- `wcrt-startup-console.o` — separately selectable console startup object.
- `tests/c89/run-tc-0017.ps1` — TC-0017 test runner.

## Requirement

- Each WCRT static distribution shall provide `wcrt-startup-console.o`
  separately from `libwcrt.a`.
- The startup object shall define the PE entry point needed to start a console
  application without a toolchain-provided C runtime startup. For TinyCC, the
  object shall expose that entry point as `_start`.
- The startup object shall construct a C89-compatible `argc` and `argv`, call
  `main(argc, argv)`, and pass the value returned by `main` to `exit`.
- `argc` shall be nonnegative, `argv[argc]` shall be a null pointer, and each
  element before `argv[argc]` shall point to a null-terminated string.
- The startup object shall be selected only by an explicit consumer link
  action. An ordinary link against `libwcrt.a` shall neither define nor select
  a WCRT PE entry point and shall remain compatible with the TinyCC-provided
  startup path.
- The startup path and the WCRT objects it selects shall not depend on a host C
  runtime library.
- WCRT shall document the supported TinyCC link command, entry symbol, console
  subsystem selection, argument parsing rules, and unsupported startup
  facilities for each supported architecture.

## Rationale

An optional startup object permits a fully WCRT-owned static executable while
keeping the standard library independent of compiler- and subsystem-specific
process startup. Separate, explicit selection prevents duplicate PE entry
points and avoids relying on archive-member extraction for startup code.

## Verification

**Method:** Automated integration test and binary inspection

**References:** `TC-0017`

TC-0017 shall verify the ordinary TinyCC startup path and the explicit WCRT
startup path on every supported architecture. It shall inspect the explicit
artifact for the expected PE machine type and host-CRT independence, execute
supported native targets with controlled arguments, and verify `main` return
and `atexit` processing through `exit`.

## Relationships

- **Derived from:** ADR-0001 and Phase 0 process-startup and
  deterministic-static-link objectives in `ROADMAP.md`
- **Depends on:** REQ-0013 process termination; supported TinyCC and Windows
  ABI definitions
- **Conflicts with:** None; explicit selection avoids the toolchain startup
  path when WCRT owns the entry point

## Tailoring

Native execution is required on supported release architectures. A controlled
cross-link and PE inspection may be used as interim ARM64 evidence only while
native ARM64 execution is unavailable; it does not satisfy the final release
gate.

## Implementation Record

- `src/platform/windows/startup_console.c` defines TinyCC's `_start` entry,
  parses the Windows command line into `argc` and `argv`, calls `main`, and
  terminates through `exit`.
- `tools/build-wcrt.ps1` produces `wcrt-startup-console.o` separately from
  `libwcrt.a` for x86, x64, and ARM64.
- `tests/c89/run-tc-0017.ps1` verifies ordinary startup compatibility, explicit
  startup linking, PE properties, host-CRT independence, arguments, return
  propagation, `atexit`, and native redirected standard input with exact
  negative and affirmative confirmation records.
- Native x86 and x64 execution and ARM64 cross-link inspection pass locally;
  final ARM64 native evidence remains assigned to the ARM64 CI matrix entry.
