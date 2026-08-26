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

WCRT has a working C89 library baseline and has implemented the planned C99
library surface through the T6 integration-closure tranche. The C89 headers
and runtime are covered by requirement-linked presence and behavioral tests
(TC-0001 through TC-0015),
while source quality and the optional console and GUI startup paths are covered
by TC-0016 through TC-0018.
The implemented C99 allocations include REQ-0019, REQ-0021 through REQ-0033,
and REQ-0035 through REQ-0042 as detailed by the
[C99 to 1.0.0 work plan](docs/C99-1.0-WORK-PLAN.md). CI resolves the latest
eligible `tinycc` package from the `tcc_package` WPM repository at workflow
execution, records the exact selected version and hashes, and rejects a mixed
compiler baseline across targets. The retained T6 closure baseline is
`0.9.28-rc.1442+2474e1c2`; successor run `32533622277` passes the complete
native Debug matrix and package assembly with latest-selected
`0.9.28-rc.1444+9a4be30f`. Native ARM64
Release execution, release trust, installation/rollback, and independent
release review remain acceptance work.

Continuous integration builds and tests x86, x64, and ARM64 on native Windows
runners. WCRT is still pre-release and is not yet suitable for production use;
the remaining C89 release gates and compatibility documentation must be closed
before the project makes an unqualified C89 conformance claim.

See [ROADMAP.md](ROADMAP.md) for planned work and milestone completion criteria.
The selected scope for the next compatibility-focused release is in the
[WCRT 1.1.0 compatibility roadmap](docs/WCRT-1.1.0-ROADMAP.md).

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

The supported TinyCC 0.9.28rc targets retain a later `__STDC_VERSION__` value
under `-std=c89`. A consumer selecting WCRT's strict C89 header surface shall
therefore define `WCRT_C89=1`; the controlled C99 tests verify that this
selector removes the C99-only interfaces from affected public headers.

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
recorded separately from runtime limitations. ADR-0005 permits only the exact,
source-specific `_Complex` type/parser and imaginary-literal diagnostics in its
controlled matrix as non-fatal ExpectedFail results.
The retained `0.9.28-rc.1442+2474e1c2` baseline passes those probes. Every
newer package selected by WPM must pass the same probes and complete matrix, so
WCRT builds include the complex runtime and require its complete export inventory.
If a controlled diagnostic recurs, packages include `complex.h` and `tgmath.h`
for implementation
review but omit complex runtime symbols and disclose that state in
`C99-COMPLEX-PROFILE.txt` and per-architecture JSON. Any other failure remains
release-blocking.

## Repository layout

```text
include/          Public standard and WCRT headers
src/              Standards-facing runtime implementation
src/platform/     Windows and architecture-specific code
tests/            Unit, regression, integration, and conformance tests
tools/            Build and test utilities
docs/             Design notes and compatibility documentation
```

The standards-facing implementation currently lives directly under `src/`,
with Windows-specific code under `src/platform/windows/`.

## Building

The build requires PowerShell and an architecture-matched TinyCC installation.
Debug builds also require `cv2pdb.exe` in `PATH`. Install the signed WPM
package once from an elevated PowerShell session:

```powershell
Invoke-WebRequest `
  https://github.com/Thewafflication/cv2pdb_package/releases/latest/download/wpm-release.public `
  -OutFile cv2pdb-wpm-release.public
wpm trust add cv2pdb-wpm-release.public
wpm repo add https://github.com/Thewafflication/cv2pdb_package/releases/latest/download
wpm update
wpm install cv2pdb
```

The package sets `CV2PDB_HOME` and adds that directory to the system `PATH`.

Initialize the pinned Waughtal Software Process (WSP) submodule after cloning:

```powershell
git submodule update --init wsp
```

```powershell
./tools/build-wcrt.ps1 -Architecture x64 -Configuration Debug -TinyCc $env:TCC_HOME/tcc.exe
./tools/run-c89-tests.ps1 -Architecture x64 -TinyCc $env:TCC_HOME/tcc.exe
./tools/run-extension-tests.ps1 -Architecture x64 -TinyCc $env:TCC_HOME/tcc.exe
```

GitHub Actions builds and tests x86, x64, and ARM64 independently. TC-0016 is
always run first. Each job publishes the debug DLL, PDB, and a TeX test-results
table. After every Debug job passes, the workflow also publishes an unsigned
`wcrt-debug-any-<version>.zip` WPM package containing all three targets. It
installs separately beneath `%ProgramFiles%\WCRT\Debug` and sets
`WCRT_DEBUG_HOME`. The candidate source may be committed before optimized
Release evidence exists. A `MAJOR.MINOR.PATCH` or `vMAJOR.MINOR.PATCH` tag
starts Release builds and native consumer/startup smoke tests on all three
architectures, then combines them into one WPM development package whose base
version is taken from the tag.
As with the TinyCC package, WPM versions normalize a tag suffix, add a
development commit distance when applicable, append the eight-character Git
revision as build metadata, and add `.dirty` for modified working trees. An
exact tagged build uses the tag version without commit metadata.
The DLL contains a Windows `VERSIONINFO` resource with this package version,
its numeric file version, architecture, configuration, source revision,
package name, license, and repository information.

Each architecture build produces the shared `wcrt.dll`, its TinyCC import definition
`wcrt.def`, the static TinyCC archive `libwcrt.a`, the optional
`wcrt-startup-console.o` and `wcrt-startup-gui.o` startup objects, and a copy of
the public headers. TinyCC's compiler-support library supplies its private
complex arithmetic helpers on every target. The WPM package installs shared
headers beneath `include`
and target files beneath `x86`, `x64`, and `arm64` architecture directories.
The package also contains the C99 complex capability/profile records. A target
whose compiler passes the probe contains the complex runtime; a controlled
ExpectedFail target remains packageable with that runtime explicitly omitted.
After all Debug architecture jobs pass, a semantic-version tag builds Release
artifacts for x86, x64, and ARM64. Package assembly consumes those exact
artifacts, applies the separate WPM signature, requires `wpm verify` exit zero,
and compares every packaged DLL with its Release input. Publication depends on
the complete `build` -> `release` -> `package` chain. Only then can the workflow
publish one WPM-signed `arch=any` package containing every target, the public
WPM key, and repository `index.json` to the corresponding GitHub Release.
Authenticode identity/timestamping and Defender scanning are explicitly
deferred from 1.0.0, not represented as Pass. Remaining readiness blockers are
tracked in the project release-readiness record.

An ARM64 C99 consumer links the DLL with the import definition just like an
x86 or x64 consumer:

```powershell
tcc program.c $env:WCRT_HOME/arm64/lib/wcrt.def -o program.exe
```

WCRT's public complex functions remain imports from `wcrt.dll`; TinyCC resolves
its private operator helpers from its ordinary compiler-support library.

## Optional static process startup

An ordinary link against `libwcrt.a` continues to use TinyCC's process startup.
Consumers that want WCRT to own the PE entry point select exactly one startup
object and omit the toolchain runtime startup. The TinyCC compiler-support
archive remains necessary for architecture-specific compiler helpers; it is
not a C runtime startup.

For a console application defining `main`, use the architecture-matched files:

```powershell
tcc -nostdlib -Wl,-nostdlib -Wl,-subsystem=console `
  $env:WCRT_HOME/x64/lib/wcrt-startup-console.o program.c `
  $env:WCRT_HOME/x64/lib/libwcrt.a `
  $env:TCC_HOME/lib/x86_64-win32-libtcc1.a `
  $env:TCC_HOME/lib/kernel32.def -o program.exe
```

For a GUI application defining ANSI `WinMain`, select the GUI object and
Windows subsystem:

```powershell
tcc -nostdlib -Wl,-nostdlib -Wl,-subsystem=windows `
  $env:WCRT_HOME/x64/lib/wcrt-startup-gui.o program.c `
  $env:WCRT_HOME/x64/lib/libwcrt.a `
  $env:TCC_HOME/lib/x86_64-win32-libtcc1.a `
  $env:TCC_HOME/lib/kernel32.def -o program.exe
```

Replace both `x64` and `x86_64-win32` with `x86` and `i386-win32`, or with
`arm64` and `arm64-win32`, for those targets.
The console startup applies Windows quote and backslash parsing to construct
`argc` and `argv`. The GUI startup passes the command line after the executable
name to `WinMain` and uses `STARTUPINFO.wShowWindow` when Windows supplies it,
or `SW_SHOWDEFAULT` otherwise. Unicode `wWinMain` startup is deferred until the
C99 wide-character ABI and UTF-16 interoperability requirements are complete.

## Installing with WPM

Trust the WCRT release key once, add the GitHub Release as a WPM repository,
refresh its index, and install the multi-architecture development package:

```powershell
Invoke-WebRequest `
  https://github.com/Thewafflication/wcrt/releases/latest/download/wpm-release.public `
  -OutFile wpm-release.public
wpm trust add wpm-release.public
wpm repo add https://github.com/Thewafflication/wcrt/releases/latest/download
wpm update
wpm install wcrt
```

One installation provides the headers, DLL, static library, import definition,
and startup objects for x86, x64, and ARM64. Build scripts select the desired
target beneath `%WCRT_HOME%\x86`, `%WCRT_HOME%\x64`, or `%WCRT_HOME%\arm64`.

To install a prerelease version, enable prereleases only for WCRT before
updating the repository:

```powershell
wpm config set prerelease true --package wcrt
wpm update
wpm install wcrt
```

## Conformance

WCRT maintains the current C99 clause/facility matrix in
[`docs/c99-conformance-profile.md`](docs/c99-conformance-profile.md). A
milestone is not complete merely because all named headers and functions exist;
it must also meet its behavioral tests, supported-platform tests,
documentation gate, and TinyCC build gate.

Where the C standard permits implementation choices, WCRT will document them
under `docs/implementation-defined.md`. Known deviations will be tracked openly
and will block an unqualified conformance claim.

Release trust order, rollback, and support are defined in
[`docs/release-process.md`](docs/release-process.md); the project threat and
dependency model is in
[`docs/security/design-for-security.md`](docs/security/design-for-security.md).
The deferred Authenticode service, identity, verification, evidence, and
certificate lifecycle design is retained in
[`docs/windows-signing-plan.md`](docs/windows-signing-plan.md) for future
reassessment; it is not a WCRT 1.0 gate.

## Contributing

Contributions are welcome once the bootstrap structure and contribution guide
are in place. New code should be narrowly scoped, tested, compatible with the
current minimum C source dialect, and free of unconditional dependencies on
post-Windows-2000 APIs in the legacy target.

## License

Copyright holders license WCRT under the **GNU General Public License, version
3 or any later version** (`GPL-3.0-or-later`). See [LICENSE.txt](LICENSE.txt) for
the full GPL version 3 license text.
