# REQ-0036 — C99 floating-point environment

**Content type:** Project requirement

**Status:** Implemented locally; native ARM64 verification pending

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.6; x86, x64, and
ARM64 Windows floating-point control architecture

## Scope

This requirement controls WCRT's supported floating-point status flags,
rounding-direction modes, saved environments, public ABI, and target mapping.
It covers the C99 `<fenv.h>` interface and observable processor state; it does
not claim IEC 60559 traps, dynamic precision modes, or Annex F math behavior.

### Required files

- `include/fenv.h` — edition-selected public types, constants, and functions.
- `src/fenv.c` — architecture-specific access to the active thread's hardware
  floating-point environment.
- `tests/c99/fenv.c` and presence tests — ABI, flag, rounding, save/restore,
  update, hold, and C89-isolation verification.

## Requirement

- In selected C99 mode, `<fenv.h>` shall define `fenv_t`, `fexcept_t`,
  `FE_DIVBYZERO`, `FE_INEXACT`, `FE_INVALID`, `FE_OVERFLOW`, `FE_UNDERFLOW`,
  `FE_ALL_EXCEPT`, `FE_TONEAREST`, `FE_DOWNWARD`, `FE_UPWARD`,
  `FE_TOWARDZERO`, and `FE_DFL_ENV`.
- `fenv_t` shall be a 16-byte, 4-byte-aligned normalized WCRT object on x86,
  x64, and ARM64. It records the supported rounding mode, exception flags, and
  implementation control state without exposing native register layout.
  `fexcept_t` shall be an unsigned 32-bit flag object.
- The environment is the calling thread's processor state, not process-global
  software state. x86 and x64 shall reconcile the x87 control/status words and
  MXCSR; ARM64 shall reconcile FPCR and FPSR. Public values are target-invariant
  and the implementation shall translate them to native encodings.
- `feclearexcept`, `fegetexceptflag`, `feraiseexcept`, `fesetexceptflag`, and
  `fetestexcept` shall operate on any subset of `FE_ALL_EXCEPT` and preserve
  unselected flags. `fegetround` shall return the active supported mode;
  `fesetround` shall reject any other value without changing the environment.
- `fegetenv` shall capture the supported environment. `fesetenv` shall install
  a captured object or the WCRT default environment. `feholdexcept` shall save
  the environment, clear supported flags, and establish non-stop mode.
  `feupdateenv` shall save raised flags, install the supplied environment, then
  raise the saved flags in the installed environment.
- `FE_DFL_ENV` shall denote the WCRT startup profile: round to nearest, all
  supported exception flags clear, and exception traps masked. WCRT does not
  expose optional nonstandard precision or denormal-control modes.
- Floating arithmetic performed after `fesetround` shall demonstrate the
  selected direction on a volatile halfway or adjacent-value expression.
  Flags raised by volatile division, invalid arithmetic, overflow, underflow,
  and inexact operations shall be observable where the target instruction path
  supports them. Target differences shall be retained rather than inferred.
- `<fenv.h>` shall be self-contained and repeatable. Its entire public surface
  shall be absent when `WCRT_C89` selects C89 mode, and the runtime source shall
  remain compilable in the project's C89 implementation dialect.

## Rationale

Processor floating state is naturally per thread and affects real arithmetic.
A process-global shadow, as in the preliminary T4 implementation, can report a
rounding mode without changing any calculation and therefore cannot satisfy
the C99 contract. A normalized public ABI keeps all Windows targets stable
while allowing correct native register mappings.

## Verification

**Method:** Automated ABI and behavioral test, state-transition testing,
volatile arithmetic probes, target-specific compile/link, and personal review

**References:** `TC-0036`

TC-0036 shall verify type sizes and alignment; every flag alone and in
combinations; preservation of unselected flags; all rounding modes and invalid
mode rejection; arithmetic direction; capture, restore, hold, default, and
update transitions; null-independent valid use; C89 isolation; and native
x86/x64 behavior. ARM64 shall compile/link locally and execute in native CI
before target-complete acceptance.

## Relationships

- **Derived from:** C99 §7.6 and supported Windows processor architectures
- **Depends on:** REQ-0024 and the T0 compiler/ABI capability record
- **Required by:** REQ-0035 error and rounding verification
- **Conflicts with:** Process-global shadow state, silently ignored rounding
  control, target inference, or exposing a native-register ABI

## Tailoring

All five standard exception conditions and all four standard rounding
directions are supported. Exception traps remain masked by the WCRT default and
hold environments; trap delivery and nonstandard denormal/precision controls
are outside this requirement. Native ARM64 execution is an explicit retained
evidence gate.

## Implementation Record

`include/fenv.h` defines the target-invariant 16-byte environment and complete
C99 interface. `src/fenv.c` maps flags, rounding, save/restore, hold, default,
and update operations to x87 on x86, reconciled x87/MXCSR on x64, and translated
FPCR/FPSR state on ARM64. There is no process-global shadow state.

TC-0036 verifies ABI/alignment, all flags alone and combined, preservation of
unselected state, invalid rounding rejection, volatile arithmetic under every
rounding direction, hardware-generated exception flags, and every environment
transition. It passes natively on x86 and x64 and compiles/links against the
ARM64 system-register path. Native ARM64 execution and independent review
remain required before target-complete acceptance.
