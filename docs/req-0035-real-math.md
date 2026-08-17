# REQ-0035 — C99 real mathematics

**Content type:** Project requirement

**Status:** T6 fused-operation correction implemented; exact-revision native
target verification and independent review pending

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.12

## Scope

This requirement extends the existing C89 mathematics unit with the complete
C99 real-mathematics surface. It controls declarations, type-generic inquiry
macros, all `float`, `double`, and `long double` function families, ordinary
accuracy, special values, error reporting, and C89 isolation. Complex and
type-generic mathematics remain REQ-0037 and REQ-0038.

### Required files

- `include/math.h` — C89 declarations plus the edition-selected C99 macros and
  complete real-function declarations.
- `src/math.c` — self-contained real-math implementation without host-CRT
  delegation.
- `tests/c99/math.c` and presence tests — function-inventory, numerical,
  special-value, macro, error, ABI, and isolation verification.

## Requirement

- In selected C99 mode, `<math.h>` shall define `HUGE_VALF`, `HUGE_VALL`,
  `INFINITY`, `NAN`, `FP_INFINITE`, `FP_NAN`, `FP_NORMAL`, `FP_SUBNORMAL`,
  `FP_ZERO`, `FP_ILOGB0`, `FP_ILOGBNAN`, `MATH_ERRNO`, `MATH_ERREXCEPT`, and
  `math_errhandling`. WCRT shall report errors through `errno` and therefore
  define `math_errhandling` as `MATH_ERRNO`; it does not claim IEC 60559 Annex F
  behavior or automatic `MATH_ERREXCEPT` reporting by every math function.
- `fpclassify`, `isfinite`, `isinf`, `isnan`, `isnormal`, and `signbit` shall
  classify binary32 and binary64 values, including quiet NaNs, both infinities,
  signed zeros, and subnormals. `isgreater`, `isgreaterequal`, `isless`,
  `islessequal`, `islessgreater`, and `isunordered` shall implement unordered
  comparison semantics. Every macro argument shall be evaluated exactly once.
- The header and runtime shall provide every C99 real function in the
  trigonometric, hyperbolic, exponential/logarithmic, power/absolute-value,
  error/gamma, rounding, remainder, manipulation, maximum/minimum/difference,
  and fused-operation groups, with the required `f`, unsuffixed, and `l`
  variants and prescribed integer result types.
- The Windows ABI model is binary32 `float` and binary64 `double` and
  `long double` on x86, x64, and ARM64. Long-double entry points shall remain
  distinct linkable functions, but their value set and accuracy contract are
  the binary64 contract recorded by REQ-0024.
- Exact-representation operations such as classification, sign copying,
  adjacent-value traversal, splitting, scaling within range, and integral
  rounding shall be verified with exact bit patterns. Ordinary transcendental
  results shall target 12 significant decimal digits for binary64 and 5 for
  binary32 over the controlled vector range. Tests shall use scaled tolerances
  and shall not imply correctly rounded transcendental results.
- NaNs, infinities, signed zero, subnormals, poles, domain errors, and
  representational overflow or underflow shall follow the documented C99
  special-case partition. Where C99 permits implementation choice, WCRT shall
  preserve the existing C89 error behavior and document that choice.
- Implemented functions shall not delegate to a host CRT. Internal composition
  from other WCRT math primitives is permitted when the resulting error and
  special-value behavior is tested.
- The existing 22-function C89 surface, finite C89 `HUGE_VAL` choice, errno
  behavior, and TC-0007 results shall remain unchanged. C99-only declarations
  and macros shall be absent when `WCRT_C89` selects the C89 surface.

## Rationale

The compiler and Windows ABI provide IEEE binary representations but do not
supply WCRT's library contract. A complete, edition-selected surface with
explicit accuracy and special-case rules prevents a header-only claim and
keeps the older C89 behavior stable.

## Verification

**Method:** Automated compile-time and behavioral test, controlled reference
vectors, exact bit-pattern checks, boundary analysis, and personal review

**References:** `TC-0035`

TC-0035 shall compile every declaration and macro; prove single evaluation;
exercise every function family in each semantic type; classify normal,
subnormal, zero, infinite, and NaN representations; check signed-zero and
adjacent-value direction; test ordinary reference vectors, poles, domain and
range partitions; and rerun TC-0007. Native x86, x64, and ARM64 execution is
required for target-complete acceptance; ARM64 compile/link is only a local
preliminary gate.

## Relationships

- **Derived from:** C99 §7.12
- **Depends on:** REQ-0004, REQ-0007, REQ-0024, REQ-0036, and the T0 Windows
  floating model
- **Required by:** REQ-0037 and REQ-0038
- **Conflicts with:** Host-CRT delegation, a wider Windows `long double` claim,
  multiple-evaluation inquiry macros, or an Annex F claim without evidence

## Tailoring

WCRT does not claim IEC 60559 Annex F conformance, correctly rounded
transcendental functions, alternate NaN payload parsing, or automatic hardware
exception reporting from every math function. `nan`, `nanf`, and `nanl` return
a quiet NaN and ignore the tag string. The retained C89 pole/error choices are
documented compatibility behavior.

## Implementation Record

`include/math.h` now declares the complete C99 real-math inventory and routes
all inquiry/comparison macros through binary32/binary64 helpers with one
evaluation per argument. `src/math.c` uses bit-level classification, sign,
splitting, scaling, adjacent-value, and remainder operations; compensated and
scaled arithmetic for `fma`; series and iterative cores for the elementary
functions; an incomplete-gamma evaluation for `erf`/`erfc`; and a Lanczos
evaluation for `lgamma`/`tgamma`. Float results pass through a binary32 range
gate, and long-double entry points use the accepted Windows binary64 model.

TC-0035 compiles and links the full function inventory, proves macro single
evaluation, checks exact normal/subnormal/zero/infinity/NaN bit partitions,
exercises every function group, and reruns TC-0007. It passes natively on x86
and x64 and compiles/links for ARM64. The x86/x64 C89 and extension aggregates,
all three Release builds, native consumer/startup tests, ARM64 consumer link,
and Windows 2000 x86 import gate pass locally.

T6 replaces the compensated `fma` approximation with fixed-size exact integer
accumulation spanning every supported binary64 product/addend bit, followed by
one binary32 or binary64 rounding under the active direction. `fmaf` rounds
directly to binary32; Windows `fmal` uses the binary64 model. TC-0035 retains
hand-selected cancellation/direction cases and 260 reproducible exact-rational
binary64 vectors in `tests/c99/data/fma-vectors.json`. These pass on local x86
and x64 and compile/link on ARM64. Exact-revision native ARM64 and independent
review remain required; older GitHub Actions run `31463268579` does not prove
the T6 correction.
