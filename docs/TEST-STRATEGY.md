# WCRT Test Strategy

**Content type:** Project test strategy

**Status:** Implemented baseline with identified deferred controls

**Source:** WSP test strategy at the pinned WSP baseline

## Scope

This strategy applies to verification used for WCRT requirement completion,
continuous integration, release readiness, and C89 conformance claims. It
covers project-owned source, generated libraries, supported architectures, and
WPM packages. Developer-only exploratory checks are not release evidence.

## Test Levels and Verification Methods

| Level | Method | Controlled artifacts |
| --- | --- | --- |
| Source quality | Automated inspection and Doxygen analysis | `REQ-0016`, `TC-0016` |
| Startup integration | Static link, PE inspection, and native execution | `REQ-0017`–`REQ-0018`, `TC-0017`–`TC-0018` |
| API presence | C89 compile-time verification without host-header fallback | `TC-0001`–`TC-0015`, `tests/c89/presence/` |
| Component behavior | Dynamic functional and boundary testing | `tests/c89/*.c`, PowerShell runners |
| C99 interface and isolation | Compile-time, behavioral, C89-isolation, and ABI testing | `TC-0019`, `TC-0021`–`TC-0026`, `tests/c99/` |
| Compiler and data model | Targeted compile-only capability and width assertions | `tests/c99/capabilities/`, `docs/c99-platform-model.md` |
| Library integration | DLL and static-library consumer builds and execution | `tools/test-built-libraries.ps1` |
| Legacy import compatibility | Repository-owned PE import parsing and Windows 2000 x86 allowlist | `tools/test-windows-2000-imports.ps1` |
| Compatibility comparison | Equivalent test input against Microsoft UCRT | `tools/run-c89-comparison.ps1` |
| Release verification | Release builds, consumer checks, package signing, and package verification | `.github/workflows/build.yml` |

The authoritative procedure and pass criteria for each controlled test are in
the corresponding `docs/tc-NNNN-*.tex` specification.

## Supported Verification Matrix

| Architecture | CI operating environment | Configuration | Toolchain |
| --- | --- | --- | --- |
| x86 | `windows-2025` | Debug and tagged Release | TinyCC WPM package |
| x64 | `windows-2025` | Debug and tagged Release | TinyCC WPM package |
| ARM64 | `windows-11-arm` | Debug and tagged Release | TinyCC WPM package |

TC-0016 is architecture independent and runs once on `windows-2025`. TC-0017
and TC-0018 apply to x86, x64, and ARM64; cross-link inspection is interim
evidence and does not replace required native execution. The comparison
workflow uses the locally available Microsoft compiler and UCRT and is
supporting evidence rather than the WCRT release gate.

## Environments and Preconditions

CI provisions TinyCC and cv2pdb from their signed WPM repositories. Test
runners receive the target architecture and compiler path explicitly. Outputs
are isolated below `output/` and build intermediates below `build/` or `tmp/`;
runners shall not reuse prior-run results as current evidence.

## Identifiers and Locations

- Requirements use `REQ-NNNN` and reside in `docs/req-NNNN-*.md`.
- Test cases use `TC-NNNN` and reside in `docs/tc-NNNN-*.tex`.
- Automated implementations reside in `tests/c89/run-tc-NNNN.ps1`.
- The controlled inventory is `tests/c89/manifest.md`.
- Machine-readable results reside below `output/test-results/`.
- Generated reports reside below `output/pdf/`.

## CI and Release Gates

Every change runs pinned WSP tool self-tests, traceability validation,
TC-0016, architecture-specific builds, consumer verification, the C89 test
suite, the complete controlled C99 aggregate, compiler/data-model probes, and
the Windows 2000 import gate for the x86 DLL. A required non-Pass result fails
its job. Compiler facilities planned for later tranches may be recorded as
unsupported without failing the current baseline; a facility required by an
implemented requirement fails the job when unavailable. Tagged releases additionally
build Release artifacts, combine all targets into one signed WPM development
package, verify that package, generate
SHA-256 checksums, and publish only after every architecture succeeds.
After all ordinary Debug architecture jobs pass, CI combines their outputs into
one unsigned `wcrt-debug` multi-architecture WPM package and retains it as a
workflow artifact.

## Evidence and Reporting

Architecture jobs retain JSON results, TeX test tables, binaries, symbols,
headers, and build artifacts in GitHub Actions. Job summaries expose the final
status of each test case. Project report tools can combine controlled
specifications and execution results into the C89 report.

The controlled C99 aggregate rejects drift between its inventory and
`tests/c99/manifest.md`. Per-target capability/data-model evidence is retained
as `tinycc-c99-capabilities.json`; x86 import evidence enumerates the inspected
DLL imports and binary digest in `windows-2000-imports.json`. Tagged x86
Release builds repeat and retain the import check for the release-configured
DLL.

Project test entry points import the PowerShell adapter from the pinned WSP
logging library through `tests/test-logging.ps1`. Human-readable progress and
outcomes use the common `[INFO ]`, `[PASS ]`, `[WARN ]`, and `[ERROR]` records.
Structured PowerShell objects, JSON, TeX, and process exit codes remain the
authoritative machine-readable evidence; logging shall not replace or alter
those interfaces.

Failure output from an execution must remain available in its original CI run.
A later passing run is separate evidence and does not reclassify the failure.

## Retention

CI evidence is retained for the repository-configured GitHub Actions artifact
and log retention period. Published release packages, checksums, and release
metadata are retained with the GitHub Release. A durable policy binding all
release test evidence to a defined retention period remains deferred in the
WSP adoption record.

## Known Exclusions

- Manual tests are not currently used as requirement-verification evidence.
- PAdES documentation signing is not selected.
- Full Authenticode and Defender release evidence remains deferred.
- Microsoft UCRT comparison results do not substitute for WCRT results.

## Responsibilities

WCRT maintainers own requirement and test approval, CI configuration, release
decisions, failure disposition, and updates to this strategy. Pull-request
reviewers verify that changed behavior has corresponding controlled
requirements, specifications, implementations, and evidence.
