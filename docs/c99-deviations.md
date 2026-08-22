# WCRT C99 deviations and excluded profiles

**Content type:** Deviation register

**Status:** No open project-code deviation; compiler-blocked items remain

## DEV-C99-0001 — Fused multiply-add universal single rounding

- **Requirement:** REQ-0035, C99 7.12.13.1
- **Affected implementation:** `fma`, `fmaf`, and `fmal` in `src/math.c`
- **State:** Corrected in T6; exact-revision native Debug target verification
  passes; independent review pending
- **Correction:** Fixed-size integer magnitudes now accumulate the complete
  exact product and addend across the supported exponent range before one
  active-direction rounding. Binary32 is rounded directly and Windows
  `long double` follows binary64.
- **Evidence:** TC-0035 includes cancellation, overflow-avoidance, subnormal,
  binary32, four-direction, and 260 reproducible exact-rational binary64
  vectors. Exact-revision native x86/x64/ARM64 Debug behavior passes in run
  `32027269426`. The deviation is removed from the implementation disposition;
  tagged native ARM64 Release remains an R1 prepublication gate rather than a
  conformance deviation.

## Compiler-owned blocking items

TinyCC 0.9.28rc emits the controlled `#pragma STDC ignored` diagnostic under
`-Wall -Werror` for `FENV_ACCESS`, `FP_CONTRACT`, and `CX_LIMITED_RANGE` in the
local x86/x64/ARM64 package family. Exact executable identities and retained
target JSON remain part of T6 verification. These are compiler-blocked, not
silent runtime deviations. Optional imaginary types are omitted because the
selected compiler does not provide them.

## Resolved selected-compiler adaptation

Native ARM64 run `32020695485` proved that TinyCC 1442 complex multiplication
called its packaged `__tcc_muldc3` with a different private register convention
than the helper entry expects. WCRT now supplies a target-scoped bridge and
scalar helpers in the static library and ARM64 DLL companion archive. This is
not registered as a public-library deviation: the public C99 representation,
function ABI, and results remain required. Corrected native ARM64 Debug
execution passes in exact-revision run `32027269426`; tagged native ARM64
Release smoke is separately an R1 Unknown until the release workflow runs.

## Profiles not claimed

WCRT 1.0.0 does not claim IEC 60559 Annex F binding, Annex G imaginary-type
support, correctly rounded transcendental functions, locales other than C, or
a `long double` format wider than Windows binary64.
