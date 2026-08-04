# WCRT Roadmap

This roadmap is ordered by dependency and conformance level. Dates are omitted
until measured tranche history establishes reliable forecasts. The controlled
C99-to-1.0.0 estimates and quality plan are in
[`docs/C99-1.0-WORK-PLAN.md`](docs/C99-1.0-WORK-PLAN.md). A checked item means
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

### Priority 0 — immediate

- [x] Add the minimal Microsoft-compatible `io.h` inclusion surface
      (REQ-0043) and implement the `<stdio.h>` `_fileno(FILE *stream)`
      interface (REQ-0044), with tests for the standard streams, opened files,
      invalid arguments, and compatibility with the WSP C logger.
- [ ] **In progress — WPM compatibility tranche:** WCRT now implements
      implementations carried by the latest tagged WPM release with WCRT
      requirements and implementations for `_stricmp`, `_strnicmp`,
      `fopen_s`, `strcpy_s`, `strncpy_s`, `sscanf_s`, `_utime32`, `_utime64`,
      `_stat64`, and the x86 `_stat` linker alias (REQ-0045 through REQ-0049);
      removing the corresponding WPM fallbacks remains.

- [ ] Adopt a GPL-3.0-or-later source-file notice policy.
- [x] Create the `include`, `wcrt`, `tests`, `tools`, and `docs` structure.
- [ ] Define supported TinyCC versions and acquire compiler capability probes.
- [x] Add deterministic static-library and test-executable builds.
- [ ] Define 32-bit x86 calling conventions, object format, symbol naming, and
      ABI boundaries.
- [ ] Define the strategy for x64 builds separately from the Windows 2000 x86
      compatibility target.
- [x] Add a minimal process startup/termination path and platform abstraction.
- [x] Provide the optional, separately selected WCRT console startup specified
      by REQ-0017 and verify it through TC-0017.
- [x] Provide the optional, separately selected WCRT GUI startup specified by
      REQ-0018 and verify it through TC-0018.
- [x] Add CI and repeatable local test scripts for supported Windows
      architectures.
- [ ] Add import-table inspection to catch accidental dependencies on newer
      Windows APIs.
- [ ] Create conformance, implementation-defined behavior, and deviation
      documents.

**Exit condition:** a trivial program can be compiled with TinyCC, linked with
WCRT, started, tested, and terminated on the baseline x86 target.

## Milestone 1 — C89 / C90

Deliver the first usable hosted-library baseline. Work should be divided into
independently testable header/function families.

- [x] Fundamental headers and types: `stddef.h`, `limits.h`, `float.h`,
      `stdarg.h`, `errno.h`, `assert.h`.
- [x] Character handling: `ctype.h`.
- [x] String and byte operations: `string.h`.
- [x] General utilities: `stdlib.h`, including conversion, allocation, search,
      sorting, environment, and process-control functions.
- [x] Input and output: `stdio.h`, including files, buffering, formatted I/O,
      and temporary files.
- [x] Time and date: `time.h`.
- [x] Localization: `locale.h`.
- [x] Signals: `signal.h`.
- [x] Non-local jumps: `setjmp.h`.
- [x] Mathematical functions and error behavior: `math.h`.
- [ ] Document text/binary stream mapping, newline behavior, path rules,
      locale scope, floating-point assumptions, and signal support on Windows.

**Exit condition:** the documented C89 hosted-library surface passes WCRT's
conformance suite and all common release gates.

## Milestone 2 — C99

Extend the C89 base without regressing its compatibility mode.

The ordered requirement backlog is maintained in
[`docs/C99-REQUIREMENTS.md`](docs/C99-REQUIREMENTS.md). Work proceeds in these
dependency groups:

Completion of Milestone 2, the common release gates, and a successful
release-readiness decision define the WCRT 1.0.0 boundary. C11 and the remaining
Microsoft CRT compatibility backlog are post-1.0 work.

### 2A — Fundamental types and compiler contracts

- [x] Add `stdbool.h` Boolean macros (REQ-0021).
- [x] Add `stdint.h` exact-, least-, fast-, pointer-, and maximum-width integer
      types, limits, and constant-expression macros (REQ-0022).
- [x] Extend `limits.h` for `long long` (REQ-0023) and extend `float.h` with the
      C99 evaluation and decimal-conversion model (REQ-0024).
- [x] Add `va_copy` to `stdarg.h` and audit C99 `restrict`-qualified public
      declarations without exposing C99 syntax in C89 mode (REQ-0025).
- [x] Add `iso646.h`, which C99 inherits from the C95 amendment (REQ-0026).
- [ ] Establish TinyCC capability probes for every language/compiler facility
      required by public C99 headers, especially `_Bool`, `long long`,
      `restrict`, variadic macros, complex arithmetic, and type-generic macros.

### 2B — Extended integers and formatted I/O

- [ ] Add `inttypes.h`, including `intmax_t`/`uintmax_t`, format and scan
      macros, `imaxabs`, `imaxdiv`, `strtoimax`, `strtoumax`, `wcstoimax`, and
      `wcstoumax`.
- [ ] Extend `stdlib.h` with `_Exit`, `atoll`, `llabs`, `lldiv`, `strtof`,
      `strtold`, `strtoll`, and `strtoull` and their required error behavior.
- [ ] Complete C99 formatted output and input: `hh`, `ll`, `j`, `z`, and `t`
      length modifiers; `%a`/`%A`, `%F`, and applicable wide-character
      conversions; plus `vfscanf`, `vscanf`, and `vsscanf`.
  - [x] Implement and test `snprintf` and `vsnprintf` bounded output
        (REQ-0019 / TC-0019).
- [ ] Implement hexadecimal floating-point input/output and verify rounding,
      overflow, underflow, matching failure, and return-value behavior.

### 2C — Wide and multibyte library

- [ ] Add the complete C99 `wchar.h` and `wctype.h` surfaces, including wide
      streams, conversions, string operations, classification, and mappings.
- [ ] Define the initial conversion state, `mbstate_t`, state-dependent
      encodings, restartable conversions, and interaction with locale and
      stream orientation.
- [ ] Define and verify the `wchar_t` ABI and Windows UTF-16 interoperability
      on x86, x64, and ARM64.
- [ ] After those prerequisites pass, specify and implement a separately
      selected Unicode GUI startup object for `wWinMain`; do not alias it to
      the C89 `WinMain` startup object.

### 2D — Real, complex, and type-generic mathematics

- [ ] Extend `math.h` with float and long-double variants, classification and
      comparison macros, `HUGE_VALF`, `HUGE_VALL`, infinity/NaN support,
      `math_errhandling`, and all C99 real mathematical function families.
- [ ] Add `fenv.h` and document which exception flags, rounding modes, and
      environment operations Windows and TinyCC can implement on each
      architecture.
- [ ] Add `complex.h` types, constants, real/imaginary access, and all C99
      complex mathematical function families.
- [ ] Add `tgmath.h` only after the real and complex functions and TinyCC
      type-generic dispatch behavior are verified.

### 2E — Conformance closure

- [ ] Audit every C89 header for C99 additions and changed constraints,
      including formatting, conversion, locale, stream, and macro behavior.
- [ ] Publish a clause-level C99 conformance matrix that distinguishes required,
      optional, implementation-defined, compiler-blocked, and missing items.
- [ ] Test header self-containment, type widths, macro constant-expression
      properties, C89-mode isolation, ABI behavior, and backward compatibility
      on every supported target.

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

## Parallel track — Microsoft CRT source compatibility

This track provides documented Microsoft CRT extensions needed by Windows C
programs without weakening ISO C behavior. It may advance alongside the
standards milestones when its dependencies are complete. Each interface shall
be isolated behind a Microsoft-compatibility selection, assigned its own
requirement and tests, and classified as source, ABI, or behavioral
compatibility.

The target is a documented, versioned source-compatibility profile—not a
drop-in replacement for `msvcrt.dll`, `ucrtbase.dll`, or private Visual C++
runtime internals. C++ runtime, compiler exception machinery, undocumented
symbols, and bug-for-bug compatibility remain out of scope unless separately
approved.

### MS0 — Compatibility contract and inventory

- [ ] Select and record the Microsoft documentation and Windows SDK/UCRT
      version used as the compatibility baseline.
- [ ] Inventory public Microsoft C runtime headers, functions, macros, types,
      globals, aliases, deprecations, and architecture-specific exports.
- [ ] Publish a compatibility matrix distinguishing implemented, planned,
      intentionally unsupported, compiler-provided, and OS-provided items.
- [ ] Define compatibility-mode selection, naming, calling conventions, symbol
      decoration, data model, `errno`/`_doserrno` mapping, invalid-parameter
      behavior, and interaction with ISO language-edition modes.
- [ ] Require Windows 2000 x86 import inspection for every compatible facility;
      newer APIs must use runtime detection and an acceptable fallback.

### MS1 — Common formatting, strings, paths, and conversions

- [x] **Immediate WPM dependency:** implement `_stricmp`, `_strnicmp`,
      `strcpy_s`, and `strncpy_s` (REQ-0045 and REQ-0047).

- [ ] Complete narrow and wide Microsoft formatted-I/O families, including
      `_scprintf`, `_vscprintf`, `_snscanf`, count-output policy, Microsoft
      length modifiers, and explicitly selected legacy semantics.
  - [x] Implement and test legacy `_snprintf` and `_vsnprintf`
        (REQ-0020 / TC-0020).
- [ ] Add documented secure formatting and scanning families (`*_s`) under the
      Microsoft contract, separately from ISO Annex K support.
- [ ] Add commonly used string and memory extensions: case-insensitive
      comparison, duplication, reversal, case conversion, tokenization,
      collation, bounded helpers, and their wide-character variants.
- [ ] Add integer-to-text and extended text-to-integer families, including
      `_itoa`, `_ltoa`, `_i64toa`, unsigned variants, `_atoi64`, `_strtoi64`,
      and `_strtoui64`, with secure variants where documented.
- [ ] Add drive, directory, and path construction/decomposition interfaces,
      including `_fullpath`, `_makepath`, `_splitpath`, and their secure and
      wide-character forms.

### MS2 — Files, descriptors, directories, and filesystem metadata

- [x] **Immediate WPM dependency:** implement `fopen_s` and `sscanf_s`
      (REQ-0046), `_utime32` and `_utime64` (REQ-0048), and `_stat64` plus the
      linker-compatible x86 `_stat` alias (REQ-0049).

- [ ] Complete the `io.h` low-level descriptor surface beyond the Phase 0
      `_fileno` baseline, including `_open`, `_close`, `_read`, `_write`,
      `_lseek`, `_tell`, `_commit`, `_dup`, `_dup2`, `_pipe`, `_isatty`,
      `_setmode`, and handle conversion.
- [ ] Add `fcntl.h`/`share.h` flags and sharing semantics with explicit Windows
      handle, text/binary translation, inheritance, and append behavior.
- [ ] Add `direct.h` directory and current-drive functions in narrow and wide
      forms.
- [ ] Add Microsoft `sys/stat.h`, `sys/types.h`, `_findfirst`/`_findnext`,
      file-length, access, chmod, unlink, rename, and temporary-name families,
      including 32/64-bit time and file-size variants.
- [ ] Add remaining Microsoft stream extensions such as `_fdopen`, `_fsopen`,
      `_wfopen`, `_wfreopen`, locking, flushing, mode control, and wide
      temporary-file functions.

### MS3 — Processes, environment, and runtime startup state

- [ ] Add `process.h` `_exec*`, `_spawn*`, `_cwait`, `_getpid`, `_popen`, and
      `_pclose` families with documented quoting, environment, descriptor
      inheritance, wait, and error behavior.
- [ ] Add narrow and wide environment mutation/access interfaces, including
      `_putenv`, `_wputenv`, `_dupenv_s`, environment globals, and accessors.
- [ ] Add documented startup globals and accessors such as `__argc`, `__argv`,
      `__wargv`, `_environ`, `_wenviron`, `_pgmptr`, `_wpgmptr`, `_fmode`, and
      `_commode` where consistent with the selected WCRT startup object.
- [ ] Add `onexit`/`_onexit`, `_cexit`, `_c_exit`, abort/report controls, and
      invalid-parameter handlers with deterministic multithreaded behavior.
- [ ] Keep console, `WinMain`, and `wWinMain` startup variants separately
      selectable; compatibility globals shall reflect the selected startup
      path rather than introduce an unconditional entry point.

### MS4 — Allocation, locale, time, search, and mathematics

- [ ] Add `malloc.h` aligned allocation, reallocation, offset allocation,
      size-query, stack-allocation, and heap-inspection interfaces with
      documented ownership and overflow behavior.
- [ ] Add Microsoft locale objects and per-thread locale control, including
      `_create_locale`, `_free_locale`, `_configthreadlocale`, and `_l`
      function variants after the ISO locale model is complete.
- [ ] Add Microsoft date/time extensions, including `_mkgmtime`, timezone and
      daylight accessors, secure conversion functions, and explicit 32/64-bit
      time variants.
- [ ] Add Microsoft search/sort extensions such as `qsort_s`, `bsearch_s`,
      `_lfind`, and `_lsearch`, preserving Microsoft callback conventions.
- [ ] Add documented Microsoft floating-point control, status, classification,
      and non-ISO mathematical functions only after the C99 `fenv.h` and
      `math.h` requirements establish the architecture model.

### MS5 — Multithreaded CRT behavior

- [ ] Implement `_beginthread`, `_beginthreadex`, `_endthread`, and
      `_endthreadex` over the WCRT threading/platform layer; document handle
      ownership and why raw `CreateThread` may bypass CRT initialization.
- [ ] Make `errno`, `_doserrno`, locale state, multibyte conversion state,
      `strtok`-family state, temporary buffers, invalid-parameter handlers, and
      other required runtime data correctly thread-local or synchronized.
- [ ] Define stream, heap, environment, exit-handler, and locale locking and
      verify deadlock, recursive-entry, teardown, and process-exit behavior.
- [ ] Validate compatibility on x86, x64, and ARM64, with runtime fallbacks for
      synchronization APIs unavailable on the Windows 2000 x86 baseline.

### MS6 — Compatibility closure

- [ ] Build representative open-source Windows C consumers against WCRT's
      compatibility mode without source patches and record all exclusions.
- [ ] Add header self-containment, symbol/export, ABI, negative, concurrency,
      and ISO-mode-isolation tests for every compatibility family.
- [ ] Verify that Microsoft aliases and secure functions never silently change
      the corresponding ISO function's contract.
- [ ] Publish the supported Microsoft CRT profile and freeze its compatibility
      and deprecation policy for the first stable WCRT release.

**Exit condition:** every interface in the selected Microsoft CRT profile has
a requirement, verification evidence, compatibility classification, and
documented deviation; private and unsupported interfaces are explicitly
listed, and all applicable ISO and legacy-Windows gates remain green.

## Platform validation matrix

- [x] Cross-build implemented C89 requirements as PE/COFF ARM64 with TinyCC.
- [x] Execute the C89 behavioral suite on a native Windows ARM64 GitHub runner.

The exact matrix will be fixed during bootstrap. The intended tiers are:

| Target | Architecture | Validation level |
| --- | --- | --- |
| Windows 2000 | x86 | Baseline compatibility |
| Windows XP / Server 2003 | x86 | Legacy regression |
| Windows Vista / 7 / 8.x | x86 and applicable x64 | Compatibility regression |
| Windows 10 / 11 | x86 where supported, x64, ARM64 | Primary development and release |

Native testing is preferred. Emulators and virtual machines may supplement but
must not silently replace platform validation. Windows editions that cannot run
a given architecture are outside that architecture's test row.

## Beyond C23

After the C23 milestone, likely work includes performance tuning, fuzzing,
hardening, additional architectures, improved diagnostics, packaging, and
carefully namespaced extensions. None of these should weaken standards
conformance or the documented legacy-Windows compatibility contract.
