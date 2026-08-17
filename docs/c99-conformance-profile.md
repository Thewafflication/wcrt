# WCRT C99 hosted-library conformance profile

**Content type:** Conformance evidence index

**Status:** T6 technical verification baseline; release decision blocked

**Standard baseline:** ISO/IEC 9899:1999 as corrected through TC3 (WG14
N1256)

**Source baseline:** `aea690d2caf108054d1644eefb176efde779f524`

## Reading the profile

Normative class and implementation disposition are independent. `required`
means the selected hosted C99 library profile requires the facility;
`implementation-defined` identifies a required choice that must also be
documented. `compiler-blocked` identifies a compiler-owned facility that WCRT
cannot implement in headers or runtime code. A target state is Pass only when
the cited evidence was produced from the exact profile baseline.
Exact-revision native x86/x64/ARM64 Debug results and the separately bounded
local Release results are summarized in
`docs/evidence/c99-t6/local-verification.md`. A facility Pass does not approve
the unsigned release candidate or erase a compiler-blocked ExpectedFail.

## Facility matrix

| ID | Clause | Header/facility | Normative class | Disposition | x86 | x64 | ARM64 | Requirement | Implementation | Test/evidence | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| F-001 | 7.1 | Library use, definitions, and `<stddef.h>` common types | required | implemented | Pass | Pass | Pass | REQ-0011; REQ-0042 | `include/stddef.h` | TC-0011; TC-0042 | Exact-revision native behavior passes on all targets. |
| F-002 | 7.1 | All 24 standard headers self-contained and repeatable | required | implemented | Pass | Pass | Pass | REQ-0042 | `include/` | TC-0042 | Mixed-order matrix is controlled by TC-0042. |
| F-003 | 7.1 | Reserved identifiers and function macro suppression | required | implemented | Pass | Pass | Pass | REQ-0038; REQ-0040 | `include/`; `src/` | TC-0038; TC-0040 | Personal and independent source review required. |
| F-004 | 7.2 | `<assert.h>` enabled and disabled assertions | required | implemented | Pass | Pass | Pass | REQ-0001; REQ-0040 | `include/assert.h`; `src/assert.c` | TC-0001; TC-0040 | C99 diagnostic includes enclosing function after T6 correction. |
| F-005 | 7.3 | `<complex.h>` types, operators, `complex`, `_Complex_I`, and `I` | required | implemented | Pass | Pass | Pass | REQ-0037 | `include/complex.h`; `src/platform/windows/tinycc_complex_abi.c`; `src/platform/windows/tinycc_complex_abi.S` | TC-0037; TC-0042 | Depends on target `_Complex` and imaginary literals; the selected ARM64 TinyCC package requires the controlled private helper-ABI bridge. |
| F-006 | 7.3 | Complex trigonometric and hyperbolic families | required | implemented | Pass | Pass | Pass | REQ-0037 | `src/complex.c` | TC-0037 | Ordinary, branch, and exceptional partitions. |
| F-007 | 7.3 | Complex exponential, logarithm, power, and square root | required | implemented | Pass | Pass | Pass | REQ-0037 | `src/complex.c` | TC-0037 | Accuracy is implementation-defined. |
| F-008 | 7.3 | Complex manipulation and component functions | required | implemented | Pass | Pass | Pass | REQ-0037 | `src/complex.c` | TC-0037 | Includes signed-zero projection behavior. |
| F-009 | 7.3 | Imaginary types and `imaginary`/`_Imaginary_I` | optional | compiler-blocked | N/A | N/A | N/A | REQ-0037 | `include/complex.h` | TC-0037 | Optional model omitted because TinyCC has no imaginary types. |
| F-010 | 7.3 | `CX_LIMITED_RANGE` standard pragma | required | compiler-blocked | ExpectedFail | ExpectedFail | ExpectedFail | REQ-0041 | TinyCC compiler | TC-0041 | Selected TinyCC emits the retained compiler-blocked diagnostic. |
| F-011 | 7.4 | `<ctype.h>` C locale character classification | required | implemented | Pass | Pass | Pass | REQ-0002; REQ-0040 | `include/ctype.h`; `src/ctype.c` | TC-0002; TC-0040 | Includes C99 `isblank`. |
| F-012 | 7.4 | Character case mapping | required | implemented | Pass | Pass | Pass | REQ-0002 | `src/ctype.c` | TC-0002 | Defined for EOF and unsigned-char values. |
| F-013 | 7.5 | `<errno.h>`, `errno`, `EDOM`, `ERANGE`, and `EILSEQ` | required | implemented | Pass | Pass | Pass | REQ-0003; REQ-0031 | `include/errno.h`; `src/errno.c` | TC-0003; TC-0031 | Thread-local behavior is not claimed. |
| F-014 | 7.6 | `<fenv.h>` types, flags, directions, and default environment | required | implemented | Pass | Pass | Pass | REQ-0036 | `include/fenv.h`; `src/fenv.c` | TC-0036; TC-0042 | Stable project ABI over target hardware state. |
| F-015 | 7.6 | Floating exception flag operations | required | implemented | Pass | Pass | Pass | REQ-0036 | `src/fenv.c` | TC-0036 | Native execution required. |
| F-016 | 7.6 | Rounding control and environment save/restore | required | implemented | Pass | Pass | Pass | REQ-0036 | `src/fenv.c` | TC-0036 | Native x87/SSE/FPCR behavior required. |
| F-017 | 7.6 | `FENV_ACCESS` standard pragma | required | compiler-blocked | ExpectedFail | ExpectedFail | ExpectedFail | REQ-0036; REQ-0041 | TinyCC compiler | TC-0036; TC-0041 | Default is off; selected TinyCC emits the retained compiler-blocked diagnostic. |
| F-018 | 7.7 | `<float.h>` C99 model and evaluation macros | implementation-defined | implemented | Pass | Pass | Pass | REQ-0024 | `include/float.h` | TC-0024; TC-0042 | Windows binary32/binary64 choices are documented. |
| F-019 | 7.8 | `<inttypes.h>` greatest-width types and functions | required | implemented | Pass | Pass | Pass | REQ-0027 | `include/inttypes.h`; `src/stdlib.c` | TC-0027 | Narrow and wide functions share checked cores. |
| F-020 | 7.8 | Integer print and scan macro families | required | implemented | Pass | Pass | Pass | REQ-0027 | `include/inttypes.h` | TC-0027; TC-0029; TC-0030 | Target data-model dependent spellings. |
| F-021 | 7.9 | `<iso646.h>` alternative operator spellings | required | implemented | Pass | Pass | Pass | REQ-0026 | `include/iso646.h` | TC-0026; TC-0042 | Edition-selected without runtime behavior. |
| F-022 | 7.10 | `<limits.h>` integer limits including `long long` | required | implemented | Pass | Pass | Pass | REQ-0005; REQ-0023 | `include/limits.h` | TC-0005; TC-0023 | Expression types are part of verification. |
| F-023 | 7.11 | `<locale.h>` categories, `setlocale`, and `localeconv` | required | implemented | Pass | Pass | Pass | REQ-0006; REQ-0040 | `include/locale.h`; `src/locale.c` | TC-0006; TC-0040 | Only the C locale is supported. |
| F-024 | 7.11 | `struct lconv` C99 international monetary members | required | implemented | Pass | Pass | Pass | REQ-0040 | `include/locale.h`; `src/locale.c` | TC-0040; TC-0042 | C89 prefix layout remains compatible. |
| F-025 | 7.11 | Locale names, encodings, and category effects | implementation-defined | implemented | Pass | Pass | Pass | REQ-0006; REQ-0041 | `src/locale.c`; `docs/implementation-defined.md` | TC-0006; TC-0041 | The empty locale name selects C. |
| F-026 | 7.12 | `<math.h>` classification and comparison macros | required | implemented | Pass | Pass | Pass | REQ-0035 | `include/math.h`; `src/math.c` | TC-0035 | Macro arguments evaluated once. |
| F-027 | 7.12 | Real trigonometric and hyperbolic families | required | implemented | Pass | Pass | Pass | REQ-0007; REQ-0035 | `src/math.c` | TC-0007; TC-0035 | Implementation-defined accuracy profile. |
| F-028 | 7.12 | Real exponential and logarithmic families | required | implemented | Pass | Pass | Pass | REQ-0035 | `src/math.c` | TC-0035 | Special/range behavior and ordinary vectors. |
| F-029 | 7.12 | Real power and absolute-value families | required | implemented | Pass | Pass | Pass | REQ-0007; REQ-0035 | `src/math.c` | TC-0007; TC-0035 | Includes `cbrt` and `hypot`. |
| F-030 | 7.12 | Error and gamma families | required | implemented | Pass | Pass | Pass | REQ-0035 | `src/math.c` | TC-0035 | `signgam` extension is not claimed. |
| F-031 | 7.12 | Nearest-integer functions | required | implemented | Pass | Pass | Pass | REQ-0035 | `src/math.c` | TC-0035; TC-0036 | Active rounding interactions require native evidence. |
| F-032 | 7.12 | Remainder and quotient functions | required | implemented | Pass | Pass | Pass | REQ-0035 | `src/math.c` | TC-0035 | Includes `remquo`. |
| F-033 | 7.12 | Floating manipulation functions | required | implemented | Pass | Pass | Pass | REQ-0035 | `src/math.c` | TC-0035 | Includes exact next/scaling operations. |
| F-034 | 7.12 | Maximum, minimum, and positive difference | required | implemented | Pass | Pass | Pass | REQ-0035 | `src/math.c` | TC-0035 | NaN and signed-zero partitions. |
| F-035 | 7.12 | Fused multiply-add family | required | implemented | Pass | Pass | Pass | REQ-0035 | `src/math.c`; `docs/c99-deviations.md` | TC-0035; `tests/c99/data/fma-vectors.json` | Exact integer accumulation and one active-direction target rounding; native vectors pass on all targets. |
| F-036 | 7.12 | `math_errhandling` and error reporting | implementation-defined | implemented | Pass | Pass | Pass | REQ-0035 | `include/math.h`; `src/math.c` | TC-0035 | WCRT selects `MATH_ERRNO`, not Annex F. |
| F-037 | 7.12 | `FP_CONTRACT` standard pragma | required | compiler-blocked | ExpectedFail | ExpectedFail | ExpectedFail | REQ-0041 | TinyCC compiler | TC-0041 | Default/off and pragma recognition require compiler evidence. |
| F-038 | 7.12 | IEC 60559 / Annex F binding | out-of-profile | not-applicable | N/A | N/A | N/A | REQ-0035; REQ-0041 | `docs/c99-deviations.md` | TC-0041 | WCRT makes no Annex F claim. |
| F-039 | 7.13 | `<setjmp.h>`, `setjmp`, and `longjmp` | required | implemented | Pass | Pass | Pass | REQ-0008; REQ-0042 | `include/setjmp.h`; `src/setjmp.c`; `src/platform/windows/setjmp.S` | TC-0008; TC-0042 | Calling-state behavior requires native target execution. |
| F-040 | 7.14 | `<signal.h>`, handlers, and `raise` | required | implemented | Pass | Pass | Pass | REQ-0009 | `include/signal.h`; `src/signal.c` | TC-0009 | Project process-local signal model. |
| F-041 | 7.14 | Signal values and handler semantics | implementation-defined | implemented | Pass | Pass | Pass | REQ-0009; REQ-0041 | `src/signal.c`; `docs/implementation-defined.md` | TC-0009 | Supported values are documented. |
| F-042 | 7.15 | `<stdarg.h>` variable argument traversal | required | implemented | Pass | Pass | Pass | REQ-0010 | `include/stdarg.h` | TC-0010; TC-0042 | ABI-sensitive on all targets. |
| F-043 | 7.15 | `va_copy` independent traversal | required | implemented | Pass | Pass | Pass | REQ-0025 | `include/stdarg.h` | TC-0025 | Compiler builtin contract. |
| F-044 | 7.16 | `<stdbool.h>` Boolean macros | required | implemented | Pass | Pass | Pass | REQ-0021 | `include/stdbool.h` | TC-0021; TC-0042 | Compiler owns `_Bool`. |
| F-045 | 7.17 | `<stddef.h>` `size_t`, `ptrdiff_t`, `wchar_t`, `NULL`, and `offsetof` | required | implemented | Pass | Pass | Pass | REQ-0011; REQ-0033 | `include/stddef.h` | TC-0011; TC-0033; TC-0042 | Target ABI and constant expression checked. |
| F-046 | 7.18 | `<stdint.h>` exact, minimum, fast, pointer, and greatest types | required | implemented | Pass | Pass | Pass | REQ-0022 | `include/stdint.h` | TC-0022; TC-0042 | Optional exact widths are provided. |
| F-047 | 7.18 | Integer limits and constant-expression macros | required | implemented | Pass | Pass | Pass | REQ-0022 | `include/stdint.h` | TC-0022 | Exact expression types are checked. |
| F-048 | 7.19 | `<stdio.h>` streams, orientation, buffering, and lifecycle | required | implemented | Pass | Pass | Pass | REQ-0012; REQ-0031 | `include/stdio.h`; `src/stdio.c` | TC-0012; TC-0031 | Static and DLL consumers required. |
| F-049 | 7.19 | File operations and temporary files | required | implemented | Pass | Pass | Pass | REQ-0012 | `src/stdio.c`; `src/platform/windows/file.c` | TC-0012 | Windows 2000 imports required on x86. |
| F-050 | 7.19 | Narrow formatted output including C99 conversions | required | implemented | Pass | Pass | Pass | REQ-0019; REQ-0029 | `src/stdio_format.c` | TC-0019; TC-0029; TC-0039 | Bounded and hexadecimal paths included. |
| F-051 | 7.19 | Narrow formatted input including C99 conversions | required | implemented | Pass | Pass | Pass | REQ-0030 | `src/stdio_scan.c` | TC-0030; TC-0039 | Retained-input behavior included. |
| F-052 | 7.19 | Character, line, and direct block I/O | required | implemented | Pass | Pass | Pass | REQ-0012 | `src/stdio.c` | TC-0012 | Text/binary distinctions are implementation-defined. |
| F-053 | 7.19 | File positioning, status, and error reporting | required | implemented | Pass | Pass | Pass | REQ-0012 | `src/stdio.c` | TC-0012 | `fpos_t` representation is documented. |
| F-054 | 7.20 | `<stdlib.h>` numeric conversions | required | implemented | Pass | Pass | Pass | REQ-0013; REQ-0028; REQ-0039 | `src/stdlib.c` | TC-0013; TC-0028; TC-0039 | Hex rounding mandatory; decimal accuracy documented separately. |
| F-055 | 7.20 | Decimal conversion accuracy through `DECIMAL_DIG` | recommended-practice | optional-omitted | N/A | N/A | N/A | REQ-0039 | `src/stdlib.c`; `docs/implementation-defined.md` | TC-0039 | Universal correct rounding is not claimed; the known counterexample is retained. |
| F-056 | 7.20 | `MB_CUR_MAX` and multibyte conversion state | required | implemented | Pass | Pass | Pass | REQ-0040; REQ-0031 | `include/stdlib.h`; `src/wchar_convert.c` | TC-0031; TC-0040 | C locale uses single-byte ASCII. |
| F-057 | 7.20 | Pseudorandom sequence | implementation-defined | implemented | Pass | Pass | Pass | REQ-0013; REQ-0041 | `src/stdlib.c`; `docs/implementation-defined.md` | TC-0013 | Algorithm and `RAND_MAX` documented. |
| F-058 | 7.20 | Memory allocation and deallocation | required | implemented | Pass | Pass | Pass | REQ-0013 | `src/stdlib.c`; `src/platform/windows/heap.c` | TC-0013 | Overflow and failure paths required. |
| F-059 | 7.20 | Environment, termination, and `_Exit` | required | implemented | Pass | Pass | Pass | REQ-0013; REQ-0028 | `src/stdlib.c`; `src/platform/windows/process.c` | TC-0013; TC-0028 | Startup/atexit integration required. |
| F-060 | 7.20 | Search and sort utilities | required | implemented | Pass | Pass | Pass | REQ-0013 | `src/stdlib.c` | TC-0013 | Comparator and zero-count partitions. |
| F-061 | 7.20 | Integer absolute value and division families | required | implemented | Pass | Pass | Pass | REQ-0013; REQ-0028 | `src/stdlib.c` | TC-0013; TC-0028 | Includes long-long forms. |
| F-062 | 7.21 | `<string.h>` copy, concatenate, compare, and collation | required | implemented | Pass | Pass | Pass | REQ-0014; REQ-0025 | `include/string.h`; `src/string.c` | TC-0014; TC-0025 | C locale collation equals byte comparison. |
| F-063 | 7.21 | String search, tokenization, and error strings | required | implemented | Pass | Pass | Pass | REQ-0014 | `src/string.c` | TC-0014 | Unsigned-byte ordering required. |
| F-064 | 7.21 | Memory copy, move, compare, search, and set | required | implemented | Pass | Pass | Pass | REQ-0014; REQ-0025 | `src/string.c` | TC-0014; TC-0025 | Restrict contracts and overlap partitions. |
| F-065 | 7.22 | `<tgmath.h>` real/complex type-generic macros | required | implemented | Pass | Pass | Pass | REQ-0038 | `include/tgmath.h` | TC-0038 | Uses compiler `_Generic` support and single evaluation. |
| F-066 | 7.23 | `<time.h>` clock and calendar functions | required | implemented | Pass | Pass | Pass | REQ-0015 | `include/time.h`; `src/time.c`; `src/platform/windows/clock.c` | TC-0015; TC-0042 | Epoch, ranges, and timezone choices are documented. |
| F-067 | 7.23 | `strftime` complete C99 conversions and modifiers | required | implemented | Pass | Pass | Pass | REQ-0015; REQ-0040 | `src/time.c` | TC-0015; TC-0040 | Includes C99 C/D/F/g/G/h/n/r/R/t/T/u/V and E/O. |
| F-068 | 7.24 | `<wchar.h>` wide I/O, orientation, and formatted families | required | implemented | Pass | Pass | Pass | REQ-0031; REQ-0033 | `include/wchar.h`; `src/wchar_io.c`; `src/wchar_format.c`; `src/wchar_scan.c` | TC-0031; TC-0033 | Windows UTF-16 ABI. |
| F-069 | 7.24 | Wide numeric conversions | required | implemented | Pass | Pass | Pass | REQ-0027; REQ-0031; REQ-0039 | `src/wchar_convert.c` | TC-0027; TC-0031; TC-0039 | Checked adaptation of narrow conversion cores. |
| F-070 | 7.24 | Wide strings and memory operations | required | implemented | Pass | Pass | Pass | REQ-0031 | `src/wchar_string.c` | TC-0031 | Complete family inventory. |
| F-071 | 7.24 | Wide time formatting | required | implemented | Pass | Pass | Pass | REQ-0031 | `src/wchar_time.c`; `src/time.c` | TC-0031; TC-0015 | Narrow/wide conversion agreement required. |
| F-072 | 7.24 | Restartable multibyte conversion families | required | implemented | Pass | Pass | Pass | REQ-0031; REQ-0033 | `src/wchar_convert.c` | TC-0031; TC-0033; TC-0042 | Target-invariant `mbstate_t`. |
| F-073 | 7.24 | `wchar_t`, `wint_t`, `WEOF`, and encoding model | implementation-defined | implemented | Pass | Pass | Pass | REQ-0033; REQ-0041 | `include/wchar.h`; `docs/implementation-defined.md` | TC-0033; TC-0042 | UTF-16 code units; not a Unicode scalar-value type. |
| F-074 | 7.25 | `<wctype.h>` wide classification functions | required | implemented | Pass | Pass | Pass | REQ-0032 | `include/wctype.h`; `src/wctype.c` | TC-0032 | Exhaustive C-locale classification. |
| F-075 | 7.25 | Wide descriptors and mappings | required | implemented | Pass | Pass | Pass | REQ-0032 | `src/wctype.c` | TC-0032 | Invalid descriptors return documented neutral results. |

## Blocking summary

- Exact-revision native x86/x64/ARM64 Debug behavior, ABI, consumers, and
  startup results pass in run `32027269426`. Native ARM64 Release execution is
  still Unknown and is not inferred from the Debug or local compile/link result.
- F-010, F-017, and F-037 are compiler-blocked ExpectedFail results for the
  selected TinyCC package; they are not Pass.
- F-055 is omitted recommended practice. The retained decimal counterexample
  prevents a universal correct-rounding claim.
- Release readiness is Reject while independent review, native ARM64 Release,
  signing/timestamp, final-byte Defender, WPM signature verification, and
  installation/rollback gates are incomplete.
