# REQ-0037 — C99 complex arithmetic

**Content type:** Project requirement

**Status:** Implementation baselined; TinyCC 1441 x86/x64 verification passes
and ARM64 compile/link verification passes; native ARM64 review remains open

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.3

## Scope

This requirement controls the C99 complex header, the required compiler-owned
types and constants, all 66 complex-math entry points, numerical behavior,
target ABI qualification, and C89 isolation. Optional imaginary types, Annex G
conformance, and `CMPLX` construction macros are outside scope.

### Required files

- `include/complex.h` — edition-selected macros and declarations.
- `src/complex.c` — self-contained complex mathematics implementation.
- `tests/c99/complex.c` and presence tests — inventory, numerical, special-
  value, representation, branch-cut, and isolation verification.
- `tools/generate-complex-vectors.py` and retained JSON — deterministic
  100-decimal-digit, half-even ordinary reference evidence.

## Requirement

- In C99 mode, `<complex.h>` shall define `complex` as `_Complex`, define
  `_Complex_I` as a constant expression of type `const float complex`, and
  define `I` as the available imaginary unit. It shall not simulate the C99
  type with a public structure.
- The header shall declare the `f`, unsuffixed, and `l` variants of `cacos`,
  `casin`, `catan`, `ccos`, `csin`, `ctan`, `cacosh`, `casinh`, `catanh`,
  `ccosh`, `csinh`, `ctanh`, `cexp`, `clog`, `cabs`, `cpow`, `csqrt`, `carg`,
  `cimag`, `conj`, `cproj`, and `creal`.
- Accessors, conjugation, and projection shall preserve required component
  values and signed zeros. Magnitude and argument shall use scaled magnitude
  and quadrant-sensitive real targets.
- Analytic functions shall return C99 principal values in radians and preserve
  the side of branch cuts through signed zero. Ordinary finite binary64
  components shall meet a scaled `1e-11` bound; binary32 shall meet `1e-5`.
  Windows `long double` uses the binary64 bound.
- Zero, subnormal, infinity, NaN, pole, overflow, and underflow partitions
  shall be explicit. Complex composition retains error reports from the T4
  real targets, including their C89-compatible choices; it does not add a
  second complex-specific `errno` policy. WCRT does not claim Annex G or
  automatic floating-exception reporting.
- The implementation shall not delegate to host complex-math functions. It
  may compose independently verified WCRT real-math targets.
- C99 declarations shall be absent when `WCRT_C89` selects C89 mode, and the
  existing C89 and earlier C99 surfaces shall remain unchanged.
- Complex representation and calling behavior shall be verified independently
  for x86, x64, and ARM64 before conformance is claimed.

## Rationale

Complex types and operators belong to the compiler, while the functions and
special-value policy belong to WCRT. Separating the implementation from the
compiler-capability disposition permits review and packaging without claiming
that a compiler-limited release can retain reviewable evidence without
substituting a nonconforming public type.

## Verification

**Method:** Compile-time inventory, exact component checks, controlled ordinary
vectors, branch and special-value partitions, metamorphic checks, ABI consumers,
and C89 regression

**References:** `TC-0037`

TC-0037 shall run normally once the selected compiler accepts both `_Complex`
operations and imaginary constants. Until then, only ADR-0005's exact,
source-specific type/parser and literal diagnostics may be classified
`ExpectedFail`; a different diagnostic or any post-capability test failure is
a release failure.

## Relationships

- **Derived from:** C99 §6.2.5, §6.4.4.2, §6.5, and §7.3
- **Depends on:** REQ-0024, REQ-0035, REQ-0036, and ADR-0005
- **Required by:** REQ-0038
- **Conflicts with:** A structure substitute represented as C99 complex

## Implementation Record

`include/complex.h` exposes the compiler-owned types and complete inventory.
`src/complex.c` implements exact manipulation, scaled magnitude and square
root, exponential/logarithmic foundations, power, forward and inverse
trigonometric and hyperbolic families, and three semantic precisions. The
TinyCC `0.9.28-rc.1441+0af32d51` compiles and executes TC-0037 natively on x86
and x64, compiles it for ARM64, and links complex calls through the static and
DLL interfaces on all three targets. Native ARM64 execution and independent
review remain open.
