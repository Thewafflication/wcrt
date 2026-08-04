# C99 T0 Baseline Correction Work Log

**Status:** Planned

**Owner:** WCRT maintainer

**Created:** 2026-08-04

## Scope and Estimate

Reconcile ROADMAP status for REQ-0021 through REQ-0026, integrate TC-0021
through TC-0026 into the aggregate C99 runner and CI evidence, add TinyCC C99
capability probes, document x86/x64/ARM64 data models, and automate the Windows
2000 x86 import check. Runtime C99 work beyond REQ-0026 is excluded.

**Size:** One process correction, six test integrations, one capability matrix,
and approximately 150–350 changed lines.

**Forecast:** 16–28 focused hours. No completion date is committed until the
maintainer records available weekly capacity.

| Phase | Planned effort |
| --- | ---: |
| Plan | 2–3 h |
| Specify and design | 3–5 h |
| Implement | 5–9 h |
| Personal review | 2–3 h |
| Verify | 3–6 h |
| Postmortem | 1–2 h |

## Assumptions and Risks

- Existing TC-0021 through TC-0026 runners pass with the currently supported
  TinyCC packages; this has not been reverified as part of planning.
- CI already exposes enough target-specific information to drive capability
  probes without downloading another compiler.
- Import inspection may require a tool already present in the WPM or Windows
  build environment. Tool selection is part of T0 design.

## Quality Plan

- Review the aggregate runner and CI result enforcement for omitted tests.
- Verify positive capability results and intentional negative/isolation cases.
- Run C89 and the complete C99 aggregate on x86, x64, and ARM64.
- Confirm generated JSON and summaries preserve every controlled C99 result.
- Review the data-model and import-policy records against actual binaries.

## Time Log

| Date | Phase | Focused minutes | Excluded interruption | Note |
| --- | --- | ---: | --- | --- |

## Defect Log

| ID | Type | Injected | Removed | Fix minutes | Related artifact | Disposition |
| --- | --- | --- | --- | ---: | --- | --- |

## Review Checklist

- [ ] Requirement and manifest status agree with repository evidence.
- [ ] Every controlled C99 test appears in aggregate results.
- [ ] Capability probes cover `_Bool`, `long long`, `restrict`, variadic
      macros, complex arithmetic, and type-generic macros.
- [ ] C89-mode isolation remains green.
- [ ] ABI/data-model results cover x86, x64, and ARM64.
- [ ] Windows 2000 x86 imports are automatically inspected.
- [ ] Documentation, CI summaries, and retained evidence agree.

## Postmortem

Complete after verification. Compare planned and actual size, phase effort,
forecast, defects, and quality activities; then record estimate or checklist
changes proposed for T1.
