# WPM TinyCC Compatibility Closure Work Record

**Content type:** Consolidated milestone plan, design, work log, review, and
closeout record

**Work package:** REQ-0050/REQ-0051 WPM compatibility closure

**Period:** 2026-08-25

**Starting baseline:** WCRT `79a3869`; WPM `ef2c4c5` and parent of `252b8b0`

**Owner:** WCRT maintainer

**Status:** Local implementation and review complete; native ARM64 and
independent review pending

**Approval:** Review and merge of this work package

## Milestone Plan

### Objective and Scope

Inventory WPM's retired TinyCC compatibility runtime, prove which interfaces
WCRT already owns, implement the remaining bounded runtime functionality, and
add controlled provenance, requirements, tests, build integration, and local
evidence.

Included work is the explicit POSIX selector, missing error names, time-type
header guard, typed `stat`/`utime` adapters, POSIX-facing structures, strict
isolation, x86/x64 native verification, ARM64 compile/link verification, DLL
exports, and static/DLL consumer links.

Excluded work is the broader 1.1.0 descriptor/path surface, full POSIX
conformance, `_POSIX_VERSION`, a VC6 consumer ABI, native Unix ownership and
inode semantics, and MSVCRT's private `__iob_func` bridge.

### Baseline and Assumptions

- Existing REQ-0017, REQ-0019, REQ-0029, and REQ-0045 through REQ-0049 remain
  the controlled owners of the WPM functions already integrated.
- Existing `_stat64` and `_utime64` behavior is the Windows operation baseline.
- `WCRT_POSIX` is available as a project-owned explicit feature selector.
- The local host can execute x86 and x64 Windows binaries and cross-compile,
  but cannot execute, ARM64 binaries.
- The WPM repository is an inspection source only; this work package writes
  only in WCRT.

### Deliverables and Allocation

| Deliverable | Allocation | Completion evidence |
| --- | --- | --- |
| Historical source and provenance matrix | Compatibility documentation | WPM inventory review |
| Feature-selection and adapter decision | ADR-0007 | Architecture review |
| POSIX selector and error foundation | REQ-0050 | TC-0050 |
| POSIX file status and time adapters | REQ-0051 | TC-0051 |
| Aggregate, traceability, build, and consumer integration | Test/build tooling | Local regression evidence |

The implementation-size forecast at scope freeze was 24--32 affected artifacts
and 800--1,400 changed lines. Focused effort was not instrumented and shall not
be inferred from chat or command wall time. Replanning was required if the
work exposed a descriptor-core dependency, a Microsoft ABI change, or more
than two additional POSIX function families; none occurred.

### Risks and Controls

| Risk | Impact | Control |
| --- | --- | --- |
| Macro aliases rewrite Microsoft code | Combined profiles become unsafe | Use typed adapters and distinct structures |
| POSIX names leak into ISO headers | C89/C99 contract changes | Positive and negative compile fixtures |
| WPM constants conflict with Windows ABI | Mixed-object error comparisons fail | Use Microsoft/TinyCC values and record the correction |
| Windows metadata is mistaken for Unix metadata | Consumers rely on false semantics | Specify and test every placeholder or substitute |
| New source is absent from release artifacts | Source tests pass but DLL consumers fail | Required export checks and static/DLL consumers |
| New imports break Windows 2000 x86 | Supported legacy target regresses | Run the existing PE import allowlist |

Rollback consists of removing the selected headers/source and the two new
controlled requirements while leaving existing Microsoft implementations
untouched. Generated results are isolated below `build/` and `output/`.

## Design Record

### Components and Interfaces

| Component | Responsibility | Dependency | Allocation |
| --- | --- | --- | --- |
| `<errno.h>`, `<time.h>` | Selector-gated errors and time interoperation | Existing C error/time ABI | REQ-0050 |
| `<sys/types.h>`, `<sys/stat.h>`, `<utime.h>` | POSIX-facing types and declarations | `WCRT_POSIX` | REQ-0051 |
| `posix_files.c` | Typed value adaptation | `_stat64`, `_utime64` | REQ-0051 |
| POSIX test manifest and runners | Isolation, behavior, and inventory control | TinyCC and WCRT sources | TC-0050, TC-0051 |
| Build and consumer tools | Artifact export and link verification | WCRT DLL/static package | REQ-0051 |

`stat` zero-initializes its result, obtains Microsoft metadata, then copies
every field into the separately declared POSIX-facing structure. It publishes
timespec and direct-seconds forms coherently. No pointer aliasing between the
two public structure types is used.

`utime` converts its two `time_t` members into the explicit-width Microsoft
structure and delegates the operation. A null `times` pointer passes through
to the current-time behavior. Both adapters use WCRT `errno` and allocate no
resources.

The selected profile reports seconds only. Inode, user, and group are zero;
link count is one; mode reports the available file type and owner bits; and
Windows creation time substitutes for POSIX metadata-change time. Invalid or
unrepresentable values fail without wrapping. The implementation adds no
Windows API call or trust boundary beyond the existing path operations.

ADR-0007 is the durable authority for selection, separate types, Windows error
values, metadata limits, and the absence of `_POSIX_VERSION`.

### Verification Allocation

| Requirement or risk | Method | Evidence |
| --- | --- | --- |
| Selected values and strict isolation | C89/C99 positive/negative compile | TC-0050 |
| Type widths, declarations, and include order | Compile-time assertions | TC-0051 |
| Metadata, times, ranges, and errors | Native filesystem behavior | TC-0051 |
| Missing build allocation or exports | Three-architecture builds and `.def` check | `build-wcrt.ps1` |
| Static/DLL consumer compatibility | Packaged-header link/run smoke tests | `test-built-libraries.ps1` |
| Legacy Windows imports | PE import analysis | `test-windows-2000-imports.ps1` |
| Traceability and source quality | Repository-owned automated checks | TC-0016 and traceability runner |

## Work and Defect Log

| Order | Phase | Activity | Output |
| --- | --- | --- | --- |
| 1 | Inspect | Located the renamed/deleted WPM history and classified every file | Compatibility inventory |
| 2 | Specify | Froze selector, provenance, requirements, tests, and exclusions | ADR-0007, REQ/TC-0050--0051 |
| 3 | Implement | Added headers, typed adapters, manifests, and runners | Source change |
| 4 | Integrate | Added aggregate, traceability, build, export, and consumer gates | Tooling change |
| 5 | Verify | Ran targeted, aggregate, build, consumer, quality, and import checks | Local evidence below |
| 6 | Review | Inspected contracts, ABI isolation, failures, documentation, and diff | Review summary below |

| ID | Type | Injected | Removed | Disposition |
| --- | --- | --- | --- | --- |
| WPM-D001 | Source inventory | Historical WPM shim | Requirements | Corrected `ENOSYS` from WPM's 38 to the Microsoft/TinyCC Windows value 40 |
| WPM-D002 | Test harness | Test implementation | First TC-0050 run | Added the selected TinyCC internal include root while retaining `-nostdinc` |
| WPM-D003 | Test harness | Aggregate integration | First aggregate run | Resolved compiler includes through `WCRT_TEST_TINYCC` when the diagnostic wrapper is active |

No fix effort was recorded.

The slice affected 28 artifacts with 1,261 added and three deleted lines,
including this record. It remained within the scope-freeze size forecast.

## Personal Review Record

**Reviewed baseline:** Current working diff and locally generated Debug
artifacts

**Reviewer:** Implementer personal review; independent pull-request review is
still required

| Area | Criterion | Result | Observation |
| --- | --- | --- | --- |
| Requirements | Every remaining shim has an owner or exclusion | Pass | Inventory maps all former callables and definitions |
| Standards | Origin and contract are cited accurately | Pass | ISO, Microsoft, POSIX, Windows, TinyCC, and WPM roles are distinct |
| Headers | Selected and strict profiles do not conflict | Pass | C89/C99 repeated-inclusion fixtures pass |
| ABI | Existing Microsoft layouts and names are unchanged | Pass | POSIX uses separate types/functions; x86 Microsoft tests pass |
| Failure behavior | Errors and ranges are deterministic | Pass | Null, missing, negative, and current-time partitions pass |
| Portability | Windows substitutions are visible | Pass | Placeholder identity, link count, times, and mode are documented/tested |
| Build/package | New functions reach static and DLL consumers | Pass | Exports and consumer links pass on all build targets |
| Security/imports | No new Windows dependency is introduced | Pass | x86 Windows 2000 import gate passes |
| Quality | Documentation, line length, and traceability pass | Pass | TC-0016 and traceability are green |

No unresolved personal-review finding remains. Native ARM64 execution,
independent review, CI execution, and release approval are outside this local
review's evidence boundary.

## Local Verification Evidence

- TC-0050 and TC-0051 pass natively with TinyCC 0.9.28rc on x86 and x64.
- The full 32-case C99/Microsoft/POSIX extension aggregate passes on x86 and
  x64, including manifest-to-runner inventory validation.
- The complete TC-0001--TC-0015 C89 aggregate passes on x86 and x64.
- ARM64 C89/C99 selected/strict fixtures compile, and the TC-0051 behavior
  executable links. It is not reported as native execution.
- Debug DLL, static library, headers, and startup objects build for x86, x64,
  and ARM64; every `.def` contains `stat` and `utime`.
- Static and DLL consumers using packaged POSIX headers link and run on x86 and
  x64; both link on ARM64 with runtime explicitly deferred.
- Traceability passes for 50 requirements, specifications, and automated
  implementations. TC-0016 scans 189 C/header files with zero violations.
- The fresh x86 DLL passes the Windows 2000 import allowlist with no unexpected
  imports.
- `git diff --check` passes.

## Closeout and Handoff

| Exit criterion | Evidence | Gate | Status |
| --- | --- | --- | --- |
| Provenance inventory and disposition complete | Inventory and ADR-0007 | Required | Pass |
| Requirements, tests, implementation, and traceability agree | REQ/TC-0050--0051 | Required | Pass |
| x86/x64 native behavior and regression | Aggregate records | Required | Pass |
| Three-architecture build and consumer link | Debug build/consumer output | Required | Pass |
| Native ARM64 behavior | `windows-11-arm` result | Release gate | Pending |
| Independent review and CI | Pull request and workflow evidence | Release gate | Pending |

The local implementation slice is complete, but the release-level work package
shall not be closed until native ARM64 and independent CI/review evidence pass.
No deferred objective is represented as a passing required gate.

The working tree already contained user-owned changes to `README.md`,
`ROADMAP.md`, `docs/PROJECT-PROCESS.md`, `docs/WSP-ADOPTION.md`, the WSP
submodule, and the new 1.1.0 roadmap. They were preserved and are not claimed
as outputs of this slice.

## References

- [WPM compatibility inventory](../wpm-tcc-compatibility-inventory.md)
- [ADR-0007](../adr-0007-posix-compatibility-selection.md)
- [REQ-0050](../req-0050-posix-selection-errno.md)
- [REQ-0051](../req-0051-posix-stat-utime.md)
- [Project test strategy](../TEST-STRATEGY.md)
