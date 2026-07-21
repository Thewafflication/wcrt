# WCRT Roadmap

This roadmap is ordered by dependency and conformance level. Dates are omitted
until the bootstrap work establishes reliable estimates. A checked item means
the work is present, tested, documented, and passing the relevant release
gates—not merely started.

## Release gates for every milestone

Each standards milestone must satisfy all of these gates:

- [ ] Public headers compile cleanly as C with TinyCC.
- [ ] Debug and release library builds succeed with the supported TinyCC
      versions.
- [ ] Unit, regression, and integration tests pass on the primary Windows
      targets.
- [ ] The 32-bit compatibility build has no unconditional imports newer than
      Windows 2000.
- [ ] Supported architecture and operating-system combinations are tested or
      explicitly identified as community-tested.
- [ ] Implementation-defined and unspecified choices are documented.
- [ ] Known standard deviations are documented and reflected in the
      conformance matrix.
- [ ] Earlier completed milestones continue to pass their test suites.
- [ ] Source archives contain licensing notices and reproducible build
      instructions.

## Phase 0 — Bootstrap

Establish a runtime that can be built and tested before making a standards
conformance claim.

- [ ] Adopt a GPL-3.0-or-later source-file notice policy.
- [ ] Create the `include`, `wcrt`, `tests`, `tools`, and `docs` structure.
- [ ] Define supported TinyCC versions and acquire compiler capability probes.
- [ ] Add deterministic static-library and test-executable builds.
- [ ] Define 32-bit x86 calling conventions, object format, symbol naming, and
      ABI boundaries.
- [ ] Define the strategy for x64 builds separately from the Windows 2000 x86
      compatibility target.
- [ ] Add a minimal process startup/termination path and platform abstraction.
- [ ] Add CI or repeatable local test scripts for supported Windows tiers.
- [ ] Add import-table inspection to catch accidental dependencies on newer
      Windows APIs.
- [ ] Create conformance, implementation-defined behavior, and deviation
      documents.

**Exit condition:** a trivial program can be compiled with TinyCC, linked with
WCRT, started, tested, and terminated on the baseline x86 target.

## Milestone 1 — C89 / C90

Deliver the first usable hosted-library baseline. Work should be divided into
independently testable header/function families.

- [ ] Fundamental headers and types: `stddef.h`, `limits.h`, `float.h`,
      `stdarg.h`, `errno.h`, `assert.h`.
- [ ] Character handling: `ctype.h`.
- [ ] String and byte operations: `string.h`.
- [ ] General utilities: `stdlib.h`, including conversion, allocation, search,
      sorting, environment, and process-control functions.
- [ ] Input and output: `stdio.h`, including files, buffering, formatted I/O,
      and temporary files.
- [ ] Time and date: `time.h`.
- [ ] Localization: `locale.h`.
- [ ] Signals: `signal.h`.
- [ ] Non-local jumps: `setjmp.h`.
- [ ] Mathematical functions and error behavior: `math.h`.
- [ ] Document text/binary stream mapping, newline behavior, path rules,
      locale scope, floating-point assumptions, and signal support on Windows.

**Exit condition:** the documented C89 hosted-library surface passes WCRT's
conformance suite and all common release gates.

## Milestone 2 — C99

Extend the C89 base without regressing its compatibility mode.

- [ ] Add C99 headers and types, including `stdint.h`, `inttypes.h`,
      `stdbool.h`, `complex.h`, `fenv.h`, and `tgmath.h` as supported.
- [ ] Add `long long`, extended integer conversion, and integer format macros.
- [ ] Add C99 formatted-I/O behavior, including `snprintf`/`vsnprintf` and new
      conversion requirements.
- [ ] Add wide-character and multibyte facilities from `wchar.h` and
      `wctype.h`.
- [ ] Add C99 mathematical classification, comparison, and function families.
- [ ] Add hexadecimal floating-point and other C99 numeric conversions.
- [ ] Define support for complex arithmetic and floating-point environment
      behavior under TinyCC and Windows.
- [ ] Test C99 header behavior, type widths, macros, and backward compatibility.

**Exit condition:** supported C99 library facilities pass their conformance
matrix, with unsupported optional facilities explicitly identified.

## Milestone 3 — C11

Add C11 library facilities, accounting carefully for compiler and operating
system capabilities.

- [ ] Add `stdalign.h`, `stdatomic.h`, `stdnoreturn.h`, `threads.h`, and
      `uchar.h` surfaces where implementable.
- [ ] Implement aligned allocation and C11 allocation requirements.
- [ ] Implement bounds-checking interfaces only under their optional Annex K
      contract; do not imply Annex K support merely by exposing similarly named
      extensions.
- [ ] Implement UTF-16/UTF-32 character conversion facilities.
- [ ] Define atomic guarantees by architecture and TinyCC capability.
- [ ] Implement C11 threads over Windows synchronization primitives with
      runtime fallbacks where newer APIs are optional.
- [ ] Implement thread-local runtime state needed by `errno`, locales, streams,
      and other library facilities.

**Exit condition:** the supported C11 library profile, atomic guarantees, and
threading behavior are tested and documented per architecture.

## Milestone 4 — C17

C17 primarily corrects and clarifies C11, so this milestone emphasizes defect
resolution and stability rather than API volume.

- [ ] Audit C11 behavior against corrections incorporated into C17.
- [ ] Add regression tests for all applicable defect reports.
- [ ] Resolve or document every known C11-era conformance deviation.
- [ ] Revalidate atomics, threads, floating-point behavior, and wide-character
      conversion across supported targets.
- [ ] Freeze the stable compatibility and ABI policy for the 1.x line.

**Exit condition:** the C17 conformance matrix has no undocumented deviation
and all earlier compatibility modes remain green.

## Milestone 5 — C23

Implement C23 as compiler support and final standard requirements become
available in the supported TinyCC baseline.

- [ ] Inventory C23 library additions, removals, deprecations, and changed
      semantics against the adopted standard text.
- [ ] Add new headers and library interfaces supported by the target toolchain.
- [ ] Implement checked integer arithmetic and other new utility facilities.
- [ ] Implement new bit-manipulation facilities and exact-width type behavior
      where the platform representation permits them.
- [ ] Update character, Unicode, formatting, time, and mathematical facilities
      for C23 requirements.
- [ ] Define behavior for interfaces removed or deprecated by C23 while
      preserving older compatibility modes.
- [ ] Add capability-based fallbacks for incomplete TinyCC C23 language
      support; keep compiler gaps distinct from runtime conformance gaps.

**Exit condition:** WCRT publishes a tested C23 support profile and explicitly
classifies every omitted, optional, or compiler-blocked facility.

## Platform validation matrix

The exact matrix will be fixed during bootstrap. The intended tiers are:

| Target | Architecture | Validation level |
| --- | --- | --- |
| Windows 2000 | x86 | Baseline compatibility |
| Windows XP / Server 2003 | x86 | Legacy regression |
| Windows Vista / 7 / 8.x | x86 and applicable x64 | Compatibility regression |
| Windows 10 / 11 | x86 where supported, x64 | Primary development and release |

Native testing is preferred. Emulators and virtual machines may supplement but
must not silently replace platform validation. Windows editions that cannot run
a given architecture are outside that architecture's test row.

## Beyond C23

After the C23 milestone, likely work includes performance tuning, fuzzing,
hardening, additional architectures, improved diagnostics, packaging, and
carefully namespaced extensions. None of these should weaken standards
conformance or the documented legacy-Windows compatibility contract.
