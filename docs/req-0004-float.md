# REQ-0004 — `<float.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.1.4 Limits `<float.h>` and `<limits.h>`

## Required files

- `include/float.h` — floating-point model and limits.
- `tests/c89/float.c` — compile-time values and runtime representation checks.

No runtime source file or public function is required by this header.

## Required macros

- Model: `FLT_RADIX`, `FLT_ROUNDS`.
- Precision: `FLT_DIG`, `DBL_DIG`, `LDBL_DIG`, `FLT_MANT_DIG`,
  `DBL_MANT_DIG`, `LDBL_MANT_DIG`.
- Exponent ranges: `FLT_MIN_EXP`, `DBL_MIN_EXP`, `LDBL_MIN_EXP`,
  `FLT_MAX_EXP`, `DBL_MAX_EXP`, `LDBL_MAX_EXP`, plus the corresponding
  base-10 `*_MIN_10_EXP` and `*_MAX_10_EXP` macros.
- Values: `FLT_MAX`, `DBL_MAX`, `LDBL_MAX`, `FLT_EPSILON`, `DBL_EPSILON`,
  `LDBL_EPSILON`, `FLT_MIN`, `DBL_MIN`, `LDBL_MIN`.

## Requirements

- Values shall describe the actual TinyCC/WCRT ABI, including any x86 x87
  extended-precision behavior documented by WCRT.
- Constant types and representations shall meet §4.1.4.
- Values shall satisfy the minimum environmental limits in §2.2.4.2.
- `FLT_ROUNDS` shall describe the implementation's addition rounding mode; any
  inability to track runtime rounding changes shall be documented.
- Inclusion shall not change the floating-point environment.

## Acceptance

Tests shall verify macro presence, types where observable, ordering constraints,
radix/exponent relationships, epsilon behavior, and consistency with actual
storage formats. The shared gates in `REQUIREMENTS.md` apply.

## Implementation record

- `include/float.h` defines the complete C89 model for TinyCC targets.
- On x64 Windows, runtime tests establish binary32 `float`, binary64 `double`,
  and binary64 `long double`; WCRT intentionally does not copy TinyCC's bundled
  header claim of x87 extended precision on that ABI.
- On 32-bit x86, TinyCC's x87 extended model is retained. Other target branches
  document their compiler model in the header.
- `tests/c89/float.c` validates environmental minima, ordering, exponent signs,
  positive ranges, and stored epsilon behavior.
