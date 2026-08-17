# REQ-0041 — C99 conformance profile

**Content type:** Project requirement

**Status:** Implemented and automatically verified; independent review pending

**Source:** ISO/IEC 9899:1999 as corrected through TC3, clauses 7.1--7.25 and
Annex J

## Scope

This requirement controls the release-facing, clause-level hosted-library
profile. It is an evidence index, not a replacement for facility tests.

## Requirement

- The project shall maintain `docs/c99-conformance-profile.md` with at least
  one row for every library clause 7.1 through 7.25 and every one of the 24 C99
  standard headers.
- Every facility row shall state its normative class (`required`, `optional`,
  `recommended-practice`, `implementation-defined`, `unspecified`, or
  `out-of-profile`), implementation disposition (`implemented`,
  `optional-omitted`, `compiler-blocked`, `deviating`, or `not-applicable`),
  x86/x64/ARM64 evidence status, and direct requirement, implementation, test,
  and evidence references.
- Standard pragmas, optional imaginary types, Annex F, implementation-defined
  behavior, compiler-owned language features, and approved deviations shall be
  explicit. `Unknown`, `Fail`, unapproved `ExpectedFail`, and unapproved
  deviations on required behavior block the conformance gate.
- Derived summary labels shall not erase the independent normative,
  disposition, and evidence dimensions. Absence of a failure is not evidence
  of Pass.

## Verification

**Method:** Automated inventory/schema/reference validation plus clause-level
personal and independent review

**References:** `TC-0041`

TC-0041 shall reject missing clauses, headers, classifications, target states,
or referenced artifacts. Human review shall challenge every Pass against the
cited exact-revision evidence.

## Relationships

- **Depends on:** REQ-0040 and all controlled C99 facility requirements
- **Required by:** REQ-0042 and release readiness
- **Conflicts with:** A blanket or self-declared C99 conformance claim

## Implementation Record

`docs/c99-conformance-profile.md` contains 75 classified rows over all 25
library clauses and 24 standard headers. TC-0041 passes its positive and
negative inventory checks at source
`aea690d2caf108054d1644eefb176efde779f524`; the target Pass cells cite exact
native run `32027269426`. F-010, F-017, and F-037 remain compiler-blocked
ExpectedFail, F-009 and F-055 remain omitted optional/recommended practice,
and F-038 remains out of profile. Independent challenge remains a release gate.
