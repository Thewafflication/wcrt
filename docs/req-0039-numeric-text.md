# REQ-0039 — Numeric text interchange

**Content type:** Project requirement

**Status:** T2 narrow allocation implemented; native ARM64 verification pending

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.19.6 and §7.20.1.3

## Scope

The T2 allocation controls narrow hexadecimal floating syntax, infinity, NaN,
signed zero, rounding, range errors, and integration among `strtof`, `strtod`,
`strtold`, formatted output, and formatted input. It applies to binary32
`float` and the Windows binary64 `double`/`long double` model in the C locale.

Wide numeric text, additional locales, public C99 classification macros,
floating-environment control, and the final clause-level integration audit
remain allocated to later tranches.

### Required files

- `src/stdlib.c` and internal numeric-text implementation — string conversion.
- `src/stdio_format.c` and `src/stdio_scan.c` — formatted-I/O integration.
- `tests/c99/numeric-text.c` — exact, boundary, and cross-interface checks.

## Requirement

- In addition to the decimal behavior already controlled by REQ-0028,
  `strtof`, `strtod`, and `strtold` shall accept an optional sign followed by a
  `0x`/`0X` hexadecimal subject with an optional binary exponent,
  `INF`/`INFINITY` ignoring case, or `NAN` with an optional accepted payload
  sequence.
- The functions shall consume the longest valid subject sequence, store the
  first unrecognized character through a nonnull end pointer, and store the
  original input pointer when no conversion occurs.
- Hexadecimal conversion shall be correctly rounded. WCRT shall use its
  documented round-to-nearest, ties-to-even model for binary32 and binary64,
  and hexadecimal `strtof` shall avoid a binary64 intermediate and consequent
  double rounding.
- A negative subject that converts to zero shall produce negative zero. An
  infinity subject shall produce signed infinity, and an accepted NaN subject
  shall produce a quiet NaN. WCRT accepts but does not preserve NaN payload
  text.
- On overflow, the functions shall return the signed target range value
  selected by WCRT's `HUGE_VAL*` model and store `ERANGE` in `errno`. On
  underflow, they shall return the correctly rounded subnormal or signed zero
  and store `ERANGE` when a nonzero mathematical result is subnormal or zero.
- Formatted hexadecimal input and output shall use the same classification,
  rounding, sign, and range model. A finite value written with the default
  `%a` precision shall round-trip exactly through the corresponding input
  conversion.
- Successful finite conversions that do not overflow or underflow shall not
  change `errno`.

## Rationale

Hexadecimal text is the exact interchange form for the supported binary
formats. A shared numeric model prevents `strto*`, `printf`, and `scanf` from
disagreeing at signed-zero, subnormal, maximum-finite, infinity, and NaN
boundaries.

## Verification

**Method:** Automated behavioral test, exact-vector analysis, and review

**References:** `TC-0039`

TC-0039 shall use exact bit patterns and adjacent boundary values for positive
and negative zero, minimum subnormal, minimum normal, halfway cases, powers of
two, maximum finite, overflow, underflow, infinity, and quiet NaN. It shall
verify case, end pointers, malformed input, `errno`, precision rounding, and
round trips through the narrow formatted-I/O interfaces on every target.

## Relationships

- **Derived from:** C99 §7.20.1.3 and formatted-I/O numeric conversions
- **Depends on:** REQ-0024, REQ-0028, REQ-0029, and REQ-0030
- **Conflicts with:** Delegating conversion to an unavailable host CRT

## Tailoring

Only the T2 narrow allocation is approved here. The remaining REQ-0039 scope
cannot be reported complete until T6 closes the decimal-accuracy audit, wide,
locale, math-environment, and clause-integration allocations. T2 retains the
REQ-0028 decimal regression baseline but does not represent that earlier
algorithm as a newly verified correctly rounded decimal implementation.

## Implementation Record

`src/stdlib.c` recognizes hexadecimal subjects, signed infinity, and quiet
NaN forms without a host-CRT import. Hexadecimal conversion constructs
binary32 and binary64 representations directly, including ties-to-even,
subnormal, overflow, underflow, signed-zero, end-pointer, and `ERANGE`
behavior; `strtof` does not pass hexadecimal input through binary64 first.
The Windows `long double` path intentionally follows the documented binary64
model.

TC-0039 passes exact and adjacent binary vectors, binary32 halfway cases,
range cases, known boundary spellings, and 519 finite `%a` round trips on x86
and x64. ARM64 compile/link passes, while native execution and the later T6
wide/locale/environment allocation remain open.
