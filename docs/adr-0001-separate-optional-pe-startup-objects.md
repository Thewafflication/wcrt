# ADR-0001: Separate Optional PE Startup Objects

**Content type:** Architecture decision record

**Status:** Proposed

**Date:** 2026-07-26

## Context

WCRT currently distributes `libwcrt.a` as a static C library. TinyCC supplies
the PE entry point and startup path during an ordinary application link. A
consumer may also need a fully WCRT-owned executable that does not link the
toolchain C runtime startup.

PE startup depends on the Windows subsystem and application interface. Console
applications enter the C program through `main`, while C89 GUI applications use
the Windows `WinMain` extension. A PE image has one entry point, and placing a
startup implementation in the general runtime archive could conflict with the
toolchain startup or depend on fragile archive extraction behavior.

The controlled behavior is specified by
[REQ-0017](req-0017-optional-console-startup.md) and
[REQ-0018](req-0018-optional-gui-startup.md).

## Decision Drivers

- Preserve compatibility with ordinary TinyCC static-library consumers.
- Permit executables that do not depend on a host C runtime startup.
- Make ownership of the single PE entry point explicit at link time.
- Support both console and GUI Windows subsystem contracts.
- Keep subsystem-specific and toolchain-sensitive startup outside the standard
  C library archive.
- Make startup behavior independently packageable, documentable, and testable
  on x86, x64, and ARM64.

## Considered Options

1. Provide separate, explicitly selected console and GUI startup objects.
2. Put one default startup implementation in `libwcrt.a`.
3. Continue relying exclusively on TinyCC startup code.
4. Put console and GUI dispatch in one startup object selected by runtime logic.

## Decision

WCRT provides subsystem-specific startup as optional object files separate from
`libwcrt.a`:

- `wcrt-startup-console.o` owns the entry point for console applications and
  invokes `main`.
- `wcrt-startup-gui.o` owns the entry point for GUI applications and invokes
  ANSI `WinMain` for the C89 milestone.

A consumer selects exactly one startup object through a documented explicit
link action that omits the toolchain C runtime startup. Linking only
`libwcrt.a` continues to use the toolchain-provided entry point.

The two objects may share internal source or helpers, but their public
artifacts, subsystem contracts, link recipes, and verification remain
distinct. DLL startup, C++ initialization, and other compiler runtime
extensions are outside this decision.

WCRT deliberately defers `wWinMain` until the C99 milestone defines and verifies
the public `wchar_t` ABI and Windows UTF-16 interoperability on every supported
architecture. The C89 GUI startup shall not expose `wWinMain` prematurely or
silently alias it to `WinMain`. A Unicode GUI startup shall use a distinct
artifact and requirement so consumers select its ABI explicitly. If that work
changes the startup boundary established here, it shall supersede this ADR.

## Rationale

Separate objects make the subsystem choice visible and prevent duplicate entry
points. They also avoid relying on the linker to extract an entry-point member
from a general archive. A single combined object would still require a
link-time subsystem decision while coupling unrelated application interfaces.
Exclusive reliance on TinyCC would prevent WCRT-owned static startup, while an
implicit default in `libwcrt.a` would weaken interoperability.

## Consequences

### Positive

- Ordinary static consumers retain their current link behavior.
- Freestanding-from-toolchain-startup console and GUI applications are both
  supported.
- PE subsystem, entry symbol, argument contract, and imports can be verified
  independently.
- Future startup variants can be added without changing `libwcrt.a`.

### Negative

- Packages contain architecture-specific startup artifacts and documentation.
- Consumers using WCRT startup need an explicit, more advanced link command.
- Console argument parsing and GUI command-line/show-state behavior require
  separate implementation and compatibility testing.
- ANSI `WinMain` does not provide a Unicode-native GUI startup interface.

### Follow-up

- Implement REQ-0017 and TC-0017 for console startup.
- Implement REQ-0018 and TC-0018 for GUI startup.
- Add both objects to static distributions and WPM package verification.
- Document exact TinyCC commands, entry symbols, imports, and supported
  argument parsing for each architecture.
- Define and verify the `wchar_t` ABI and Windows UTF-16 interoperability in
  the C99 milestone before specifying `wWinMain`.
- Add `wWinMain` as a separately named, controlled startup artifact and
  requirement after those prerequisites pass.

## References

- [REQ-0017 — Optional console process startup](req-0017-optional-console-startup.md)
- [REQ-0018 — Optional GUI process startup](req-0018-optional-gui-startup.md)
- [WCRT roadmap](../ROADMAP.md)
- [WSP ADR guidance](../tools/wsp/architecture/adr-guidance.md)
