# C99 Compiler Capabilities and Windows Data Models

**Content type:** Implementation-defined behavior and compiler capability record

**Status:** T6 selected dependency baseline; corrected exact-candidate native
ARM64 evidence pending

**Selected compiler:** TinyCC `0.9.28-rc.1442+2474e1c2`, source revision
`d5c02f0fcdfdf75265d38df6ff9db2f8067367ac`, for i386 Windows, x86_64
Windows, and AArch64 Windows

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
| `wint_t` / signedness | 4 / unsigned | 4 / unsigned | 4 / unsigned |
| `mbstate_t` / alignment | 8 / 4 | 8 / 4 | 8 / 4 |
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
The 2-byte unsigned `wchar_t` width is a Windows ABI fact. ADR-0003 selects a
4-byte unsigned `wint_t` so the public type is unchanged by the integer
promotions and has a distinct `WEOF` value. This intentionally differs from
the Microsoft UCRT's 2-byte `wint_t`; Windows operating-system interoperability
uses `wchar_t`/`WCHAR`, not `wint_t`. `mbstate_t` has one target-invariant
8-byte, 4-byte-aligned ABI with the all-zero representation as its initial
state. ADR-0002's T2 bridge remains bound to the same `wchar_t` and C-locale
character behavior and is updated during T3 for the exact public `wint_t`.

## TinyCC C99 Facility Matrix

The following compile-only probes use `-std=c99 -Wall -Werror`. T5 qualified
TinyCC `0.9.28-rc.1441+0af32d51` on 2026-08-13. T6 requalified the selected
1442 package locally on x86/x64 and by ARM64-target compilation; exact native
ARM64 candidate evidence remains Unknown. ADR-0005 retains the older
controlled diagnostic matrix as a regression fallback.

| Facility | Result | Release interpretation |
| --- | --- | --- |
| `_Bool` | Supported | Required and enforced for REQ-0021 |
| `long long` | Supported | Required and enforced for REQ-0022 and REQ-0023 |
| `restrict` | Supported | Required and enforced for REQ-0025 |
| variadic macros | Supported | Available for later header design; no runtime claim |
| complex arithmetic/types | Supported with controlled ARM64 adaptation | Required on x86, x64, and ARM64; the complex runtime and all 66 exports become mandatory. TinyCC's ARM64 emitted operator calls and packaged helper entries use different private register conventions, so WCRT supplies the pinned bridge described below. |
| complex imaginary constants | Supported | Standard `fi`, `i`, and `Li` constants compile on all three targets |
| type-generic macros | Supported extension | Expanded `_Generic` type and single-evaluation probes pass; WCRT explicitly dispatches mixed arguments to avoid TinyCC's Windows `float + long double` conversion defect |
| `#pragma STDC FENV_ACCESS` | Compiler-blocked | TinyCC 1442 emits the retained `#pragma STDC ignored` diagnostic under `-Werror`; WCRT cannot claim pragma recognition |
| `#pragma STDC FP_CONTRACT` | Compiler-blocked | TinyCC 1442 emits the retained `#pragma STDC ignored` diagnostic under `-Werror`; WCRT cannot claim pragma recognition |
| `#pragma STDC CX_LIMITED_RANGE` | Compiler-blocked | TinyCC 1442 emits the retained `#pragma STDC ignored` diagnostic under `-Werror`; WCRT cannot claim pragma recognition |

Complex arithmetic remains a compiler dependency, but the selected TinyCC
1442 package passes the language gate. On ARM64 its call sites place the four
binary64 components in `x0`--`x3` and the result address in `x4`, while its
packaged `__tcc_muldc3`/`__tcc_divdc3` C entries expect `d0`--`d3` and `x0`.
WCRT's private assembly boundary moves those values and delegates to scaled C
helpers; Windows ARM64 `long double` aliases the same binary64 helpers. Static
consumers receive the bridge in `libwcrt.a`; DLL consumers additionally link
`libwcrt-tinycc-complex-abi.a`, while all public complex functions remain DLL
imports. This is selected-toolchain adaptation, not a public WCRT ABI change.
TC-0037/TC-0038 are ordinary required tests. ADR-0005 still defines the only
allowed ExpectedFail fallback if a selected compiler regresses to one of the
exact retained diagnostics; a different failure cannot use that disposition.

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

The exact local compiler executable SHA-256 values selected for the candidate
baseline are `2ba43fde75ee6795c83dafdd896b726b3d6c11d7c90c6e54a79aba9c300d5328`
(x86), `deea187e49165b6110e0d3719e01d11db5f1729e18179dbe5e48eae4f5f55d54`
(x64), and
`868d1c84426953f89a7720ae3a91ed04a127e57ae4b7fad8a44ae46ceef4d55c`
(ARM64). These identify local inputs; they do not substitute for retained
native target results.
