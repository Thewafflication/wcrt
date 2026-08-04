# C99 T1 Integer Core Work Log

**Status:** Local implementation and personal review complete; native ARM64 pending

**Owner:** WCRT maintainer

**Created:** 2026-08-04

## Baseline and Scope

Implement REQ-0027 (`inttypes.h`) and REQ-0028 (C99 general utilities) for
the supported x86, x64, and ARM64 Windows targets. The controlled scope is all
applicable `PRI*` and `SCN*` macros, `imaxabs`, `imaxdiv`, `strtoimax`,
`strtoumax`, `_Exit`, `atoll`, `llabs`, `lldiv`, `strtof`, `strtold`,
`strtoll`, and `strtoull`, including declarations, expression types, parsing
boundaries, range errors, C89 isolation, and ABI behavior.

Hexadecimal floating syntax, infinity/NaN subject sequences, formatted-I/O
engine support for the new length modifiers, wide integer conversions, and
`WINT_*` are excluded. Hexadecimal floating conversion remains controlled by
planned REQ-0039; formatted-I/O execution remains controlled by REQ-0029 and
REQ-0030; wide facilities remain controlled by REQ-0031 through REQ-0033.

**Controlled size:** Two requirements and two matching controlled tests.

**Estimated change size:** 14–22 artifacts and 900–1,600 changed lines,
including specifications, tests, implementation, runner integration, and
closeout records.

## Estimate

| Phase | Focused hours |
| --- | ---: |
| Plan and specify | 6–10 |
| Design | 6–10 |
| Implement | 18–30 |
| Personal review | 5–9 |
| Verify and evidence | 10–18 |
| Postmortem and re-estimate | 3–8 |
| **Total** | **48–85** |

## Architecture and Optional Choices

- Windows uses ILP32 on x86 and LLP64 on x64/ARM64. `intmax_t` and
  `uintmax_t` are respectively `long long` and `unsigned long long` on every
  supported target.
- `intptr_t`/`uintptr_t` formatting uses 32-bit integer conversions on x86 and
  `long long` conversions on x64/ARM64, matching `stdint.h` typedef identity.
- The least-width and exact-width format macros follow their promoted argument
  types. Fast 8/16/32-bit types are `int`/`unsigned int` on every target.
- C99 requires the exact-width, least-width, fast-width, maximum-width, and
  pointer-width macro families when their corresponding typedefs exist; WCRT
  defines all of them because REQ-0022 defines every corresponding typedef.
- `long double` has the binary64 Windows/TinyCC representation and calling
  convention documented in `docs/c99-platform-model.md`; `strtold` therefore
  has `strtod` numeric range and precision while retaining its declared return
  type.
- `WINT_*` remains inapplicable until `wint_t` is baselined. No optional
  `inttypes.h` interface is otherwise omitted.
- Invalid integer bases are outside the C99 function domain. WCRT returns zero,
  stores the original input in the end pointer, and leaves `errno` unchanged as
  a documented defensive extension; controlled conformance tests use only base
  zero or 2–36.
- `_Exit` uses the existing process-termination primitive, does not invoke
  `atexit` handlers, and preserves the existing Windows exit-status ABI.

## Quality Plan

- Review and baseline REQ-0027/TC-0027 and REQ-0028/TC-0028 before changing
  public headers or runtime implementation.
- Compile self-contained and repeated-inclusion header tests with warnings as
  errors; check exact macro spellings and expression/field types.
- Exercise integer bases, prefixes, signs, no-conversion cases, end pointers,
  exact extrema, one-past extrema, and `errno` preservation/range errors.
- Exercise decimal floating syntax, exponent rollback, signed zero,
  representative normal values, overflow, underflow, end pointers, `errno`
  preservation, and the published `double`/`long double` ABI model.
- Use child processes to distinguish `_Exit` from normal `exit` cleanup.
- Run direct C89-isolation tests and the full C89 suite; verify x86, x64, and
  ARM64 compilation/ABI, with native execution only where available.
- Run aggregate C99/compatibility, traceability, source-quality, build,
  consumer, startup, evidence-format, and import regressions applicable to the
  changed surface.
- Perform requirement, design, code, test, documentation, compatibility, ABI,
  warning, and final-diff review; disposition every finding below.

## Time Log

| Date | Phase | Focused minutes | Excluded interruption | Note |
| --- | --- | ---: | --- | --- |

No focused time is inferred from chat or command wall time.

## Defect Log

| ID | Type | Injected | Removed | Fix minutes | Related artifact | Disposition |
| --- | --- | --- | --- | ---: | --- | --- |
| T1-D001 | test | Test implementation | Initial test | — | `Invoke-C99UtilityTest.ps1` | Removed a stale `$LASTEXITCODE` check after a successful nested PowerShell runner; TC-0013 intentionally leaves the child `_Exit` status in that process-global variable. |
| T1-D002 | numeric | Implementation | Personal code review | — | `src/stdlib.c` | Changed decimal normalization so leading integer and fractional zeros do not exhaust the 18 stored significant digits; added a long-leading-zero regression. |
| T1-D003 | documentation | Test specification | Traceability verification | — | `tc-0028-stdlib-c99.tex` | Rephrased a planned-requirement reference because controlled test specifications may reference only baselined requirement IDs. |
| T1-D004 | algorithm | Implementation | Final code review | — | `src/stdlib.c` | Initialized the internal overflow result on the documented defensive invalid-base path, preventing an indeterminate branch in `strtoll`. |

## Checklist

- [x] Requirement and verification specifications are reviewable before code.
- [x] Public declarations and ABI choices match all supported targets.
- [x] Applicable macro families and expression types are complete.
- [x] Integer and floating conversions cover boundaries and error behavior.
- [x] C89 isolation and existing C89 behavior remain green.
- [x] Manifest, aggregate runner, traceability, and retained evidence agree.
- [x] Personal design, code, test, and documentation review is complete.
- [x] Relevant x86, x64, and ARM64 regressions are recorded accurately.
- [x] Actual size, evidence, postmortem, and revised T2 estimate are recorded.

## Personal Review Results

- Requirements: every requested interface, boundary/error class, edition rule,
  optional choice, and later-tranche exclusion maps to a controlled test or an
  explicit target-only limitation.
- Design and ABI: integer typedef identities agree with REQ-0022; macro
  modifiers follow default argument promotions and pointed-to scan types;
  x86 uses 32-bit pointer modifiers and x64/ARM64 use `ll`; `lldiv_t` and
  `imaxdiv_t` preserve member order and natural Windows layout; no new platform
  import was introduced.
- Code: accumulation is checked before multiplication, signed minima avoid
  positive signed overflow, decimal exponent rollback and leading-zero
  normalization were inspected, normal `exit` and `_Exit` remain distinct,
  and C99 declarations are edition guarded.
- Tests: positive, negative, boundary, no-conversion, preservation, range,
  expression-type, repeated-inclusion, C89-isolation, child-process, manifest,
  and architecture partitions were reviewed. Findings T1-D001 through D003
  are fixed and covered.
- Documentation/compatibility: requirements, test specifications, indices,
  strategy, platform decisions, work plan, manifest, aggregate inventory, and
  evidence statements agree. Source quality and final whitespace checks pass.

## Local Verification Evidence

- TC-0027 and TC-0028 pass natively with TinyCC 0.9.28rc on x86 and x64.
  TC-0028's `_Exit` child returns 23 rather than the callback's 99.
- The extension aggregate passes 17 cases on x86 and x64, including nine C99
  cases (TC-0019 and TC-0021 through TC-0028) and eight Microsoft-
  compatibility cases; the manifest-to-runner inventory gate passes.
- The full C89 aggregate passes TC-0001 through TC-0015 on x86 and x64.
- ARM64 TC-0027/TC-0028 presence objects and behavioral/child executables
  compile and link with TinyCC 0.9.28rc. Its capability record passes LLP64,
  64-bit `long long`, 64-bit pointer/size types, and binary64 `long double`.
- Fresh Debug DLLs, static libraries, headers, and startup objects build for
  x86, x64, and ARM64. Static and DLL consumers link and execute on x86/x64;
  ARM64 consumers link, but execution is unavailable on the x64 host.
- Traceability passes for 35 requirements, specifications, and implementations.
  TC-0016 scans 136 C/header files with zero violations. All 15 pinned WSP
  common-tool checks pass.
- x86 and x64 startup tests pass. The fresh x86 DLL passes the Windows 2000
  import allowlist with no unexpected import and SHA-256
  `02D7B9DFBE5D727D765E6C07FD696A5A55C6C27D38D2D40FC4030444A26D6611`.
- `git diff --check` passes. No commit, push, or publication was performed.

## Remaining Target-Only Evidence

- Execute TC-0027, TC-0028, the full C89 and extension aggregates, consumers,
  and startup tests on the native `windows-11-arm` CI runner. Local ARM64
  evidence is compile/link/PE evidence and is not represented as execution.
- Run the changed GitHub Actions workflow to confirm workflow parsing, summary
  rendering, enforcement, and artifact retention for the exact source change.

## Postmortem

T1 changed 24 artifacts with 1,149 additions and 20 deletions (1,169 changed
lines), within the 900–1,600-line inspected estimate. Runtime/header
implementation accounts for 450 added lines; controlled records, tests, and
runner integration account for the remainder. No focused effort or elapsed
schedule is claimed because maintainer-focused time was not recorded.

Four defects were removed locally: one test-runner status leak, two conversion
algorithm defects found in personal code review, and one controlled-record
reference rejected by traceability. No fix minutes were recorded. No defect is
known to remain in the locally executable scope; native ARM64 and changed-CI
execution remain external evidence gates.

T2 is revised from 70–120 to **75–130 focused hours**, and from 1,200–2,400 to
**1,300–2,600 changed lines**. T1 confirms that controlled records, isolation,
aggregate integration, and target evidence consume roughly half the changed
surface even for two cohesive facilities. T2 has three conformance units plus
format-parser interaction, `va_list` paths, hexadecimal floating rounding, and
larger negative/boundary matrices. The modest increase preserves reuse credit
for T1's checked integer conversions, format-modifier decisions, runner pattern,
and ABI records rather than extrapolating chat or command wall time as effort.
