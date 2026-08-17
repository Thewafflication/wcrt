# REQ-0042 — C99 compatibility validation

**Content type:** Project requirement

**Status:** Baselined for T6; matrix verification pending

**Source:** ISO/IEC 9899:1999 as corrected through TC3, clauses 7.1.2 and
7.1.4; WCRT supported-platform policy

## Scope

This requirement controls the whole-product compatibility and evidence matrix
for the C99 exit gate on Windows x86, x64, and ARM64.

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
- Debug and Release static/DLL builds, consumers, startup objects, package
  assembly/verification, x86 Windows 2000 imports, source quality,
  traceability, and retained evidence validation shall pass at one exact source
  and dependency baseline.
- Native execution is required on every supported architecture. A cross-build,
  PE inspection, missing artifact, or unavailable external check shall be
  `Unknown` or `Blocked`, never Pass.

## Verification

**Method:** Generated header matrix, compile-time ABI assertions, full serial
regression, native target execution, build/consumer/package checks, evidence
validation, and independent audit

**References:** `TC-0042`

TC-0042 is the controlled local matrix entry point. Exact-revision CI supplies
native target evidence that cannot be produced locally. The profile and release
records shall preserve the distinction.

## Relationships

- **Depends on:** REQ-0040, REQ-0041, REQ-0016 through REQ-0018, and every
  controlled C89/C99/compatibility test
- **Required by:** WCRT 1.0.0 release-readiness approval
- **Conflicts with:** Parallel shared-output test execution or inferred target
  success
