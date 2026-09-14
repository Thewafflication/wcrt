# Waughtal C Run Time

[![Build and test WCRT](https://github.com/Thewafflication/wcrt/actions/workflows/build.yml/badge.svg)](https://github.com/Thewafflication/wcrt/actions/workflows/build.yml)
[![Latest release](https://img.shields.io/github/v/release/Thewafflication/wcrt)](https://github.com/Thewafflication/wcrt/releases/latest)

Waughtal C Run Time (**WCRT**) is a compact, standards-oriented C runtime for
Windows. It is built with TinyCC and supports x86, x64, and ARM64. The x86
runtime deliberately uses APIs available on Windows 2000; x64 and ARM64 apply
to Windows versions that support those architectures.

WCRT provides an ISO C89/C90 baseline, a tested C99 surface, selected Microsoft
CRT and POSIX compatibility interfaces, optional PE startup objects, and a
small native thread-pool extension. It can be linked as a DLL or a static
library.

WCRT is developed incrementally. An interface is considered supported only
when it has a requirement, an implementation, and a passing test. The
[requirements index](docs/REQUIREMENTS.md) is the authoritative inventory.

## What is included

| Profile | Current surface |
| --- | --- |
| ISO C | Hosted C89/C90 baseline and the documented C99 library profile |
| Microsoft compatibility | Selected low-level I/O, file metadata, secure I/O/string helpers, string and integer extensions, command streams, and underscore aliases |
| POSIX compatibility | Selected descriptors, files, directories, paths, strings, time, process identity, option parsing, filename matching, command streams, threads, and read-only file mapping |
| WCRT extensions | Fixed-size worker thread pools with submission, draining, shutdown, and destruction |
| Platforms | x86, x64, and ARM64 Windows; Windows 2000 compatibility is an x86 guarantee |

The POSIX layer is a bounded Windows portability profile, not a complete POSIX
implementation. Define `WCRT_POSIX=1` before including affected headers. WCRT
does not define `_POSIX_VERSION`.

Notable limits:

- `<pthread.h>` provides mutexes, condition variables, lifecycle operations,
  one-time initialization, and thread-specific storage. It is not the complete
  pthread API.
- `<sys/mman.h>` supports read-only file mappings. Writable, anonymous, fixed,
  and executable mappings are rejected.
- Microsoft compatibility is source-oriented and selected; WCRT is not a
  drop-in replacement for every MSVCRT or UCRT interface.
- Optional console and GUI startup objects are separate from `libwcrt.a` and
  must be selected explicitly.

See the [documentation guide](docs/README.md) for the conformance profile,
platform model, requirements, tests, and release policy.
Tagged releases publish the generated API reference to
[GitHub Pages](https://thewafflication.github.io/wcrt/).

## Install with WPM

Trust the WCRT package key once, add the release repository, and install:

```powershell
Invoke-WebRequest `
  https://github.com/Thewafflication/wcrt/releases/latest/download/wpm-release.public `
  -OutFile wpm-release.public
wpm trust add wpm-release.public
wpm repo add https://github.com/Thewafflication/wcrt/releases/latest/download
wpm update
wpm install wcrt
```

One package installs headers and x86, x64, and ARM64 libraries. Target files
are beneath `%WCRT_HOME%\x86`, `%WCRT_HOME%\x64`, and
`%WCRT_HOME%\arm64`.

To allow prerelease packages for WCRT only:

```powershell
wpm config set prerelease true --package wcrt
wpm update
wpm install wcrt
```

## Build from source

Requirements:

- PowerShell 7
- an architecture-matched TinyCC installation
- `cv2pdb.exe` in `PATH` for Debug builds
- the pinned WSP submodule

Initialize the repository and build an x64 Debug runtime:

```powershell
git submodule update --init wsp
./tools/build-wcrt.ps1 `
  -Architecture x64 `
  -Configuration Debug `
  -TinyCc $env:TCC_HOME/tcc.exe
```

Run the ISO and compatibility suites:

```powershell
./tools/run-c89-tests.ps1 `
  -Architecture x64 `
  -TinyCc $env:TCC_HOME/tcc.exe
./tools/run-extension-tests.ps1 `
  -Architecture x64 `
  -TinyCc $env:TCC_HOME/tcc.exe
```

Each build produces:

```text
wcrt.dll                 Shared runtime
wcrt.def                 TinyCC import definition
libwcrt.a                Static runtime
wcrt-startup-console.o   Optional console startup
wcrt-startup-gui.o       Optional GUI startup
include/                 Public headers
```

## Link a program

Link against the DLL import definition:

```powershell
tcc program.c $env:WCRT_HOME/x64/lib/wcrt.def -o program.exe
```

Or link the static runtime:

```powershell
tcc program.c $env:WCRT_HOME/x64/lib/libwcrt.a -o program.exe
```

Replace `x64` with `x86` or `arm64` as appropriate.

### Select strict C89 headers

The supported TinyCC toolchains report a later `__STDC_VERSION__` even with
`-std=c89`. Define `WCRT_C89=1` when a consumer needs WCRT's strict C89-only
header surface:

```powershell
tcc -std=c89 -DWCRT_C89=1 program.c `
  $env:WCRT_HOME/x64/lib/wcrt.def -o program.exe
```

### Select POSIX interfaces

Define `WCRT_POSIX` in the source or on the compiler command line:

```c
#define WCRT_POSIX 1

#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
```

```powershell
tcc -DWCRT_POSIX=1 program.c `
  $env:WCRT_HOME/x64/lib/wcrt.def -o program.exe
```

### Use the worker pool

The pool is a WCRT extension rather than a POSIX interface:

```c
#include <wcrt/thread_pool.h>

static void work(void *context)
{
    /* Process context. */
}

int main(void)
{
    wcrt_thread_pool *pool = wcrt_thread_pool_create(4);
    if (pool == NULL)
        return 1;
    if (wcrt_thread_pool_submit(pool, work, NULL) != 0) {
        wcrt_thread_pool_destroy(pool);
        return 1;
    }
    wcrt_thread_pool_destroy(pool);
    return 0;
}
```

Destruction drains accepted work and joins every worker. A task must not call
`wcrt_thread_pool_wait`, `wcrt_thread_pool_shutdown`, or
`wcrt_thread_pool_destroy` on its own pool.

## Optional WCRT process startup

Ordinary links use TinyCC's process startup. To let WCRT own the PE entry
point, choose exactly one architecture-matched startup object and use
`-nostdlib`. For example, an x64 console program defining `main` can use:

```powershell
tcc -nostdlib -Wl,-nostdlib -Wl,-subsystem=console `
  $env:WCRT_HOME/x64/lib/wcrt-startup-console.o program.c `
  $env:WCRT_HOME/x64/lib/libwcrt.a `
  $env:TCC_HOME/lib/x86_64-win32-libtcc1.a `
  $env:TCC_HOME/lib/kernel32.def -o program.exe
```

Use `wcrt-startup-gui.o` with `-Wl,-subsystem=windows` for an ANSI `WinMain`
program. See the startup requirements in the
[requirements index](docs/REQUIREMENTS.md) for the exact contract.

## Testing and releases

GitHub Actions builds and tests x86, x64, and ARM64 independently. The matrix
checks ISO behavior, compatibility extensions, public consumers, startup
objects, source quality, requirement traceability, package assembly, and the
Windows 2000 x86 import allowlist.

Every push produces verified Debug artifacts. A semantic-version tag also
builds optimized Release artifacts, assembles and verifies the signed WPM
package, and publishes the corresponding GitHub Release only after the full
dependency chain succeeds. See the [release process](docs/release-process.md).
In parallel with the Release builds, a reusable documentation workflow creates
x86, x64, and ARM64 test-execution PDFs, a Doxygen API-reference PDF, and one
offline HTML/LaTeX/PDF documentation ZIP. Release publication also waits for
the GitHub Pages deployment.

## Project principles

- Preserve completed C89 and C99 behavior as compatibility work expands.
- Keep platform-specific code under `src/platform/`.
- Avoid unconditional post-Windows-2000 APIs in the x86 compatibility build.
- Test observable behavior, failures, ABI assumptions, and header isolation.
- Document extensions, omissions, implementation choices, and deviations.

The long-range standards and compatibility direction is recorded in
[ROADMAP.md](ROADMAP.md). Planning documents describe intended work; they do
not override the implemented requirements and test manifests.

## Contributing

Contributions should be narrowly scoped, requirement-linked, tested with
TinyCC, and compatible with the documented platform floor. Run the relevant
focused test plus both aggregate suites before submitting a change.

## License

WCRT is licensed under GPL-3.0-or-later. See [LICENSE.txt](LICENSE.txt).
