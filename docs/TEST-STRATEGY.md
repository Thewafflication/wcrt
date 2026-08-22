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
| C99 interface and isolation | Compile-time, behavioral, C89-isolation, numeric-boundary, and ABI testing | `TC-0019`, `TC-0021`–`TC-0033`, `TC-0035`–`TC-0042`, `tests/c99/` |
| Compiler and data model | Targeted compile-only capability and width assertions | `tests/c99/capabilities/`, `docs/c99-platform-model.md` |
| Library integration | DLL and static-library consumer builds and execution | `tools/test-built-libraries.ps1` |
| Legacy import compatibility | Repository-owned PE import parsing and Windows 2000 x86 allowlist | `tools/test-windows-2000-imports.ps1` |
| Compatibility comparison | Equivalent test input against Microsoft UCRT | `tools/run-c89-comparison.ps1` |
| Release verification | Release builds, consumer checks, WPM package signing/verification, and packaged-DLL identity comparison | `.github/workflows/build.yml`; package verification tools |

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

The C89, C99, and Microsoft-compatibility aggregates route ordinary TinyCC
test-executable links through `tests/tcc-diagnostic-wrapper.cmd`. The wrapper
adds `-g -bt30`, so a hardware exception emits TinyCC's source-level backtrace
at the original failure. Compile-only, shared-library, relocatable, archive,
`-run`, import-definition, and custom `-nostdlib` links are not changed.
`tests/verify-native-test-diagnostics.ps1` deliberately faults a probe on each
architecture and requires the in-process trace to identify both the crashing
function and its caller.

After a runner exception, the aggregate retains emitted output and the
executable, records its SHA-256 digest, and attempts a 30-second batch GDB
rerun when GDB is available. The GDB result is independently classified as
Completed, UnsupportedTarget, NoBacktrace, TimedOut, Failed, or Unavailable.
`UnsupportedTarget` means the installed debugger cannot read that target's PE
format and is not a debugger Pass. cv2pdb remains the converter used for the
WCRT DLL PDB; it is not required for TinyCC's embedded test trace or GDB's
reading of the test executable. Logical failures that do not raise an
exception retain their test diagnostics but may have no native stack to
report.

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
its job except for an explicitly controlled ExpectedFail release deviation.
ADR-0005 permits TC-0037 and TC-0038 to use ExpectedFail only for TinyCC's
exact, source-specific complex-type/parser and imaginary-literal diagnostic
matrix. That state does not stop remaining tests, builds, or WPM packaging,
but it is not Pass or conformance evidence. Any other diagnostic or behavioral
failure remains fatal. The workflow selects the latest eligible `tinycc` WPM
package without a version selector, records its exact identity, and requires
the same selected version across x86/x64/ARM64 and Debug/Release. The retained
`0.9.28-rc.1442+2474e1c2` baseline passes both probes, so TC-0037 and TC-0038
remain ordinary required Pass results for every newly selected package. A candidate
source revision may be committed before the tagged Release matrix runs; that
commit is neither release approval nor publication evidence. Tagged releases
additionally build optimized Release artifacts and run native library-consumer
and startup smoke tests on every target, repeat the x86 legacy-import check,
combine all targets into one WPM-signed package, require `wpm verify` exit zero,
compare the packaged DLLs with their Release inputs, and generate SHA-256
identities. The full C89/C99/compatibility inventory is not duplicated under
Release because each tagged Release job depends on the successful exact-source
Debug matrix. The publication job depends on successful WPM package
verification, which depends directly on every Release architecture succeeding.
Missing target or WPM verification evidence is a failed job, not Pass.
Authenticode signing/timestamping and Defender scanning are explicitly Deferred
from 1.0.0 and are not release Pass results.
After all ordinary Debug architecture jobs pass, CI combines their outputs into
one unsigned `wcrt-debug` multi-architecture WPM package and retains it as a
workflow artifact.

The integration consumer constructs nonconstant complex operands and executes
multiplication and division. It resolves DLL complex functions by exact export
name at runtime so TinyCC builtins cannot turn the DLL test into static or
compiler-only evidence. ARM64 DLL consumers link `wcrt.def` plus the packaged
`libwcrt-tinycc-complex-abi.a`; the companion archive supplies only the
selected compiler's private operator bridge, while the standard functions
must still resolve from `wcrt.dll`.

## Evidence and Reporting

Architecture jobs retain JSON results, TeX test tables, binaries, symbols,
headers, and build artifacts in GitHub Actions. Job summaries expose the final
status of each test case. Project report tools can combine controlled
specifications and execution results into the C89 report.

Failure artifacts below `output/test-results/<architecture>/diagnostics/`
retain the test transcript, captured process output, exact executable and
digest, GDB output and status, and a machine-readable diagnostic record. The
native diagnostic self-test record is retained even when all product tests
pass. A missing debugger is Unavailable, not evidence that the GDB rerun
passed; the separately verified TinyCC in-process trace remains available.

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
- Authenticode signing/timestamping and Defender release scanning are Deferred
  from WCRT 1.0.0. Existing DLLs remain `NotSigned`; no Pass is inferred.
- Microsoft UCRT comparison results do not substitute for WCRT results.

## Responsibilities

WCRT maintainers own requirement and test approval, CI configuration, release
decisions, failure disposition, and updates to this strategy. Pull-request
reviewers verify that changed behavior has corresponding controlled
requirements, specifications, implementations, and evidence.
