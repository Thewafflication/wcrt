# Waughtal C Run Time

Waughtal C Run Time (**WCRT**) is a free C runtime library for Windows. Its goal
is to provide a small, understandable, standards-oriented implementation that
can be compiled with TinyCC and used on Windows versions from Windows 2000
through Windows 11.

WCRT is developed incrementally. Each milestone builds on the previous C
standard:

- C89 / C90
- C99
- C11
- C17
- C23

The project is licensed under the GNU General Public License, version 3 or (at
your option) any later version.

## Project status

WCRT is in its initial design and bootstrap phase. It is not yet suitable for
production use. The first implementation milestone is a usable C89 runtime.

See [ROADMAP.md](ROADMAP.md) for planned work and milestone completion criteria.

## Goals

- Implement the hosted C library in clearly defined standard milestones.
- Compile the library and its tests with TinyCC (`tcc`).
- Support 32-bit Windows from Windows 2000 onward.
- Support 64-bit Windows where the operating system and compiler permit it.
- Avoid dependencies on Windows APIs introduced after the configured minimum
  target version.
- Keep platform-specific code isolated from standards-facing code.
- Test observable behavior, edge cases, error handling, and ABI assumptions.
- Document extensions, omissions, implementation-defined behavior, and known
  deviations from each supported C standard.

## Non-goals

- Binary compatibility with every Microsoft C runtime.
- Reproducing undocumented behavior of another runtime.
- Claiming complete conformance before the relevant test and documentation
  gates have passed.
- Providing features from a newer C milestone in an older compatibility mode
  when doing so would break that mode's required behavior.

## Compatibility policy

### C language editions

Headers expose declarations appropriate to the selected language edition when
the compiler provides a reliable edition indicator. WCRT may also provide an
explicit configuration override for compilers whose `__STDC_VERSION__` value
is absent or incomplete.

Later milestones must preserve programs supported by completed earlier
milestones unless the C standard requires different behavior. Any unavoidable
incompatibility will be documented.

### Windows

Windows 2000 is the minimum intended operating-system target for the 32-bit
build. Code used by that build must not acquire unconditional imports from
newer Windows releases. Newer facilities may be used only behind runtime
feature detection with a compatible fallback.

The phrase “Windows 2000–11” describes the supported operating-system family,
not one identical architecture matrix: Windows 2000 is a 32-bit target, while
64-bit builds apply only to Windows versions and toolchains that support them.
Windows ARM64 is a Windows 10/11 target; WCRT does not associate ARM64 with the
legacy Windows 2000 compatibility tier.

The current ARM64 support boundary and cross-build procedure are documented in
[`docs/arm64-support.md`](docs/arm64-support.md).

### TinyCC

TinyCC compatibility is a release gate, not a best-effort port. Public headers,
runtime sources, build scripts, and the conformance test harness must remain
usable with the supported TinyCC versions. Compiler limitations will be
recorded separately from runtime limitations.

## Planned repository layout

```text
include/          Public standard and WCRT headers
src/              Standards-facing runtime implementation
src/platform/     Windows and architecture-specific code
tests/            Unit, regression, integration, and conformance tests
tools/            Build and test utilities
docs/             Design notes and compatibility documentation
```

This layout will be introduced during the bootstrap milestone and may evolve
before the first release.

## Building

The build requires PowerShell and an architecture-matched TinyCC installation.
Debug builds also require `cv2pdb.exe` in `PATH`:

```powershell
./tools/build-wcrt.ps1 -Architecture x64 -Configuration Debug -TinyCc $env:TCC_HOME/tcc.exe
./tools/run-c89-tests.ps1 -Architecture x64 -TinyCc $env:TCC_HOME/tcc.exe
```

GitHub Actions builds and tests x86, x64, and ARM64 independently. TC-0016 is
always run first. Each job publishes the debug DLL, PDB, and a TeX test-results
table. A `vMAJOR.MINOR.PATCH` tag additionally gates a Release build and creates
an architecture-specific WPM package whose version is taken from the tag.

## Conformance

WCRT will publish a conformance matrix for every milestone. A milestone is not
complete merely because all named headers and functions exist; it must also
meet its behavioral tests, supported-platform tests, documentation gate, and
TinyCC build gate.

Where the C standard permits implementation choices, WCRT will document them
under `docs/implementation-defined.md`. Known deviations will be tracked openly
and will block an unqualified conformance claim.

## Contributing

Contributions are welcome once the bootstrap structure and contribution guide
are in place. New code should be narrowly scoped, tested, compatible with the
current minimum C source dialect, and free of unconditional dependencies on
post-Windows-2000 APIs in the legacy target.

## License

Copyright holders license WCRT under the **GNU General Public License, version
3 or any later version** (`GPL-3.0-or-later`). See [LICENSE.txt](LICENSE.txt) for
the full GPL version 3 license text.
