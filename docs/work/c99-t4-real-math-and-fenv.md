# C99 T4 Real Math and Floating Environment Work Log

**Status:** Proposed baseline; no production implementation started

**Owner:** WCRT maintainer

**Created:** 2026-08-09

**Inspected source baseline:** current workspace state, including T0 capability and floating-model records and the existing C89-only `math.h` implementation

## Baseline and Scope

This tranche baselines and then implements C99 REQ-0035 and REQ-0036. It covers the public real-math and floating-environment surface required by C99 7.12 and 7.6, with the current project evidence and the T0 TinyCC capability record as the governing baseline.

The tranche shall establish and verify the supported contract before introducing any new library implementation. The baseline must explicitly separate:

- required C99 behavior from implementation-defined or target-specific behavior;
- the supported floating model from unsupported CPU or toolchain behavior;
- C89-safe public headers from later C99 declarations and macros;
- exact numerical behaviors from architecture-dependent signal or exception behavior;
- controlled requirement/test records from informal assumptions.

**In-scope requirement behavior:**

- `math.h` real-math classifications and comparison macros, and the complete `float`/`double`/`long double` family surface where the project intentionally supports it;
- `HUGE_VAL`, `HUGE_VALF`, `HUGE_VALL`, and the related infinities, signed zero, NaNs, subnormals, and poles;
- classification and comparison helper macros such as `isfinite`, `isnan`, `isinf`, `signbit`, `fpclassify`, `fmax`, `fmin`, `fdim`, `fma`, `nextafter`, and adjacent comparison helpers as applicable;
- domain/range errors, `errno`, `math_errhandling`, and `MATH_ERRNO`/`MATH_ERREXCEPT` behavior;
- exception flag behavior for `fenv.h`, rounding mode control, and the saved environment contract;
- the project’s `long double` model under the Windows binary64 ABI on x86, x64, and ARM64;
- architecture-specific evidence and deviations, including any compiler-blocked or unsupported facility.

**Excluded or explicitly deferred:**

- `REQ-0037` complex arithmetic and `REQ-0038` type-generic math; T5 remains separate and is not assumed supportable on TinyCC from T4 evidence;
- optional host-CRT or non-WCRT math delegation;
- undocumented CPU-specific floating behavior not proven by repository-controlled tests;
- any `fenv` or math facility that cannot be shown by traceable evidence on a supported target.

## Repository Reconciliation and Discrepancies

The work-plan estimate for T4 in `docs/C99-1.0-WORK-PLAN.md` is a generic placeholder and does not match the repository evidence for this tranche.

1. `docs/C99-REQUIREMENTS.md` names REQ-0035 and REQ-0036 as planned, but there is no controlled requirement record, test specification, manifest row, or implementation baseline for either requirement in `docs/req-0035-*` or `tests/c99/`.
2. The current `include/math.h` is the earlier C89 implementation and declares only the C89 22-function surface; it does not include C99 classification macros, comparison operators, `nan`/`isnan` family logic, `fenv` integration, or the later real-math coverage.
3. The current `include/float.h` and T0 capability record establish the Windows TinyCC floating model, but they do not define or verify the real-math accuracy criteria for `math.h` or any `fenv.h` behavior. They also do not define `math_errhandling`, `MATH_ERRNO`, or `MATH_ERREXCEPT` for the project.
4. The repo has no `include/fenv.h`, `src/fenv.c`, `tests/c99/fenv.c`, or corresponding manifest entries. There is therefore no retained evidence for rounding mode, exception flags, saved environment, or `fesetround`/`fegetround` behavior.
5. The T0 record defines `long double` as binary64 on all targets. This is a durable project decision, but it does not prove the full real-math and `fenv` behavior of the compiler and CPU. T4 must test supported behavior and record unsupported or compiler-blocked behavior explicitly.
6. The TinyCC capability matrix records `_Complex` as unsupported and `_Generic` as supported. That is a T5 dependency risk, not a T4 capability claim. T4 must not conflate the T5 blocker with the real-math baseline.
7. The existing C89 math tests verify ordinary values, domain/range errors, and a subset of `errno` behavior, but they do not establish C99 special-value handling, NaN/inf propagation, branch behavior, `signbit`, subnormal behavior, or rounding/flags semantics.
8. There is no controlled architecture-specific evidence for x86, x64, and ARM64 `fenv` behavior or CPU rounding differences. T4 shall therefore preserve a target-by-target evidence gate rather than inferring equivalence from a single host build.
9. The work plan’s generic T4 size/range does not include the requirement/test/specification production, runner/manifest integration, C89 isolation, numerical-reference review, and retained-evidence management that are mandatory under `docs/PROJECT-PROCESS.md`.

## Size and Effort Estimate

**Controlled size:** two primary conformance units, the project’s real-math and floating-environment contract, the associated public-header and library surface, controlled accuracy/vector tests, x86/x64/ARM64 evidence, and the C89-isolation and regression obligations.

**Estimated change size after repository inspection:** 20–36 source-controlled artifacts and approximately 1,400–2,800 additional or modified lines across requirements, tests, runners, manifests, headers, implementation, and retained-evidence support. This estimate includes controlled design records, direct work on the public contract, and verification artifacts; it is not an implementation-only LOC estimate.

| Phase | Planned focused hours |
| --- | ---: |
| Plan and estimate | 6–10 |
| Specify controlled requirements and tests | 18–30 |
| Design and design review | 20–34 |
| Implement | 38–68 |
| Personal code and test review | 12–22 |
| Verify and retain evidence | 20–36 |
| Postmortem and re-estimate | 4–8 |
| **Base total** | **118–208** |

This is a baseline estimate only. It is intentionally narrower than the generic work-plan row because the repo currently has no T4 controlled records, no `fenv.h`, and no numeric-reference strategy. The estimate is subject to a 15–25 percent reserve for target-specific float behavior and compiler-blocked facilities discovered during the requirement/test baseline; that reserve is not silently consumed as implementation time.

## Dependencies, Risks, and Responses

- **Floating model remains toolchain-specific:** The T0 record establishes the TinyCC Windows binary64 model, but not every facility is portable or available. Tests must use the actual compiler output and architecture record rather than assuming a host ISA model.
- **`fenv.h` may be absent or partial on TinyCC:** Do not assume `fenv_t`, round modes, or flag operations are present. The tranche must classify a missing capability as compiler-blocked or unverified and retain that decision in the requirement/test record.
- **Long-double treatment is ABI-based, not abstract:** The project decision is binary64 `long double` on all supported targets. Accuracy and classification tests must match that model and must not imply IEEE 754 binary80 or 128 behavior.
- **Special-value handling is numerically fragile:** NaN propagation, signed zero, subnormal handling, poles, and `pow`/`log` domain boundaries require controlled reference vectors and explicit tolerances from the published floating model.
- **C89 isolation must remain intact:** C99 math macros and `fenv.h` must not leak into the selected C89 surface or earlier public headers under `WCRT_C89`.
- **Architecture differences remain a test gate:** x86, x64, and ARM64 share a Windows binary64 `long double` claim, but the compiler and each target’s actual exception or rounding path may differ. T4 treats architecture-specific evidence as required; it does not take any target as equivalent without proof.
- **Unsupported CPU behavior must not be inferred:** No assumed SSE/AVX/x87/NEON behavior is allowed; each supported target must be tested or classified as unsupported.

## Completion Criteria

- REQ-0035 and REQ-0036 are reviewed and baselined before their implementation is represented as conforming.
- Each requirement maps to a controlled specification, runner, manifest entry, implementation, and retained result.
- `math.h` and `fenv.h` demonstrate the exact supported floating model, architecture profile, and deliberate deviations or compiler-blocked facilities.
- The project records the supported accuracy methodology for special values and ordinary values, with explicit tolerances tied to the binary64 model.
- `errno`/`math_errhandling` behavior, `MATH_ERRNO` and `MATH_ERREXCEPT`, signal and flag behavior, and rounding-mode controls are tested and retained.
- C89 isolation remains green, and no C99 math or `fenv` facility leaks into the C89 public surface.
- x86, x64, and ARM64 test evidence is retained, native execution is used where available, and any target-only or compiler-blocked result is classified rather than inferred.
- The quality checklist, time log, defect log, and postmortem are complete and evidence-based.

## Quality Plan

- Review requirement completeness and C99 clause traceability before implementation.
- Use the T0 TinyCC capability matrix and `float.h` model as the governing baseline; do not infer unsupported behavior from host CRTs or other compilers.
- Require a controlled reference-vector suite for ordinary values, domain boundaries, infinity/NaN handling, signed zero, subnormals, and poles.
- Validate macro behavior, including repeated evaluation, macro argument safety, and the selected C89/C99 header gating.
- Verify ABI and representation checks for `float`, `double`, `long double`, `HUGE_VAL*`, `fenv_t`, and the `rounding_mode`/`fexcept_t` layout on each target.
- Run the full C89 regression suite and the project C99 aggregate at the T4 completion gate; any missing inventory or failure is a block to completion.
- Retain architecture-specific JSON and output evidence for each target instead of relying only on local host success.
- Perform numerical, source-quality, traceability, and release-readiness review before closing the tranche.

## Exact Implementation Order

1. Baseline the controlled requirement records for REQ-0035 and REQ-0036, including supported accuracy, special-value behavior, `errno`/`math_errhandling`, flag behavior, rounding control, long-double treatment, and architecture differences.
2. Review and update the test manifest and aggregate runner inventory to include the new T4 cases and the direct C89-isolation checks.
3. Define the controlled reference vectors for classification/comparison, finite/infinite/NaN behavior, signed zero, subnormals, domain/range boundaries, rounding, and saved-environment behavior.
4. Add the public header, type, and constant baseline for `math.h` and `fenv.h` as a contract, without claiming unsupported facilities.
5. Implement the C99 real-math family in the smallest supported set consistent with the required behavior and the current floating model; preserve the C89-only baseline and avoid host-CRT delegation.
6. Implement `fenv.h` and the associated environment/flag functions only for facilities proven by the compiler and target evidence; classify missing or unsupported entries as compiler-blocked or deviation, not silent omission.
7. Run the forced numerical, ABI, macro, C89-isolation, and regression review cycles, and retain the target-specific outputs.
8. Close the tranche only when the evidence for each supported target is retained and the explicit deviations are recorded.

## Exact Execution and Classification Rules

- No C99 math or `fenv` capability is assumed from the presence of a host compiler or CPU feature alone.
- If a facility is absent or not implementable on TinyCC or a supported target, it shall be recorded as either `compiler-blocked` or `approved deviation` and shall not be presented as supported behavior.
- The current T0 evidence supports a Windows binary64 `long double` model and TinyCC `_Generic` availability. It does not support any unsupported real-math or `fenv` claim unless directly tested.
- T4 is not the place to claim or implement the T5 complex or type-generic blocker. T5 remains gated by the compiler capability baseline and an explicit decision.

## Time Log

| Date | Phase | Focused minutes | Excluded interruption | Note |
| --- | --- | ---: | --- | --- |

No focused minutes are entered from chat or command wall time; neither is a reliable measure of maintainer-focused effort. The maintainer may add observed focused time later.

## Defect Log

| ID | Type | Injected | Removed | Fix minutes | Related artifact | Disposition |
| --- | --- | --- | --- | ---: | --- | --- |
| T4-D001 | requirements | Planning | Planned baseline | — | `docs/C99-1.0-WORK-PLAN.md`, repo requirement records | The work-plan row is a generic placeholder and does not match the existing repository evidence. The T4 baseline therefore starts as an evidence-driven requirement/test reconciliation rather than an implementation assumption. |
| T4-D002 | test | Planning | Planned baseline | — | `tests/c99/manifest.md`, missing T4 cases | No controlled T4 requirement/test record exists; the manifest and runner inventory will be extended before implementation begins. |
| T4-D003 | interface/ABI | Planning | Planned baseline | — | `include/math.h`, `include/float.h` | The repo documents a Windows binary64 `long double` model and the C89-only math surface, but no C99 math or `fenv.h` ABI contract exists. The exact supported public surface must be baselined before implementation. |
| T4-D004 | build/tooling | Planning | Planned baseline | — | `include/fenv.h`, test runner integration | The project has no `fenv` header or runner output. This is not a code bug yet; it is an unfilled implementation and evidence requirement for the tranche. |

## Review Checklist

- [ ] Requirement and architecture traceability for REQ-0035 and REQ-0036 are complete.
- [ ] T0 floating-model and TinyCC capability records are explicitly used as the authoritative baseline.
- [ ] Numerical reference vectors cover ordinary values, special values, poles, and boundaries.
- [ ] `errno`, `math_errhandling`, and exception/flag behavior are defined before implementation.
- [ ] Rounding modes and saved environment semantics are specified and tested.
- [ ] `long double` treatment is documented as Windows binary64 on all supported targets.
- [ ] C89 isolation checks for `math.h` and the new `fenv.h` are planned and enforced.
- [ ] x86, x64, and ARM64 evidence requirements are captured before any code is represented as completed.
- [ ] Compiler-blocked or unsupported facilities are explicitly classified rather than assumed.
- [ ] Final verification includes aggregate C99, C89 regression, and retained evidence review.

## Postmortem Status

This is a pre-implementation tranche baseline. No focused effort, implementation result, or real numeric evidence is recorded yet. The work log deliberately records the current evidence and the required completion gates before code changes are made.
