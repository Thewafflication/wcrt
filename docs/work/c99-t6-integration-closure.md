# C99 T6 Integration Closure Work Log

**Status:** Planning baseline proposed; implementation No-Go pending a clean
T5 source baseline and disposition of inherited completion gates

**Owner:** WCRT maintainer

**Created:** 2026-08-15

**Standard authority:** [WG14 N1256](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf),
the public C99 plus TC1, TC2, and TC3 committee draft

**Inspected repository revision:**
`00d3f45e3f58d7332b2f5249a4596017aaff76b8` (`Baseline C99 T5 complex
and generic math`) plus the uncommitted T5 implementation working tree

**Immutable implementation baseline:** Not yet available. The inspected
working tree contains modified and untracked T5 implementation, test, build,
and documentation artifacts and therefore cannot identify exact T6 inputs.

## Tranche Decision

This record proposes the baseline for T6 integration closure. It does not
authorize implementation. T6 implementation is No-Go until the configuration
checkpoint in the exact implementation order establishes one reviewed source
revision and explicitly disposes the inherited gates below.

Planning and requirements work may continue without representing any current
facility or result as a T6 Pass. A later approval to execute T6 shall not by
itself authorize tagging, signing with private keys, pushing, publishing,
uploading, or creating a release.

## Baseline and Scope

T6 closes four controlled C99 conformance units:

- the remaining narrow, wide, locale, floating-environment, accuracy, and
  cross-interface allocations of REQ-0039;
- REQ-0040, the C99 audit of every existing C89 header and runtime family;
- REQ-0041, the clause-level C99 hosted-library profile; and
- REQ-0042, the cross-edition, cross-architecture compatibility and evidence
  baseline.

The audit covers all 24 C99 standard headers, including the 15 inherited C89
families and the nine C95/C99-added header families. It covers library clauses
7.1 through 7.25, the applicable implementation-defined and locale-specific
items, optional IEC 60559/imaginary facilities, and the compiler-language
facilities on which the public library depends. It does not claim complete C99
compiler-language conformance.

The following are completion work rather than supporting observations:

- controlled requirements, specifications, manifest entries, runners, and
  direct source/evidence traceability;
- each header in isolation and in repeated/mixed include orders;
- macro values, integer constant-expression usability, exact expression types,
  public typedef/layout/alignment, declaration qualifiers, and function ABI;
- C89 absence or C89-safe declarations for every C99-only facility;
- behavior, boundaries, error/range/rounding/locale/stream state, and retained
  input or output for every audited delta;
- native x86, x64, and ARM64 Debug verification at one exact source and
  dependency baseline;
- Release builds, package assembly and verification, static and DLL consumers,
  startup objects, x86 Windows 2000 imports, source quality, traceability, and
  retained machine-readable evidence; and
- personal and independent review, defect disposition, actual measures, and
  the terminal postmortem.

### Exclusions

- REQ-0034 optional `wWinMain` startup remains outside the 1.0.0 prerequisite
  unless separately baselined without delaying closure.
- C11 and later library facilities and the remaining Microsoft CRT roadmap are
  excluded, while already controlled Microsoft-compatibility tests remain
  mandatory regressions.
- R1 release signing, Defender scanning, release approval, tag creation,
  publication, and upload are separate work. T6 produces inputs to R1 but does
  not approve or publish a release.
- Work in the TinyCC, cv2pdb, WPM, or WSP repositories is dependency work and
  requires its own scope and authority.

## Repository Reconciliation and Discrepancies

1. The work-plan T6 placeholder of 800--1,600 lines and 80--140 hours counts
   four identifiers but not the inspected audit surface: 24 headers, 15
   inherited runtime families, four controlled specifications, the clause
   profile, direct correction tests, three architectures, two build
   configurations, package/consumer evidence, and predecessor reconciliation.
2. `HEAD` contains only the T5 planning commit. The T5 implementation spans a
   modified/untracked working tree, and its work log states that native ARM64
   execution, the T4 `fma` disposition, independent review, and the T5
   postmortem remain open. T6 therefore has no immutable entry baseline.
3. T0 through T3 work logs retain native-target, exact-revision, retained-CI,
   or independent-review gates. Later CI may provide regression evidence, but
   those records have not been reconciled and absence of a recorded failure is
   not closure.
4. REQ-0040 through REQ-0042 have no project requirement records, test
   specifications, manifest entries, runners, results, or traceability rows.
   REQ-0039 expressly leaves decimal accuracy, locale, environment, and final
   clause integration open.
5. The repository has no C99 conformance profile, implementation-defined
   behavior record, or C89/C99 deviation record even though `README.md`,
   `ROADMAP.md`, and `docs/REQUIREMENTS.md` depend on them.
6. The current C99 aggregate contains the tranche tests through TC-0039 and
   the compatibility tests, but there is no exhaustive 24-header closure
   inventory or TC-0040 through TC-0042 result.
7. The existing `<assert.h>` path supplies expression text, file, and line but
   not the C99 enclosing-function name required by 7.2.1.1.
8. The narrow `<ctype.h>` surface and runtime omit the required C99 `isblank`
   facility, although the wide classification suite contains a private narrow
   reference helper.
9. `struct lconv` omits the six C99 international positive/negative placement
   and sign-position members. The C-locale initializer and tests therefore
   cover only the earlier structure.
10. `MB_CUR_MAX` expands to an `int` expression, while C99 7.20 requires a
    positive expression of type `size_t`. Existing tests check presence and
    value but not the exact expression type.
11. The decimal conversion path retains only 19 significant digits and scales
    through binary64 arithmetic. Existing tests do not establish the complete
    implementation-defined accuracy statement or the recommended-practice
    partitions for decimal strings. The final requirement must distinguish
    mandatory hexadecimal correct rounding from recommended decimal behavior
    and any stronger project-selected claim.
12. T4 records `fma`, `fmaf`, and `fmal` as an unapproved deviation because
    their single-rounding contract is unproven. T6 cannot convert this unknown
    required result into Pass through profile wording.
13. Compiler-dependent standard pragmas and optional imaginary/IEC 60559
    facilities are not completely probed or classified. Library function
    presence does not prove compiler pragma semantics.
14. Local x86/x64 C89 and extension JSON currently pass on the dirty T5 tree,
    and a dirty x86 Release DLL has passing Windows 2000 import evidence.
    Current ARM64 C89/extension/startup JSON is absent locally. These are
    developer results, not exact-revision retained T6 evidence.
15. `docs/arm64-support.md` still describes C89 cross-compilation and deferred
    native CI even though the workflow now uses a native Windows ARM runner and
    T4 cites native ARM64 evidence. The supported-target description is stale.
16. CI installs `latest` WPM, TinyCC, and cv2pdb inputs. The T5 records identify
    TinyCC `0.9.28-rc.1441+0af32d51`, but the workflow does not yet pin an exact
    dependency set. A later passing run must retain the resolved identities and
    cannot establish reproducibility from `latest` URLs alone.
17. Generated local packages are explicitly `.dirty`, have multiple identities,
    and are not release candidates. Release-readiness, DFS, signing/timestamp,
    Defender, rollback, and support records remain absent or deferred and
    belong to R1 after T6 reaches a terminal decision.

## Size and Effort Estimate

**Controlled size:** four conformance units; 24 standard headers; 15 inherited
C89 runtime families; the cross-cutting clause, ABI, edition, import, build,
package, consumer, traceability, source-quality, and evidence gates on x86,
x64, and ARM64.

**Estimated change size after inspection:** 38--62 source-controlled artifacts
and approximately 4,800--9,600 added or modified lines across requirements,
test specifications, profile/deviation/platform documentation, inventory data,
test sources and runners, manifests, implementation corrections, build/package
integration, and retained-evidence support. Product corrections are expected
to account for about 100--1,800 of those lines; audit results may change that
range before implementation is approved.

| Phase | Planned focused hours |
| --- | ---: |
| Plan and estimate | 8--14 |
| Specify/review requirements, clauses, and tests | 32--54 |
| Design and design review | 20--36 |
| Implement in-scope corrections and evidence tooling | 48--96 |
| Personal code and test review | 24--44 |
| Verify and retain exact-target evidence | 36--68 |
| Postmortem and re-estimate | 6--10 |
| **Base total** | **174--322** |

The completion forecast applies a 25--40 percent discovery reserve because
the audit is intended to find omissions rather than confirm a closed list. The
result is **218--451 focused hours**. This reserve is not a substitute for
logging defects or revising the estimate when the clause inventory is
baselined.

Unfinished T1--T5 corrective work is not hidden in this estimate. If the
maintainer chooses to absorb predecessor closure into the same execution
period, allow a separate provisional 30--90 focused hours, dominated by the
unbounded `fma` correction/evidence decision and native/review reruns. That
range shall be charged to the tranche in which the defect is removed.

No calendar date is forecast. Earlier work logs do not contain comparable
focused-effort actuals, so commit timestamps, command duration, and CI wall
time cannot be converted into personal velocity.

## Dependencies and Entry Gates

1. Preserve and review the intended T5 working tree, then establish a clean
   full source revision. Do not use a broad commit to conceal unrelated or
   unreviewed changes.
2. Reconcile T0--T5 requirements, work logs, defects, and exact evidence.
   Close each open gate or carry it as an explicit approved exception; a later
   regression Pass alone does not rewrite tranche history.
3. Dispose T4-D012 by a conforming correction with evidence or by an approved
   deviation that narrows the 1.0.0 profile claim.
4. Obtain native ARM64 T5 behavior, ABI, build, consumer, and startup evidence
   on the exact entry revision. Cross-link evidence is insufficient.
5. Record the exact TinyCC, cv2pdb, WPM, WSP, runner image, and applicable
   reference-data identities used for the T6 baseline.
6. Review and baseline REQ-0039 final scope and REQ/TC-0040 through -0042
   before representing any implementation correction as conforming.

## Classification Model

The clause profile shall not use one ambiguous status column. Each row shall
record:

- clause and facility;
- normative class: required, conditional/optional, recommended practice,
  implementation-defined, unspecified, or out of the hosted-library profile;
- implementation disposition: implemented, omitted optional, compiler-blocked,
  deviating, or not applicable;
- evidence status by target: Pass, Fail, Unknown, ExpectedFail only where an
  approved record permits it, or N/A with rationale;
- direct project requirement, specification, implementation, test result, and
  documentation references; and
- exception owner, approval, and completion condition when non-Pass.

The user-facing summary values `Required-Implemented`, `Optional`,
`Implementation-Defined`, `Compiler-Blocked`, and `Deviation` shall be derived
from those columns. `Implementation-Defined` is not an exemption from
implementation or documentation. Unknown required evidence remains Unknown
and blocks T6.

## Risks and Responses

- **Audit discovery and scope growth -- high:** baseline the clause/facility
  inventory before correcting code; update size and effort from the controlled
  finding count rather than consuming reserve silently.
- **Numeric accuracy and `fma` -- high:** separate mandatory C99 behavior,
  implementation-defined accuracy, recommended practice, and optional IEC
  60559 claims. Use exact bit/vector evidence and never generalize sampled
  success into universal correct rounding.
- **Compiler/library boundary -- high:** probe `_Complex`, `_Generic`,
  `FENV_ACCESS`, `FP_CONTRACT`, `CX_LIMITED_RANGE`, imaginary support, and
  predefined conformance macros per target/compiler. Classify compiler-owned
  gaps explicitly.
- **C89 surface regression -- high:** compile every C89 header and runtime
  family with the explicit WCRT edition selector, check absence of later names,
  and run all C89 behavior after each public-header correction.
- **ABI drift -- high:** exact-size/type assertions are insufficient for
  runtime calling convention. Require static/DLL consumers and native tests for
  structure layout, complex values, varargs, `jmp_buf`, `fenv_t`, `mbstate_t`,
  time and wide types on all targets.
- **ARM64 evidence gaps -- high:** do not infer execution, floating environment,
  branch cuts, signal/setjmp, or startup behavior from cross-compilation.
- **Legacy import regression -- high:** rerun repository-owned PE import
  parsing after every runtime/platform correction and bind the report to the
  exact x86 DLL digest.
- **Dependency drift -- high:** resolve and retain exact package versions and
  digests before the verification baseline; a `latest` URL is discovery input,
  not provenance.
- **Generated evidence contamination -- medium:** use clean architecture- and
  configuration-specific output roots; do not reuse the known shared C89
  intermediate paths in parallel.
- **Profile overclaim -- high:** every Pass requires a direct artifact and
  matching target result. Missing evidence, stale documentation, or a dirty
  package is not Pass.

## Completion Criteria

- REQ-0039 final scope and REQ-0040 through REQ-0042 are reviewed and baselined
  with controlled TC-0039 through TC-0042 specifications before correction
  implementation is represented as conforming.
- The controlled clause inventory covers library clauses 7.1--7.25, every C99
  standard header/facility, applicable Annex F/compiler dependencies, and every
  C89-header delta with no unclassified row.
- Every required facility is implemented and has direct requirement,
  implementation, test, and target-result references, or it is explicitly
  Fail/Unknown and blocks closure. Optional, implementation-defined,
  compiler-blocked, and deviating rows have exact rationale and approval state.
- Header self-containment, repeated/mixed inclusion, macro values and types,
  constant-expression use, declaration qualifiers, and C89 isolation pass for
  every applicable public header on x86, x64, and ARM64.
- All C89, C99, startup, and Microsoft-compatibility tests pass natively on
  x86, x64, and ARM64 at one exact source/dependency revision. No native result
  is inferred from a cross-build.
- Debug and Release DLL/static builds, package assembly/verification, static
  and DLL consumers, startup objects, ABI checks, and the x86 Windows 2000
  import allowlist pass and retain exact artifact digests.
- Traceability, WSP common-tool self-tests, TC-0016/source warnings, Doxygen,
  dependency metadata, evidence validation, and `git diff --check` pass.
- All findings are recorded before correction, affected checks are rerun, and
  open defects/deviations/risks are reflected identically in the profile,
  requirements, work log, roadmap, platform documentation, and R1 inputs.
- Personal design/code/test review and independent review are complete; actual
  size, focused effort, defects, evidence, and the postmortem are recorded.
- T6 ends with a gate-by-gate Pass/Fail/Unknown/N/A decision. Any required Fail
  or Unknown makes the tranche decision Reject/No-Go for R1 approval.

## Quality Plan

- Use N1256 as the clause authority and keep normative requirements separate
  from notes, examples, recommended practice, informative annexes, and project
  extensions.
- Build a machine-checkable facility inventory first, then generate or verify
  manifest/profile completeness from that inventory to prevent hand-maintained
  row drift.
- Review requirements and tests before code. For each finding, add the defect
  row and a failing or inspection-based reproducer before correction when safe.
- Partition reviews by base types/macros, character/locale, integer/numeric
  conversion, streams/formatting, strings/multibyte/wide, time, math/fenv,
  complex/tgmath, signals/setjmp, and build/platform integration.
- Use compile-time type-compatibility and constant-expression checks plus
  runtime boundary/state tests. Presence or link success alone is insufficient.
- Run focused tests immediately after each correction, then C89/C99/compatibility
  aggregates serially per architecture before full build/package consumers.
- Retain clean JSON/TeX/log evidence with source revision, dependency identity,
  runner/architecture/configuration, command, start/end time, result, and
  artifact digest. Failure evidence remains alongside later reruns.
- Perform a personal design/code/test review before formal verification and an
  independent evidence challenge after the candidate T6 baseline is immutable.

## Exact Implementation Order

1. **Configuration checkpoint.** Inventory the dirty tree; review and commit
   only intended T5 work; establish a clean full source revision; capture the
   WSP gitlink and exact resolved dependency identities.
2. **Predecessor disposition.** Reconcile T0--T5 work logs and evidence, obtain
   native ARM64 T5 results, dispose T4-D012, complete required reviews and
   postmortems, and record any approved carried exception.
3. **Baseline controlled records.** Complete REQ-0039 and create REQ-0040,
   REQ-0041, REQ-0042 plus TC-0040, TC-0041, and TC-0042. Update indices,
   manifests, strategy, and traceability before product correction.
4. **Create the facility inventory and profile schema.** Enumerate clauses
   7.1--7.25, optional/conditional facilities, implementation-defined items,
   standard pragmas, Annex F claims, all 24 headers, and direct evidence keys.
5. **Run the declaration/header audit.** Test each header alone, repeated and
   mixed include order, C89/C99 exposure, exact prototypes/qualifiers, typedef
   size/alignment/signedness, macro values/types, and constant expressions on
   all three target compilers.
6. **Audit base diagnostics, character, error, locale, and common definitions.**
   Cover `assert`/`__func__`, `isblank`, `EILSEQ`, `struct lconv`, locale
   integration, `NULL`, `offsetof`, and dependent limits/types. Log findings
   before corrections and rerun TC-0001--TC-0006 and focused T6 tests.
7. **Audit integer, varargs, signal, and nonlocal-jump contracts.** Recheck all
   limit/constant expression types, format macros, `va_copy`, varargs ABI,
   signal constants/handlers, and `jmp_buf` calling-state behavior. Rerun the
   affected C89/C99 and native ABI cases.
8. **Close REQ-0039 numeric text.** Separate mandatory hexadecimal rounding,
   implementation-defined accuracy, recommended decimal practice, current
   rounding direction, locale syntax, narrow/wide/format/scan agreement, and
   range/errno behavior. Correct only baselined gaps and rerun exact boundary
   and cross-interface vectors.
9. **Audit general utilities and multibyte conversion.** Verify `MB_CUR_MAX`
   type/value, integer and floating conversions, allocation/process/search/sort,
   C-locale state, restartable and wide adapters, and failure/resource paths.
10. **Audit streams and formatted I/O.** Verify declarations, stream
    orientation/state, all C89/C99 modifiers/conversions, retained input,
    errors, positioning, files, bounded output, and static/DLL behavior.
11. **Audit strings, wide strings, classification, and time.** Verify
    `restrict` contracts, unsigned-byte behavior, locale collation, all wide
    families, complete `strftime`/`wcsftime` conversions and modifiers, and
    implementation-defined time/encoding behavior.
12. **Audit real math and floating environment.** Challenge every macro,
    special/range/rounding/flag result and standard pragma; remove or formally
    disposition the `fma` deviation without inferring universal behavior from
    samples.
13. **Audit complex and type-generic math.** Requalify compiler capabilities,
    ABI, constants, optional imaginary/pragma behavior, all functions and
    generic selections, single evaluation, and branch/special/range behavior.
14. **Run focused correction verification.** For each defect, rerun the direct
    reproducer, affected requirement tests, C89 isolation, dependent consumer,
    target ABI, and import checks; record removal phase and observed fix effort.
15. **Run full clean local gates serially.** Execute WSP self-tests,
    traceability, source quality, C89, C99, compatibility, capabilities, Debug
    and Release builds, consumers, startup objects, x86 imports, package
    assembly/verification, evidence validation, and patch-quality checks.
16. **Run exact-revision CI.** Require native x86/x64/ARM64 detailed results at
    the same source/dependency baseline and retain success and failure evidence
    with artifact identities. Any unavailable required result remains Unknown.
17. **Reconcile controlled status.** Update requirements, profile,
    implementation-defined/deviation/platform documents, manifests, roadmap,
    work plan, and R1 inputs from the exact results; do not advance a checkbox
    from implementation presence alone.
18. **Review and close T6.** Complete personal and independent reviews,
    disposition every finding, record actual size/effort/schedule/defects,
    perform the postmortem, and issue the gate-by-gate T6 decision.
19. **Only after terminal T6, start R1.** Create the project-owned release
    readiness record from the pinned WSP template, prepare an exact unsigned
    candidate and safe local evidence, then perform a separate read-only
    independent readiness audit. Publication/signing/external writes still
    require explicit authority.

## Time Log

| Date | Phase | Focused minutes | Excluded interruption | Note |
| --- | --- | ---: | --- | --- |

No focused minutes are inferred from repository inspection, chat duration,
command wall time, commit timestamps, or CI duration.

## Defect Log

| ID | Type | Affected baseline | Priority | Owner | Injected | Removed | Fix minutes | Related artifact | Disposition |
| --- | --- | --- | --- | --- | --- | --- | ---: | --- | --- |
| T6-D001 | build/tooling | T5 entry | critical | WCRT maintainer | T5 implementation | T6 implementation | -- | commit `1d681da8ab439a0e63ee7433ed289037806a26c7` | Removed: reviewed T5 implementation and the approved T6 baseline were committed after x86/x64 C89, C99, compatibility, build, consumer, startup, traceability, WSP-tool, and source-quality checks. ARM64 execution remains separately Unknown. |
| T6-D002 | requirements | C99 closure planning | critical | WCRT maintainer | Program planning | T6 specification | -- | REQ/TC-0040--0042 | Removed: project requirements, test specifications, index, manifest, traceability, and aggregate entries are controlled. |
| T6-D003 | interface/ABI | C89 assertion baseline | high | WCRT maintainer | C89 implementation | T6 implementation | -- | `include/assert.h`, `src/assert.c`, TC-0040 | Removed: C99 diagnostics include `__func__`; the C89 helper and surface remain separately selected. |
| T6-D004 | interface/ABI | C89 character baseline | high | WCRT maintainer | C89 implementation | T6 implementation | -- | `include/ctype.h`, `src/ctype.c`, TC-0040 | Removed: edition-selected `isblank` implements space/tab and unsigned-character/EOF partitions. |
| T6-D005 | interface/ABI | C89 locale baseline | high | WCRT maintainer | C89 implementation | T6 implementation | -- | `include/locale.h`, `src/locale.c`, TC-0040/0042 | Removed: the six international monetary members and full runtime layout are present while the C89 prefix surface remains selected. |
| T6-D006 | interface/ABI | C89 utility baseline | high | WCRT maintainer | C89 implementation | T6 implementation | -- | `include/stdlib.h`, TC-0040/0042 | Removed: `MB_CUR_MAX` is the positive constant expression `(size_t)1`. |
| T6-D007 | test | T0--T5 test design | critical | WCRT maintainer | Earlier tranche design | T6 implementation | -- | C99 manifest/runners, TC-0040--0042 | Removed: all 24 C99 headers, all 15 C89 headers, repeated/mixed inclusion, ABI, profile, and existing-family inventory gates are controlled. |
| T6-D008 | documentation | T0--T5 status | high | WCRT maintainer | Earlier verification | -- | -- | work logs, roadmap, ARM64 record | Open: status and evidence records are unreconciled or stale. |
| T6-D009 | documentation | Conformance documentation | critical | WCRT maintainer | Project bootstrap | T6 specification | -- | `docs/c99-conformance-profile.md`, `docs/implementation-defined.md`, `docs/c99-deviations.md` | Removed: 75 facility rows cover clauses 7.1--7.25 and all 24 headers with independent classifications and target states. |
| T6-D010 | requirements/numeric | REQ-0028/REQ-0039 | high | WCRT maintainer | Earlier requirements/implementation | T6 specification/verification | -- | REQ/TC-0039; decimal profile test | Removed: mandatory hexadecimal rounding, implementation-defined decimal accuracy, omitted recommended practice, C-locale syntax, wide dependencies, and a one-ULP counterexample are explicit. |
| T6-D011 | numeric | T4 | critical | WCRT maintainer | T4 implementation | T6 implementation | -- | `src/math.c`, TC-0035, `tests/c99/data/fma-vectors.json` | Removed: exact integer product/addend accumulation and single active-direction binary32/binary64 rounding replace the sample-only compensated implementation. Native exact-revision ARM64 evidence remains pending. |
| T6-D012 | build/tooling | CI dependency provisioning | high | WCRT maintainer | CI design | -- | -- | `.github/workflows/build.yml` | Open: `latest` inputs do not establish a pinned reproducible baseline. |
| T6-D013 | documentation/evidence | ARM64 support | high | WCRT maintainer | Earlier platform documentation | -- | -- | ARM64 record and local result inventory | Open: documentation is stale and current native local evidence is absent. |
| T6-D014 | test | Startup aggregate runner | critical | WCRT maintainer | Earlier test implementation | T6 implementation | -- | `tools/test-startup-objects.ps1` | Removed: the runner reported Pass and returned success even when TC-0017 and TC-0018 returned Fail. It now throws for Fail, reports Blocked without Pass wording, and only reports Pass when every child result passes. |
| T6-D015 | documentation/dependency | T5 compiler baseline | high | WCRT maintainer | T5 verification | -- | -- | T5 records; TinyCC package `d5c02f0fcdfdf75265d38df6ff9db2f8067367ac` | Open: records name TinyCC 1441, while the locally selected package is `v0.9.28-rc.1442+2474e1c2`; exact executable digests and target results must be reconciled without transferring claims. |

Fix effort is unknown and shall be recorded when each defect is diagnosed and
removed. Planning findings are not silently converted into implementation.

## Planning Review Checklist

- [x] Repository status, history, work plan, project/personal process, pinned
      WSP template, requirements/manifests, headers/sources, runners, workflow,
      local evidence, packages, and prior tranche logs were inspected.
- [x] Scope, exclusions, controlled size, phase effort, reserve, dependencies,
      risks, completion criteria, quality plan, classification model, and exact
      order are proposed.
- [x] Known discrepancies and defects are recorded without correction or Pass
      inference.
- [x] T5 has a clean reviewed exact source baseline (`1d681da8ab439a0e63ee7433ed289037806a26c7`).
- [ ] T0--T5 inherited gates are closed or explicitly accepted as exceptions.
- [x] REQ-0039 and REQ/TC-0040--0042 are reviewed and baselined.
- [x] T6 implementation is authorized.
- [ ] Actual measures, verification, review, and postmortem are complete.

## Postmortem Status

Not started. The terminal postmortem shall compare this inspected baseline with
actual artifacts, focused effort, schedule, defects by injection/removal phase,
quality activity completion, target evidence, estimate error, and selected
process improvements. Missing personal time shall remain unavailable rather
than inferred.
