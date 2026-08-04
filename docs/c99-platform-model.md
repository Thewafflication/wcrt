# C99 Compiler Capabilities and Windows Data Models

**Content type:** Implementation-defined behavior and compiler capability record

**Status:** Implemented T0 baseline

**Baseline compiler observed locally:** TinyCC 0.9.28rc for i386 Windows,
x86_64 Windows, and AArch64 Windows

## Scope

This record defines the data models used by the implemented WCRT C89 and early
C99 headers and records compiler-language capabilities needed by later C99
tranches. It does not claim that a compiler capability supplies the
corresponding C99 library header or runtime implementation.

CI regenerates `tinycc-c99-capabilities.json` with the exact packaged compiler
identity for each target. A compiler update must re-run these probes before its
results replace this observed baseline.

## Windows Data Models

All sizes are bytes and are verified by compile-time assertions in
`tests/c99/capabilities/data-model.c`.

| Type or property | x86 | x64 | ARM64 |
| --- | ---: | ---: | ---: |
| Model | ILP32 | LLP64 | LLP64 |
| `char` / plain-char signedness | 1 / signed | 1 / signed | 1 / signed |
| `short` | 2 | 2 | 2 |
| `int` | 4 | 4 | 4 |
| `long` | 4 | 4 | 4 |
| `long long` | 8 | 8 | 8 |
| object pointer | 4 | 8 | 8 |
| `size_t` / `ptrdiff_t` | 4 / 4 | 8 / 8 | 8 / 8 |
| `_Bool` | 1 | 1 | 1 |
| `wchar_t` | 2 | 2 | 2 |
| `float` | 4 | 4 | 4 |
| `double` | 8 | 8 | 8 |
| `long double` | 8 | 8 | 8 |

The x86 model is the only Windows 2000 compatibility target. The x64 model
applies to later 64-bit Windows releases, and the ARM64 model applies to
Windows 10 and 11. WCRT does not infer Windows 2000 support from shared scalar
widths on x64 or ARM64.

WCRT uses the Windows binary64 ABI for `long double` on all three targets.
Consequently `LDBL_MANT_DIG` is 53, `DECIMAL_DIG` is 17,
`FLT_EVAL_METHOD` is 0, and the default documented `FLT_ROUNDS` value is 1.
The 2-byte `wchar_t` width is an ABI fact only; the complete `wint_t`, UTF-16,
multibyte-state, and wide-library contract remains planned under REQ-0031 and
REQ-0033.

## TinyCC C99 Facility Matrix

The following compile-only probes use `-std=c99 -Wall -Werror`. Results are
the same for the three observed TinyCC targets.

| Facility | Result | Release interpretation |
| --- | --- | --- |
| `_Bool` | Supported | Required and enforced for REQ-0021 |
| `long long` | Supported | Required and enforced for REQ-0022 and REQ-0023 |
| `restrict` | Supported | Required and enforced for REQ-0025 |
| variadic macros | Supported | Available for later header design; no runtime claim |
| complex arithmetic | Unsupported | TinyCC reports `_Complex is not yet supported`; compiler-blocked input to T5 |
| type-generic macros | Supported extension | TinyCC accepts `_Generic` in C99 mode; complete `<tgmath.h>` dispatch and single evaluation remain unverified until T5 |

Complex arithmetic is therefore a known dependency risk, not a passing C99
complex-library result. T5 must not be represented as implementable on this
compiler baseline without a compiler change, an approved alternative design
consistent with C99, or an explicit compiler-blocked/deviation disposition.

## Edition Selection

TinyCC 0.9.28rc defines a later `__STDC_VERSION__` value even during the
project's `-std=c89` compilation. WCRT public headers therefore use the
explicit `WCRT_C89` selector in addition to `__STDC_VERSION__`. TC-0021 through
TC-0026 directly verify that their C99 macros, typedefs, and qualifiers do not
leak into the selected C89 surface.

## Evidence and Revalidation

- Runner: `tools/test-tinycc-capabilities.ps1`
- Probe sources: `tests/c99/capabilities/`
- Per-target evidence: `output/test-results/<architecture>/tinycc-c99-capabilities.json`
- Native behavioral evidence: the controlled aggregate and C89 suites on the
  corresponding CI runner

Any change to supported TinyCC packages, target ABI macros, fundamental type
definitions, `float.h`, `stddef.h`, or `stdint.h` requires this record and all
three target results to be reviewed together.

