# WCRT C99 implementation-defined behavior

**Content type:** Product conformance record

**Status:** T6 working baseline

This record documents choices for the hosted WCRT C99 library profile. It does
not claim IEC 60559 Annex F conformance.

## Data and floating-point model

- x86 uses ILP32; x64 and ARM64 use LLP64. `char` is signed, `wchar_t` is a
  16-bit UTF-16 code unit, `wint_t` is unsigned 32-bit, and `time_t` is signed
  64-bit. `long double` has the binary64 representation and value set.
- `float` is binary32. `double` and `long double` are binary64. Ordinary real
  and complex transcendental accuracy is the controlled tolerance documented
  by REQ-0035 and REQ-0037, not correct rounding.
- `math_errhandling` is `MATH_ERRNO`. Annex F automatic exception behavior is
  not claimed. The startup rounding mode is nearest, ties to even.
- Hexadecimal `strtof`, `strtod`, and `strtold` are rounded directly to the
  target format using nearest, ties to even. Decimal conversion retains the
  first 19 significant digits and applies binary64 power-of-ten scaling;
  decimal `strtof` then converts that binary64 result to binary32. TC-0039
  bounds tested behavior but does not establish universal correct rounding.
  The input
  `1.84420264470505631627654034273169716465594916268499065920945e-81`
  converts to `0x2f2bfd59a7dd4b00`; exact nearest-even conversion is
  `0x2f2bfd59a7dd4b01`. This retained one-ULP counterexample documents why
  C99's decimal recommended practice is not claimed.

## Locale, characters, and multibyte text

- The only locale is `C`; both `"C"` and the empty locale name select it.
  Locale collation is unsigned byte/code-unit lexical order.
- The execution narrow character set and multibyte encoding are single-byte
  ASCII for the implemented C-locale operations. `MB_CUR_MAX` is `(size_t)1`.
- Wide characters are Windows UTF-16 code units. Surrogate validation or
  Unicode normalization is not implied by the standard wide-character API.
- Monetary fields unsupported by the C locale contain an empty string or
  `CHAR_MAX`, as specified for unavailable information.

## I/O, files, time, and environment

- Text streams use Windows file handles and CR/LF adaptation where the
  controlled stream implementation specifies it. `fpos_t` follows WCRT's
  signed 64-bit file-position model.
- Calendar time is seconds since the Unix epoch in signed 64-bit `time_t`.
  The supported locale has fixed English abbreviations. The current platform
  adapter supplies local/UTC behavior; exact target tests are required.
- `system` and environment access use documented Windows process/environment
  APIs. No host C runtime is imported.
- The pseudorandom generator and its sequence are WCRT-defined with
  `RAND_MAX == 32767`; TC-0013 controls repeatability for a fixed seed.

## Signals and compiler-owned behavior

- WCRT supports the signal constants and process-local handler semantics
  implemented by `src/signal.c`; TC-0009 is the authoritative behavior set.
- TinyCC owns `_Bool`, `_Complex`, imaginary literals, generic-selection
  parsing, evaluation methods, and the three `STDC` pragmas. Exact selected
  compiler results are recorded separately; an unavailable compiler behavior
  is not supplied by the runtime and is not a Pass.
- The retained TinyCC 1442 baseline emitted private complex multiplication and
  division helper calls through a compiler-defective integer-register ABI.
  TinyCC 1444 corrected those calls to the standard Windows ARM64 function ABI:
  scalar components in `d0`--`d3` and the result pointer in `x0`. WCRT's
  `tinycc_complex_abi.S` now preserves those registers and supplies only the
  compiler-private helper symbol aliases; it does not alter the public complex
  ABI. ARM64 DLL consumers link the packaged
  `libwcrt-tinycc-complex-abi.a` companion in addition to `wcrt.def`.
