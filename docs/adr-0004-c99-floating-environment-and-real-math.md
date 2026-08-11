# ADR-0004 — C99 floating environment and real-math model

**Status:** Accepted for T4 implementation

**Date:** 2026-08-11

## Context

WCRT supports TinyCC on Windows x86, x64, and ARM64. REQ-0024 fixes `float` as
binary32 and both `double` and `long double` as binary64, but T4 also needs a
stable public `fenv_t`, actual per-thread rounding and flag behavior, and
single-evaluation type-sensitive math macros. Native register layouts and
encodings differ between x87/MXCSR and ARM64 FPCR/FPSR.

## Decision

- `fenv_t` is a normalized 16-byte, 4-byte-aligned WCRT object containing
  supported control, status, and reserved words; it is not a native save-area
  dump. `fexcept_t` is unsigned 32-bit.
- The implementation reads and writes the calling thread's hardware state.
  x86/x64 reconcile x87 and MXCSR flags and rounding; ARM64 translates the same
  public constants to FPCR/FPSR. The default is nearest, clear flags, masked
  traps.
- WCRT exposes all five C99 exception conditions and all four rounding modes,
  but not traps, x87 precision selection, flush-to-zero, denormal-are-zero, or
  other nonstandard modes.
- `math_errhandling` is `MATH_ERRNO`. The math library is not represented as
  Annex F conforming and does not promise automatic hardware flags for every
  error path.
- C99 inquiry and comparison macros dispatch to WCRT helper functions so every
  argument is evaluated once. The Windows binary64 `long double` functions are
  distinct ABI entry points implemented from the reviewed binary64 core.
- Exact representation operations are bit-based. Ordinary transcendental
  functions retain WCRT's explicit tolerance model and do not delegate to a
  host CRT.

## Consequences

The public environment ABI is identical across targets and can evolve through
reserved words, while native implementation code remains architecture-specific.
Any additional control mode requires an ADR and ABI impact review. Native
execution is required to close each architecture; cross-compilation cannot
prove register behavior. The math contract is complete C99 real surface with a
documented accuracy/error profile, not an IEC 60559 Annex F claim.
