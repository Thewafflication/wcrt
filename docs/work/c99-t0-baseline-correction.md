# C99 T0 Baseline Correction Work Log

**Status:** Local implementation and personal review complete; CI/native ARM64 pending

**Owner:** WCRT maintainer

**Created:** 2026-08-04

## Baseline and Scope

Reconcile REQ-0021 through REQ-0026 across their requirement records, public
headers, controlled specifications, test sources, runners, manifest, aggregate
runner, and CI. Close the discovered C89-isolation gaps; integrate every test
controlled by `tests/c99/manifest.md` into aggregate results and enforcement;
add target-specific TinyCC capability and data-model probes; document the x86,
x64, and ARM64 models; and add an enforced Windows 2000 x86 DLL import check
with retained machine-readable evidence.

Runtime C99 work beyond REQ-0026, new C99 library interfaces, release signing,
publication, and target claims that cannot be verified in the available local
environment are excluded.

**Controlled size:** One process correction, six requirement reconciliations,
six aggregate test integrations, six compiler-facility probes, three target
data-model checks, and one import gate.

**Estimated change size after repository inspection:** 18–28 artifacts and
450–850 added or modified lines. The original 150–350-line estimate omitted
requirement-record corrections, missing C89-isolation sources, probe sources,
machine-readable evidence plumbing, the import allowlist/gate, and CI summary
and retention changes.

**Forecast after repository inspection:** 21–36 focused hours. No completion
date is committed because available weekly capacity is not recorded and the
task environment does not provide a trustworthy focused-effort clock.

| Phase | Planned effort |
| --- | ---: |
| Plan | 3–5 h |
| Specify and design | 4–6 h |
| Implement | 7–12 h |
| Personal review | 2–4 h |
| Verify | 4–7 h |
| Postmortem | 1–2 h |

## Repository Reconciliation

- ROADMAP marks REQ-0021 through REQ-0026 complete, and headers, controlled
  specifications, individual runners, and manifest rows exist for all six.
- REQ-0021, REQ-0022, REQ-0023, REQ-0024, and REQ-0026 still state
  `Approved; implementation planned`; only REQ-0025 contains an implementation
  record. These records disagree with the roadmap and manifest.
- `tools/run-extension-tests.ps1` runs TC-0019 and Microsoft-compatibility
  tests, but omits TC-0021 through TC-0026. CI invokes and enforces that
  incomplete aggregate, summarizes its JSON, and retains the same incomplete
  JSON on x86, x64, and ARM64.
- TC-0021, TC-0022, TC-0023, TC-0024, and TC-0026 specify C89 isolation, but
  their runners provide no C89-isolation source. TC-0025 checks `restrict`
  isolation but does not check `va_copy` isolation.
- TC-0024 describes decimal round trips and observed rounding/evaluation
  behavior, while its behavioral source currently checks only macro ranges and
  epsilon addition. Any unclosed portion must remain explicit rather than be
  represented as verified.
- No repository-owned TinyCC capability runner, x86/x64/ARM64 data-model
  record, or automated Windows 2000 x86 import gate exists.
- The T5 work-plan row omitted its effort value, shifting the remaining cells;
  the published 506–868-hour program total consequently excluded all T5
  effort. T1 also omitted the demonstrated per-tranche control/evidence cost.
- The local sibling TinyCC packages identify version `0.9.28rc` for i386,
  x86_64, and AArch64 Windows. All three compilers and existing Release DLLs
  are locally available; native execution availability remains architecture
  dependent.
- CI installs the latest TinyCC prerelease rather than a pinned package. T0
  records the exact compiler identity and re-probes it, but publishing and
  pinning the supported-version set remains an open release dependency.

## Dependencies, Risks, and Responses

- **TinyCC variation:** capability results may differ by packaged target or
  future compiler baseline. Record compiler identity and each facility result
  per architecture; enforce facilities already required by REQ-0021–REQ-0026
  and classify future complex/type-generic gaps without fabricating support.
- **Cross-target execution:** an x64 developer host cannot execute ARM64 test
  binaries. Use compile-time ABI assertions locally and retain native ARM64 CI
  execution as a required target-only check.
- **C89 leakage:** TinyCC may accept later-language tokens as extensions even
  under `-std=c89`. Isolation tests therefore verify header exposure directly,
  while the full C89 suite remains the regression gate.
- **Import parsing/tool availability:** depending on `dumpbin` would add an
  unpinned Visual Studio discovery dependency. Prefer a repository-owned PE
  import parser and an explicit Windows 2000 allowlist, and test its positive
  and negative behavior.
- **Allowlist drift:** an incomplete allowlist can reject valid baseline APIs;
  an overbroad list can admit newer APIs. Keep the allowlist narrow, document
  its scope, retain the enumerated imports, and require review for additions.
- **Stale evidence:** existing files below `output/` are not current evidence.
  Regenerate results with explicit compiler paths and preserve CI artifacts;
  identify local-only and target-only results separately.
- **Requirement overstatement:** update a requirement to `Implemented` only
  when its implementation record and controlled verification match the actual
  repository state; document remaining analysis or target evidence explicitly.

## Completion Criteria

- REQ-0021 through REQ-0026 status and implementation records agree with their
  headers, specifications, manifest, and verified test coverage.
- The aggregate runner fails on any controlled C99 failure and fails if its
  C99 inventory differs from `tests/c99/manifest.md`.
- Aggregate JSON and CI summaries contain TC-0019 and TC-0021 through TC-0026
  on each supported architecture, and CI retains the complete results.
- `_Bool`, `long long`, `restrict`, variadic macros, complex arithmetic, and
  type-generic macro capability results are recorded per compiler target.
- Compile-time data-model checks and documentation agree for x86, x64, and
  ARM64, including pointer, integer, `wchar_t`, and floating widths.
- The x86 Debug DLL import table is checked against the Windows 2000 allowlist;
  the enumerated imports and gate result are retained and enforced by CI.
- C89 regression and isolation, source quality, traceability, aggregate C99
  and compatibility tests, supported-target ABI behavior, and evidence-format
  checks are green where locally available; native/CI-only checks are listed.
- Personal review findings are dispositioned, actual measures are recorded
  only when observed, and the postmortem does not claim unavailable evidence.

## Quality Plan

- Review requirement completeness and C99 traceability before claiming status.
- Add positive C99 checks and direct C89 header-exposure checks; run the full
  earlier C89 suite to detect edition leakage.
- Make the aggregate inventory self-check against the controlled manifest and
  inspect its JSON for uniqueness, completeness, and enforced failures.
- Compile all capability and data-model probes with `-Wall -Werror` for x86,
  x64, and ARM64; execute behavioral tests natively where supported.
- Unit-test PE parsing/allowlist rejection with a controlled unexpected import,
  then inspect the freshly built x86 DLL and retain its import list as JSON.
- Run WSP tool tests, traceability, source-quality checks, build/consumer and
  startup checks, C89, C99, and compatibility suites as locally available.
- Perform the project checklist for requirements, design, code, tests,
  documentation, ABI, C89 isolation, Windows 2000 compatibility, warnings,
  dependencies, and retained evidence; review `git diff --check` and the final
  patch before handoff.

## Exact Implementation Order

1. Correct requirement/status and test-specification statements so the
   intended REQ-0021–REQ-0026 baseline and evidence limits are explicit.
2. Add the missing C89-isolation sources and connect them to TC-0021–TC-0026;
   close only behavioral gaps required to support reconciled status.
3. Add TC-0021–TC-0026 to the aggregate and make the aggregate inventory check
   itself against the C99 manifest before executing tests.
4. Add the six TinyCC facility probes and the architecture data-model
   compile-time assertion, then emit one target-specific JSON capability record.
5. Publish the data-model/capability interpretation and supported/blocked
   distinctions without extending the C99 runtime surface.
6. Add the repository-owned PE import reader, Windows 2000 x86 allowlist gate,
   negative self-test, and retained JSON result.
7. Wire capability and import results into CI execution, summaries,
   enforcement, and architecture artifacts.
8. Run strongest local validation, perform the personal review checklist,
   disposition defects, and update actual size/effort/evidence/postmortem.

## Time Log

| Date | Phase | Focused minutes | Excluded interruption | Note |
| --- | --- | ---: | --- | --- |

No focused minutes are entered from chat or command wall time; neither is a
reliable measure of maintainer-focused effort. The maintainer may add observed
focused time later.

## Defect Log

| ID | Type | Injected | Removed | Fix minutes | Related artifact | Disposition |
| --- | --- | --- | --- | ---: | --- | --- |
| T0-D001 | interface/ABI | Implementation | Implementation | — | C99 edition guards in six public headers | Corrected after direct C89-isolation compilation showed TinyCC defines a later `__STDC_VERSION__` under the project C89 invocation. Focused fix time was not recorded. |
| T0-D002 | documentation | Planning | Planning | — | `docs/C99-1.0-WORK-PLAN.md` | Added the missing conditional T5 estimate, corrected the program total, and revised T0/T1 from repository evidence. Focused fix time was not recorded. |
| T0-D003 | test | Implementation | Personal review | — | `tests/c99/va-copy.c` | Wrapped an 81-character source line reported by TC-0016. Focused fix time was not recorded. |
| T0-D004 | build/tooling | Existing test design | Verify | — | C89 runner build directories | Parallel local x86/x64 C89 execution collided in the shared TC-0008 executable path and produced `Permission denied`. CI uses separate target hosts; T0 validation is rerun serially, and architecture-isolated C89 intermediates are deferred as a later tooling improvement. |

## Review Checklist

- [x] Requirement and manifest status agree with repository evidence.
- [x] Every controlled C99 test appears in aggregate results.
- [x] Capability probes cover `_Bool`, `long long`, `restrict`, variadic
      macros, complex arithmetic, and type-generic macros.
- [x] C89-mode isolation remains green on locally executable targets; native
      ARM64 execution is listed below rather than inferred from cross-builds.
- [x] ABI/data-model compile-time results cover x86, x64, and ARM64.
- [x] Windows 2000 x86 imports are automatically inspected, negatively
      controlled, retained with a DLL digest, and enforced for Debug and tagged
      Release builds.
- [x] Documentation, CI summaries, enforcement, and retained-evidence paths
      agree by personal review; execution of the changed workflow is pending.
- [x] Header self-containment, repeated inclusion, expression properties,
      direct C89 exposure, and earlier C89 regressions were reviewed.
- [x] The numeric model, rounding claim, Windows `long double`, and the boundary
      between analyzed representation and later conversion tests are explicit.
- [x] Source warnings/quality, dependency identity, PowerShell syntax,
      traceability, JSON formats, final patch whitespace, and generated evidence
      were reviewed. A local YAML/actionlint implementation was unavailable.

## Local Verification Evidence

- Pinned WSP common-tool suite: 15 checks passed, including positive and
  negative traceability, evidence, source-quality, checksum, and documentation
  controls.
- Traceability: 33 requirements, 33 controlled test specifications, and 33
  implementations passed.
- TC-0016: 128 C/header files scanned with zero violations after T0-D003.
- C89 aggregate: TC-0001 through TC-0015 passed serially on x86 and x64.
- Extension aggregate: seven controlled C99 cases (TC-0019 and TC-0021 through
  TC-0026) and eight Microsoft-compatibility cases passed on x86 and x64; each
  JSON result contains the requirement ID and no failing status.
- Capability/data model: x86 ILP32 and x64/ARM64 LLP64 assertions passed with
  TinyCC 0.9.28rc. `_Bool`, `long long`, `restrict`, variadic macros, and
  `_Generic` selection compiled on all targets; `_Complex` was consistently
  recorded as unsupported.
- Fresh Debug builds: DLL, PDB, static library, headers, and startup objects
  built for x86, x64, and ARM64. Fresh Release builds also succeeded for all
  three targets.
- Consumers and startup: x86/x64 static and DLL consumers executed and both
  startup cases passed in available configurations. ARM64 consumers linked and
  startup images passed PE/link inspection; execution was reported `Deferred`
  or `Blocked` on the x64 host.
- Windows 2000 import gate: the fresh x86 Debug DLL passed with 27 allowlisted
  `kernel32.dll` imports and a retained SHA-256 digest. The repository parser's
  27 Release imports exactly matched independent `dumpbin` output; its built-in
  `GetTickCount64` negative control was rejected.
- PowerShell parsing succeeded for all changed runners; allowlist and generated
  JSON parsed; `git diff --check` passed. No local YAML/actionlint parser was
  available, so GitHub workflow parsing remains a CI check.

## Remaining Target-Only and External Checks

- Run the changed GitHub Actions workflow so GitHub parses the YAML and each
  architecture job demonstrates summary rendering, artifact retention, and
  enforcement behavior.
- Execute C89, the complete C99/compatibility aggregate, consumers, and startup
  tests on the native `windows-11-arm` ARM64 runner. Local ARM64 evidence is
  compile/link/PE-only and is not represented as native execution.
- Confirm x86/x64 results with the exact TinyCC prerelease installed by the CI
  run; CI currently follows the latest prerelease rather than a pinned package.
- Exercise the tagged x86 Release import step when an actual release candidate
  exists. No tag or release artifact was created during T0.
- Pull-request review remains the independent review step. No commit, push, or
  external publication was performed.

## Postmortem

**Local closeout:** T0 produced 54 changed artifacts with 1,121 additions and
109 deletions (1,230 changed
lines). This exceeded the inspected 18–28-artifact, 450–850-line estimate
because the repository-owned PE parser, CI summary/retention path, requirement
implementation records, direct isolation cases, and WSP closeout record were
larger than allowed for in the refined baseline. The functional scope did not
expand beyond T0.

No actual focused effort or elapsed schedule is reported. Command wall time
and chat duration are not substitutes for maintainer-focused phase time. Four
defects were observed: three corrected in T0 and the architecture-shared local
C89 intermediate-path issue deferred because CI targets use separate hosts.
No fix minutes were available and none were inferred.

T1 is revised from 40–70 to **48–85 focused hours**. Its size range remains
700–1,300 implementation LOC, but planning must also allow for requirement and
test records, explicit C89 guards, target evidence, aggregate/CI retention,
and postmortem work. T1 shall stage `wint_t`-dependent `stdint.h` macros with
the wide-ABI tranche rather than silently claim them early.

Selected process improvements are to reuse manifest-to-aggregate inventory
checks for later suites, isolate local test intermediates by architecture
before parallel target execution, keep future capability gaps distinct from
current-baseline failures, and require program-total arithmetic review when a
tranche estimate changes. Final T0 closure awaits the target-only/external
checks above.
