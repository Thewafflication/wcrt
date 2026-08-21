# REQ-0042 — C99 compatibility validation

**Content type:** Project requirement

**Status:** Exact native Debug matrix passes; tagged Release validation and
independent audit remain prepublication gates

**Source:** ISO/IEC 9899:1999 as corrected through TC3, clauses 7.1.2 and
7.1.4; WCRT supported-platform policy

## Scope

This requirement controls the whole-product compatibility and evidence matrix
for the C99 exit gate and the separate release-publication gate on Windows
x86, x64, and ARM64.

## Requirement

- Each of the 24 standard C99 headers shall compile alone and when repeated;
  all headers shall compile in controlled mixed orders without undeclared
  dependencies or conflicting declarations.
- The selected C89 surface shall compile all 15 C89 headers and shall not expose
  C99-only headers or names. All C89, C99, and Microsoft-compatibility behavior
  cases shall pass without host standard-library substitution.
- Exact types, signedness, size, alignment, structure layout, macro expression
  types, integer constant expressions, calling conventions, varargs, complex
  values, `jmp_buf`, `fenv_t`, `mbstate_t`, `time_t`, and wide types shall match
  the documented x86 ILP32 and x64/ARM64 LLP64 ABIs.
- At one exact committed source and dependency baseline, the complete C89,
  C99, Microsoft-compatibility, header, ABI, source-quality, traceability,
  Debug static/DLL build, native consumer, startup-object, x86 Windows 2000
  import, Debug package-assembly, and retained-evidence matrix shall pass.
- The candidate source revision may be committed before tagged Release results
  exist. Before publication, the tagged workflow shall build the optimized
  Release static/DLL artifacts and run the native library-consumer and startup
  smoke tests on x86, x64, and ARM64, repeat the x86 Windows 2000 import check,
  assemble and WPM-sign the unchanged Release artifacts, require `wpm verify`
  exit zero, compare each packaged DLL with its Release input, and retain the
  exact results and artifact identities. The complete
  C89/C99/compatibility behavior inventory
  need not be duplicated in Release because the tagged Release jobs depend on
  the successful exact-source Debug matrix.
- The publication job shall depend on every tagged Release architecture and
  WPM-signed-package verification succeeding. A committed source revision, tag,
  cross-build, PE inspection, missing artifact, or unavailable check is not a
  publication Pass. Native Debug execution and the selected native Release
  smoke tests are required on every supported architecture; absent evidence
  remains `Unknown` or `Blocked`.

## Verification

**Method:** Generated header matrix, compile-time ABI assertions, full serial
Debug regression, native target execution, tagged Release consumer/startup
smoke tests, build/package checks, evidence validation, and independent audit

**References:** `TC-0042`

TC-0042 is the controlled Debug matrix entry point. Exact-revision CI supplies
native target evidence that cannot be produced locally. Tagged Release CI is a
separate R1 prepublication gate. The profile and release records shall preserve
the distinction among a committed candidate, a passing Debug baseline, a
pending or passing tagged Release matrix, and a published release.

## Relationships

- **Depends on:** REQ-0040, REQ-0041, REQ-0016 through REQ-0018, and every
  controlled C89/C99/compatibility test
- **Required by:** WCRT 1.0.0 release-readiness approval
- **Conflicts with:** Parallel shared-output test execution or inferred target
  success

## Implementation Record

Exact-revision run `32027269426` passes 15/15 C89 and 30/30 combined
C99/compatibility cases, Debug static/DLL consumers, startup objects, source
quality, traceability, and package assembly on native x86, x64, and ARM64.
Each named target artifact retains detailed results and dependency provenance.
The local exact-dependency Release rebuild passes x86/x64 consumers/startup,
all ARM64 links, and the x86 Windows 2000 import gate. The tagged Release matrix
has not run and remains an R1 prepublication Unknown; it is no longer a T6
completion result. Authenticode identity and timestamping were explicitly
deferred from 1.0.0 on 2026-08-21. The retained verification tools are not
release evidence. Exercised WPM verification, release trust, install/rollback,
and independent audit remain incomplete and are not represented as Pass.

## Impact Note — 2026-08-20

This revision separates requirement/tranche completion from release
publication timing. It does not remove an implemented facility, public API,
supported target, security control, test inventory, ABI check, or release
artifact. The complete behavior matrix remains required in native Debug CI,
while optimized Release native consumers/startup, legacy imports, signing,
package verification, and exact artifact evidence remained mandatory before
publication at that revision. The workflow then enforced `build` -> `release`
-> `sign` -> `package` -> `publish`; documentation and readiness records
classify an unrun tagged Release matrix as an expected R1 `Unknown` instead of
a T6 failure. This moves
the Release-matrix schedule after the candidate source commit without inferring
Pass or weakening the publication gate. The 2026-08-21 impact note below
supersedes only the Authenticode portion of this historical baseline.

## Impact Note — 2026-08-21

The maintainer approved deferring Authenticode signing and timestamping from
WCRT 1.0.0. This changes a release security control and test obligation, but
does not change the C89/C99 API, implementation, ABI, supported targets, or
package contents. The 1.0 workflow no longer loads Azure signing actions or
requires an Authenticode identity. It still requires the exact-source Debug
matrix, native optimized x86/x64/ARM64 Release smoke tests, x86 legacy imports,
WPM package signing, `wpm verify` exit zero, unchanged packaged-DLL hashes,
exact artifact identities, and downstream-only publication. Existing unsigned
candidate results remain `NotSigned`; future Authenticode work is `Deferred`,
never inferred Pass. ADR-0006 and the signing plan are retained for later
reassessment.
