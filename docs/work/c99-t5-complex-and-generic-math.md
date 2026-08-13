# C99 T5 Complex and Type-Generic Math Work Log

**Status:** Planning baseline proposed; implementation No-Go because the
supported TinyCC package rejects `_Complex`; T4 fused-operation disposition
and independent review also remain open

**Owner:** WCRT maintainer

**Created:** 2026-08-11

**Planning reconciliation:** 2026-08-12

**Inspected source baseline:** `027b324e233d4a0c1912667835dbef31d61c6dcc`
(`Implement C99 T4 math and floating environment`)

**Exact-revision CI evidence:** GitHub Actions run `31463268579` completed
successfully for source quality, native x86, x64, and ARM64 Debug jobs, and the
multi-architecture Debug package at the inspected revision. Release jobs were
not part of that push run.

**Fresh capability evidence:** Local package
`TinyCC 0.9.28-rc.1437+2be0218b` was probed on 2026-08-12 with its i386,
x86_64, and AArch64 Windows compilers. All three reject `_Complex` with
`_Complex is not yet supported`; all three accept the existing narrow
`_Generic` probe.

## Tranche Decision

This record plans REQ-0037 complex arithmetic and REQ-0038 type-generic
mathematics. No T5 implementation is authorized by this planning baseline.
The implementation phase is a **No-Go** until all entry gates below have an
accepted disposition:

1. A supported and provenance-controlled TinyCC package must compile the C99
   `_Complex` types, constants, conversions, operators, calling convention,
   and required pragma behavior on x86, x64, and ARM64. The current compiler
   baseline fails at the type keyword.
2. If that compiler support is not supplied, an explicit release-profile
   decision must classify REQ-0037 and dependent REQ-0038 as
   `compiler-blocked` or approve a named deviation. A pair-of-scalars struct
   or a private extension must not be represented as C99 `_Complex` support.
3. T4's native target run is reconciled into its controlled records. The `fma`
   correctly-rounded-operation deviation and independent review remain open;
   REQ-0038 dispatches to that real target and cannot silently upgrade its
   claim.
4. REQ-0037, REQ-0038, TC-0037, TC-0038, the accuracy method, and any durable
   complex ABI/compiler decision must be reviewed before implementation is
   represented as conforming.

Blocked waiting time is not focused engineering effort. Work on TinyCC itself
is a separate dependency and is excluded from this WCRT tranche estimate
unless separately inspected and re-baselined.

## Proposed Scope and Baseline

### REQ-0037 — Complex arithmetic

The controlled requirement shall cover C99 7.3 and the supported portions of
the core-language complex model:

- a self-contained, edition-selected `<complex.h>` with `complex`,
  `_Complex_I`, and `I`; `complex` must designate the compiler's `_Complex`
  type rather than a library-defined substitute;
- `_Complex_I` as a constant expression of type `const float _Complex`, `I`
  selecting the available imaginary unit, and the standard permission for a
  program to undefine and redefine `complex`, `imaginary`, and `I`;
- omission of `imaginary` and `_Imaginary_I` when the compiler does not
  support the optional imaginary types, with `I` then based on `_Complex_I`;
- `float complex`, `double complex`, and `long double complex`, including
  construction, assignment, conversions, component representation, arithmetic
  operators, and target calling convention;
- all 22 C99 complex function names and their `f`, unsuffixed, and `l`
  variants, for 66 linkable functions total:
  - trigonometric: `cacos`, `casin`, `catan`, `ccos`, `csin`, and `ctan`;
  - hyperbolic: `cacosh`, `casinh`, `catanh`, `ccosh`, `csinh`, and `ctanh`;
  - exponential/logarithmic: `cexp` and `clog`;
  - power/absolute value: `cabs`, `cpow`, and `csqrt`; and
  - manipulation: `carg`, `cimag`, `conj`, `cproj`, and `creal`;
- the default-off state, lexical scope, and supported behavior of
  `#pragma STDC CX_LIMITED_RANGE`, with unsupported compiler behavior
  explicitly classified rather than ignored;
- radians, principal values, branch cuts, the side-of-cut role of signed zero,
  infinities, NaNs, poles, overflow, underflow, and chosen `errno`/floating-
  environment behavior;
- the Windows binary32/binary64 model, including binary64 `long double`
  components, without inferring argument/return ABI equivalence among x86,
  x64, and ARM64; and
- C89 isolation, header repeatability, static and DLL linking, and absence of
  host-CRT complex-math delegation.

The tranche does not claim optional imaginary types, IEC 60559 Annex G,
correctly rounded transcendental results, C11 `CMPLX` construction macros,
complex formatted I/O, or complex text conversion unless a later controlled
requirement adds them.

### REQ-0038 — Type-generic mathematics

The controlled requirement shall cover C99 7.22 after every selected real and
complex target is independently verified:

- `<tgmath.h>` includes WCRT `<math.h>` and `<complex.h>` and exposes every
  eligible C99 type-generic macro; `modf` is explicitly excluded, and `nan` has
  no generic parameter and therefore has no corresponding generic macro;
- 17 names select between corresponding real and complex families, including
  the `fabs`/`cabs` mapping;
- 38 names select among real-only `f`, unsuffixed, and `l` functions;
- `carg`, `cimag`, `conj`, `cproj`, and `creal` form the five complex-only
  generic names, for 60 generic macro names total. The map therefore selects
  among 165 real targets and all 66 complex targets;
- generic parameters select `long double` when any generic argument has that
  corresponding real type, otherwise `double` for a double or integer generic
  argument, and otherwise `float`;
- a complex generic argument selects the complex target for a shared family;
  a real argument selects the real target; real-only macros receive only real
  generic arguments under their defined contract;
- the selected expression has the prescribed real or complex result type,
  including integer-return functions and multi-argument mixed-type cases;
- each run-time macro argument is evaluated exactly once, and type selection
  itself has no side effect;
- ordinary-function access remains available when a standard function-like
  macro is suppressed with parentheses; and
- C89 mode exposes neither `<tgmath.h>` facilities nor T5 additions through
  an earlier public header.

TinyCC's accepted `_Generic` syntax is a compiler extension in the selected
C99 mode, not proof of `<tgmath.h>` conformance. The dispatch design must be
qualified against the exact supported compiler package and must not depend on
unstated C11 behavior.

## Accuracy and Special-Value Baseline

The controlled TC-0037 vectors shall separate exact, ordinary, ill-conditioned,
branch-cut, and exceptional partitions.

- Construction, `creal`, `cimag`, `conj`, projection, component signs, real
  inputs, pure-imaginary inputs, zero, infinity, and NaN classification shall
  use exact types and bit-level component checks where the result is exact.
- For well-conditioned ordinary finite vectors, each binary64 component shall
  satisfy `abs(actual - expected) <= 1e-11 * max(1, abs(expected))`; binary32
  components shall use `1e-5` in the same formula. Windows
  `long double complex` uses the binary64 bound. These are explicit executable
  bounds, not claims of correct rounding or a uniform number of significant
  digits.
- Relative/scaled component tolerances apply only to well-conditioned ordinary
  vectors. Near zeros, poles, branch points, or cancellation, the specification
  shall use absolute bounds, residuals, mathematical identities, or a stated
  condition-aware tolerance rather than a misleading relative error.
- Branch-cut tests shall approach both sides with positive and negative zero
  and verify the principal-value range and exact zero sign where required.
  A single unsigned-cut result is insufficient on WCRT's signed-zero targets.
- Infinity/NaN cases shall use a controlled input/output classification matrix
  for every family. An Annex G special-value table may be selected only by an
  explicit design decision; using it as test data does not by itself claim all
  Annex G behavior.
- Overflow and underflow cases shall verify component classification and sign,
  retained `errno` and `fenv` policy, and the absence of avoidable intermediate
  overflow when the mathematical result is representable.
- Conjugate symmetry shall be checked where the selected principal branch and
  input domain make it applicable. It is supporting metamorphic evidence, not
  a replacement for independent reference vectors.
- Reference values shall come from a recorded MPFR/MPC or equivalently reviewed
  arbitrary-precision source at no less than 256-bit precision, with generator
  version, precision, input text, rounding, and retained output. The generator
  and vector output are source-controlled evidence inputs, not build-time
  dependencies. WCRT or the host CRT shall not be its own numerical oracle.
  High-risk values shall be cross-checked with a second independent method or
  exact identity. Special-value and signed-zero expectations come from the
  controlled C99 matrix rather than an oracle that normalizes zero signs.

TC-0038 is primarily a compile-time and side-effect suite. It shall verify all
60 names, all semantic real and complex types, integer promotion, mixed generic
arguments, result types, selected function identity, macro suppression, and
single evaluation. Numerical duplication is limited to enough vectors to prove
that dispatch reaches the already verified target.

## Repository Reconciliation and Discrepancies

1. Before this reconciliation, `docs/C99-1.0-WORK-PLAN.md` described
   REQ-0035 through REQ-0038 as planned. T4 is implemented at `027b324`, and
   exact-revision CI passed natively on x86, x64, and ARM64. The plan and T4
   controlled records now state that evidence without closing the `fma`
   deviation or independent review.
2. The former T5 row's two-conformance-unit and 2,000--4,000-LOC summary omitted
   the inspected 66-function complex inventory, 60 type-generic macros,
   compiler ABI qualification, controlled numerical vectors, C89 isolation,
   consumer tests, and retained evidence. The work-plan row is now reconciled
   to this inspected baseline.
3. Fresh local x86, x64, and ARM64-target probes with packaged TinyCC
   `0.9.28-rc.1437+2be0218b` still report `_Complex is not yet supported`. No
   approved compiler replacement, library-conforming
   alternative, compiler-blocked release disposition, or complex ABI record
   exists.
4. `_Generic` compiles on the three recorded targets, but the existing probe
   checks only float and double selection. It does not prove long-double,
   integer, qualified, mixed-argument, complex-domain, result-type,
   macro-suppression, or single-evaluation behavior.
5. The workflow installs the latest TinyCC prerelease for each architecture.
   It records exact compiler identity, but T5's compiler-sensitive ABI needs a
   provenance-controlled package baseline and an explicit update/reprobe rule.
6. No REQ-0037/REQ-0038 controlled record, TC-0037/TC-0038 specification,
   manifest row, runner, header, implementation, export, detailed evidence
   format, or T5 architecture result exists.
7. The roadmap's real-math and `fenv` boxes remain unchecked despite the T4
   implementation. Status changes must follow controlled acceptance evidence,
   not be inferred solely from source presence.
8. The initial T5 draft treated all 187 declarations in `math.h` as generic
   targets. N1256 7.22 instead defines 55 real generic names selecting among 165
   real targets, plus five complex-only names; `modf`, `nan`, and WCRT internal
   inquiry helpers are not generic targets. The open `fma` claim must still flow
   through the generic-dispatch record rather than being hidden by a macro pass.
9. T4 states an ordinary-result goal in significant digits while its executable
   tests use scaled tolerances. T5 does not inherit that ambiguity: the explicit
   `1e-11` binary64 and `1e-5` binary32 component bounds above govern ordinary
   well-conditioned T5 vectors. Any change to T4's controlled accuracy claim
   requires its own impact review.

## Refined Size and Effort Estimate

**Controlled size:** two primary conformance units; one compiler/ABI decision;
22 complex base names and 66 exported function variants; 60 generic macro
names; complete C89-isolation, consumer, and architecture evidence.

**Expected repository size after the compiler gate:** 26--40 changed or new
source-controlled artifacts; 2,400--4,800 production header/runtime lines; and
approximately 5,000--9,000 total changed lines including requirements, test
specifications, vector provenance, runners, build integration, and evidence
support. Compiler-source changes, if required, are outside this estimate.

| Phase | Planned focused hours |
| --- | ---: |
| Plan and estimate | 8--14 |
| Specify controlled requirements and tests | 24--40 |
| Design and record compiler/ABI/numeric decisions | 28--48 |
| Implement | 60--104 |
| Personal design, code, and test review | 20--34 |
| Verify locally and in CI | 30--52 |
| Postmortem and re-estimate | 4--8 |
| **Base total** | **174--300** |

The inspected base is 54--90 hours above the former work-plan row's 120--210
hours. The work plan now records this range and T4's inspected 118--208-hour
range, changing the published program base from 785--1,352 to 837--1,450
focused hours. This is not a calendar commitment. A 20--35 percent completion
reserve yields 209--405 hours after the compiler gate; the reserve covers
branch-cut/numeric and target-ABI uncertainty but not work in the TinyCC
repository.

No completion date is forecast while the entry decision is blocked. The T2,
T3, and T4 logs contain no reliable focused-time actuals from which to infer a
personal velocity.

## Dependencies, Risks, and Responses

- **Compiler language blocker -- critical:** `_Complex` is required for the
  C99 public type and operators. Require a qualifying compiler package or stop
  with a controlled compiler-blocked/deviation decision. Do not substitute a
  struct and claim conformance.
- **Compiler ABI instability -- critical:** complex arguments and returns may
  differ by target or compiler revision. Probe sizes, alignment, component
  order, operators, static calls, DLL calls, and cross-translation-unit calls
  on x86, x64, and ARM64 before freezing the WCRT ABI.
- **T4 dependency -- high:** complex formulas use real transcendental,
  classification, scaling, `fenv`, and error paths; `tgmath.h` exposes all of
  them. The native CI success is reconciled; dispose the `fma` deviation and
  complete independent review before making a complete T5 claim.
- **Error and floating-environment policy -- high:** C99 permits complex
  functions to set `errno` but does not require it, while composed T4 calls may
  set `errno` or hardware flags. Baseline a family-by-family policy before
  formulas are implemented, then test preservation and reporting explicitly.
- **Branch cuts and signed zero -- high:** algebraically equivalent formulas
  can select the wrong side or destroy a zero sign. Partition and review each
  inverse, logarithm, power, and square-root cut explicitly.
- **Intermediate range -- high:** naive formulas overflow or underflow when
  the final result is representable. Use scaling algorithms and controlled
  extreme vectors; do not accept ordinary-range evidence as range proof.
- **Reference independence -- high:** host complex functions are unavailable
  or may share defects. Pin and retain an MPFR/MPC or equivalently reviewed
  generator and its outputs, then cross-check high-risk values rather than
  delegating to a host CRT.
- **Generic macro expansion -- high:** repeated or evaluated controlling
  expressions can change behavior. Inspect preprocessor output where useful
  and instrument every argument position with side effects.
- **C99-mode extension reliance -- medium:** TinyCC accepts `_Generic` as an
  extension. Pin its behavior with negative and positive probes and record the
  dependency; an unprobed compiler update cannot silently replace it.
- **Header macro collisions -- medium:** `<tgmath.h>` intentionally shadows
  function names. Verify include order, repeated inclusion, address/function
  access through parentheses, and interaction with `<math.h>`/`<complex.h>`.
- **C89 and earlier-C99 regression -- high:** isolate both headers behind the
  project edition selector and run every existing C89/C99/compatibility gate.
- **Target evidence gap -- high:** local x64 work cannot prove native ARM64.
  Retain detailed native results from `windows-11-arm`; cross-compilation is
  preliminary evidence only.
- **Windows 2000 x86 imports -- medium:** new runtime code must remain
  self-contained and preserve the import allowlist.

## Completion Criteria

T5 is complete only when all of the following are true or have an explicit,
approved release-profile disposition:

- REQ-0037 and REQ-0038 are reviewed and baselined with clause traceability,
  accuracy/error rules, compiler/ABI constraints, and C89 isolation.
- TC-0037 and TC-0038 are controlled, mapped through the manifest and aggregate
  runner, and have executable implementations with enforced failure behavior.
- The exact supported TinyCC package passes expanded complex and generic probes
  on x86, x64, and ARM64; compiler identity and package provenance are retained.
- `<complex.h>` uses real compiler `_Complex` types and provides the required
  constants, construction semantics, operators, and all 66 functions. Optional
  imaginary-type support is accurately declared or omitted.
- Every function family passes ordinary, exact, branch-cut, signed-zero,
  infinity, NaN, overflow, underflow, and applicable symmetry tests under the
  controlled accuracy method.
- `<tgmath.h>` provides all 60 generic macro names after their real and complex
  targets pass independently; all 165 real and 66 complex target selections,
  result type, dispatch, macro suppression, and single evaluation are proven
  for all relevant argument classes.
- C89 facility absence and the full C89 suite remain green. Every earlier
  controlled C99 and Microsoft-compatibility test remains green.
- Debug and Release DLL/static builds, exported inventory, static/DLL consumers,
  startup objects, source quality, traceability, and Windows 2000 x86 imports
  pass where applicable.
- Native x86, x64, and ARM64 results are retained for the exact source and
  compiler revision. Unknown native behavior is not inferred from a cross-build.
- Detailed complex-vector, generic-dispatch, capability/ABI, aggregate, build,
  and consumer evidence is attached to CI and summarized on failure.
- Personal design/code/test review and independent review findings are
  dispositioned. Actual size, observed focused effort, defects, deviations,
  and the postmortem are recorded without fabricated time.

## Quality Plan

- Review corrected C99 7.3 and 7.22 obligation inventories against the header,
  exports, tests, and generic map; mechanically reject inventory drift.
- Expand compiler probes before public-header design: keyword/type support,
  three precisions, constants, conversions, arithmetic, representation,
  `CX_LIMITED_RANGE`, `_Generic` selection, unevaluated control, and ABI calls.
- Record a durable ADR for compiler qualification, component/return ABI,
  optional imaginary types, pragma policy, error/fenv policy, and non-Annex-G
  boundary.
- Generate controlled reference vectors with retained provenance. Review
  formulas for scaling, cancellation, quadrant, branch, and zero-sign behavior.
- Test exact helpers and accessors before transcendental composition. Add each
  function group with its own ordinary, special, range, and cut partitions.
- Use compile-time result-type assertions and instrumented side effects for
  every generic macro form and each generic argument position. Mechanically
  compare the 17 shared, 38 real-only, and five complex-only name inventories
  with the controlled specification.
- Test headers standalone, together in both include orders, repeated, under
  C99, and absent under `WCRT_C89`.
- Build and consume the functions through both static and DLL boundaries on all
  supported targets. Inspect exports and the x86 Windows 2000 import set.
- Run source quality, traceability, WSP tool tests, focused T5 cases, the full
  C89 aggregate, the complete C99/compatibility aggregate, consumer/startup
  tests, and Debug/Release builds as locally available.
- Retain machine-readable detailed results rather than only aggregate Pass
  rows. CI failure summaries must expose the captured diagnostic text.
- Perform separate requirement, numerical design, ABI, macro, code, test, and
  evidence reviews; record each finding and its disposition.

## Exact Implementation Order

1. **Hold the implementation gate.** Record this planning baseline and obtain
   agreement that a struct/private-complex substitute is not C99 conformance.
2. **Close or disposition T4 prerequisites.** Exact-revision native CI is
   reconciled; complete independent review and approve or remove the `fma`
   deviation that REQ-0038 will expose.
3. **Resolve the compiler dependency.** Select a TinyCC package with `_Complex`
   support or approve a compiler-blocked/deviation release decision. If the
   package changes, record source/package provenance and update policy. A
   compiler-blocked decision terminates conforming implementation work and
   proceeds directly to controlled deviation/release-profile closure.
4. **Qualify language and ABI behavior.** Expand and run the complex and
   `_Generic` probes for x86, x64, and ARM64. Do not design the public ABI from
   scalar widths alone.
5. **Baseline controlled records.** Review REQ-0037, REQ-0038, TC-0037,
   TC-0038, the exact 66-function and 60-macro inventories, accuracy method,
   special-value partitions, evidence schema, and the complex ABI ADR.
6. **Create failing focused test scaffolding.** Add presence/C89-isolation,
   declaration/export inventory, representation/ABI, behavioral, vector, and
   runner coverage without weakening the aggregate inventory check.
7. **Add `<complex.h>` as a contract.** Define only compiler-proven macros and
   declarations, preserve C89 isolation, and verify construction/operators and
   cross-translation-unit ABI before adding library formulas.
8. **Implement component and manipulation primitives first.** Complete exact
   component access, conjugation, projection, construction, classification,
   and signed-zero helpers; then add scaled absolute-value and quadrant-correct
   argument paths with exact, ordinary, and special-value tests.
9. **Implement the analytic foundations.** Add exponential, logarithm, square
   root, and power using scaled formulas, with both sides of their cuts and
   ordinary/range/special partitions verified before composition.
10. **Implement forward trigonometric and hyperbolic families.** Add each group
    with ordinary, range, special-value, and applicable symmetry tests.
11. **Implement inverse families and branch cuts.** Add inverse trigonometric
    and hyperbolic functions only after the shared log/sqrt behavior is stable;
    review both sides of every cut and all principal-value ranges.
12. **Verify every real and complex target independently.** Complete all three
    precisions, export inventory, static/DLL consumers, and target-specific
    execution before any type-generic header is enabled.
13. **Add `<tgmath.h>` dispatch.** Implement the 60-name map with compiler-
    qualified `_Generic` selection, function suppression, exact result types,
    and one run-time evaluation per argument.
14. **Run focused and full local regressions.** Execute T5, C89 isolation/full
    C89, all earlier C99/compatibility cases, source quality, traceability,
    builds, consumers, startup objects, and x86 import inspection.
15. **Run and retain native CI evidence.** Require successful x86, x64, and
    ARM64 detailed results at one exact source/compiler revision; preserve all
    failure diagnostics and do not overwrite failed evidence with a later run.
16. **Review and close.** Perform personal and independent reviews, disposition
    every finding/deviation, record actual size and observed effort, update
    controlled status/roadmap only from evidence, and complete the postmortem.

## Execution and Classification Rules

- A failure to parse or operate on `_Complex` is `compiler-blocked`, not a
  missing library function that WCRT can conceal in a conforming header.
- A nonstandard pair-of-scalars API must use separate names and cannot satisfy
  REQ-0037 or the complex-domain portions of REQ-0038.
- If REQ-0037 is blocked, complete `<tgmath.h>` is also blocked. A real-only
  partial header must be an explicit deviation and cannot be reported as C99
  7.22 complete.
- Optional imaginary types may remain unsupported without blocking the required
  complex types; the header and tests must state that distinction exactly.
- Cross-compilation can establish syntax, layout assertions, linkability, and
  PE structure. It cannot establish native numerical, branch-cut, floating-
  environment, or calling behavior.
- A compiler update invalidates previous complex/generic capability and ABI
  evidence until all probes and consumers are rerun.
- No focused minutes, schedule actual, or fix effort shall be inferred from
  chat duration, command wall time, commit timestamps, or CI duration.

## Planning Verification Record

- GitHub Actions run `31463268579` was queried directly and confirms the exact
  T4 source revision plus successful source-quality, native x86/x64/ARM64
  Debug, consumer, startup, C89, C99/compatibility, capability, and Debug
  packaging jobs. Release jobs were skipped for that push run.
- The existing capability runner was rerun with packaged TinyCC
  `0.9.28-rc.1437+2be0218b` for i386, x86_64, and AArch64 Windows. `_Complex`
  remains Unsupported with the same compiler diagnostic on all three;
  `_Generic` remains Supported by the existing narrow probe.
- The complete current C89 TC-0001--TC-0015 and C99/Microsoft-compatibility
  aggregates pass locally and serially on x86 and x64. The aggregate includes
  T4 TC-0035 and TC-0036 and enforces the current manifest inventory.
- All pinned WSP common-tool self-tests pass. Project traceability passes for
  43 requirements, 43 test specifications, and 43 implementations. `git diff
  --check` reports no whitespace errors.
- A first parallel local x86/x64 C89 run collided in a shared TC-0013 build
  path and produced a transient permission-denied result. The isolated TC-0013
  and both serial full reruns pass. This is retained as tooling defect T5-D006,
  not reclassified as a product failure or silently discarded.

## Time Log

| Date | Phase | Focused minutes | Excluded interruption | Note |
| --- | --- | ---: | --- | --- |

No focused minutes are recorded. Planning command wall time and CI duration are
not maintainer-focused effort.

## Defect Log

| ID | Type | Affected baseline | Priority | Owner | Injected | Removed | Fix minutes | Related artifact | Disposition |
| --- | --- | --- | --- | --- | --- | --- | ---: | --- | --- |
| T5-D001 | documentation | C99 program/T5 plan | high | WCRT maintainer | Earlier program planning | T5 planning | -- | `docs/C99-1.0-WORK-PLAN.md` | Removed by reconciling the current T4 status, T5 66-function/60-name scope, estimates, dependencies, and program total in the work plan. |
| T5-D002 | documentation | T4 at `027b324` | high | WCRT maintainer | T4 verification | T5 planning | -- | T4 requirement/work log | Removed by recording run `31463268579` and exact source revision in the controlled T4 records; `fma` and independent review remain open rather than being silently closed. |
| T5-D003 | test | T0 capability/T5 entry | critical | WCRT maintainer | T0 capability design | -- | -- | complex and type-generic probes | The probes correctly identify the broad blocker/input, but are too narrow to qualify complex ABI or complete generic dispatch. Expansion remains required before public-interface design. |
| T5-D004 | requirements | Proposed REQ-0038 | high | WCRT maintainer | Initial T5 planning | T5 planning review | -- | REQ-0038 inventory | Corrected the assumption that all 187 `math.h` declarations are generic targets. The controlled map is 55 real names/165 real targets plus five complex-only names and 66 complex targets. |
| T5-D005 | numeric | Proposed TC-0037 | high | WCRT maintainer | T4 specification inheritance | T5 planning review | -- | T5 accuracy baseline | Removed T5 ambiguity by stating executable component tolerances. Any reconciliation of T4's significant-digit wording remains a separate controlled impact review. |
| T5-D006 | build/tooling | Local multi-target verification | medium | WCRT maintainer | Existing runner design | -- | -- | C89 runners/build paths | Parallel x86/x64 aggregates collided while overwriting the shared TC-0013 executable and one run reported permission denied. The isolated case and serial aggregates pass. Keep architecture runs serial until build paths are isolated; assess a tooling correction before T5 parallel local verification. |

Fix minutes are unknown and are not inferred.

## Review Checklist

- [x] Repository, process, work plan, T4 records, capability probes, headers,
      aggregate, CI workflow, and exact-revision CI result were inspected.
- [x] Current `_Complex` and `_Generic` outcomes are freshly recorded separately
      for all supported target compilers without converting a narrow optional
      probe into library support.
- [x] Proposed scope enumerates the complete complex-function and generic-macro
      surfaces and the optional imaginary-type boundary.
- [x] Size, phase effort, reserve, risks, dependencies, completion criteria,
      quality plan, and exact implementation order are recorded.
- [ ] T4 `fma` disposition and independent review are closed; native target
      status reconciliation is complete.
- [ ] Supported TinyCC complex language/ABI behavior is accepted on all targets.
- [ ] REQ-0037/REQ-0038 and TC-0037/TC-0038 are reviewed and baselined.
- [ ] Complex numerical/ABI design and reference-vector provenance are reviewed.
- [ ] Implementation and personal code/test review are complete.
- [ ] Native x86/x64/ARM64 evidence and independent review are complete.
- [ ] Actual measures, defect dispositions, and postmortem are complete.

## Actuals and Postmortem Status

No T5 implementation exists at this planning revision. Actual changed size,
focused effort, implementation defects, verification results, and schedule are
therefore not recorded. The postmortem remains pending and shall compare the
accepted baseline, any compiler-gate decision, implementation actuals, escaped
defects, and selected process improvements after the tranche reaches a terminal
accepted, compiler-blocked, or deviating disposition.
