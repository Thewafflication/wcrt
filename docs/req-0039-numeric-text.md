# REQ-0039 — Numeric text interchange

**Content type:** Project requirement

**Status:** T6 integration verified on exact-revision native x86/x64/ARM64

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.19.6 and §7.20.1.3

## Scope

The requirement controls narrow and wide numeric text, infinity, NaN, signed
zero, rounding, range errors, and integration among the `strto*`, formatted
output, and formatted input families. It applies to binary32 `float` and the
Windows binary64 `double`/`long double` model in WCRT's only locale, C.

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
- For decimal subjects, WCRT shall document the implementation-defined
  accuracy of its 19-significant-digit accumulation and binary scaling path.
  The C99 recommended practice of universal correct rounding through
  `DECIMAL_DIG` is not claimed. Its known above-halfway counterexample and
  controlled decimal round-trip envelope shall remain visible in TC-0039.
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
- Narrow and wide conversions shall agree on accepted C-locale subject
  sequences, end positions, signs, classifications, values, and range errors.
  The locale decimal point is `.`; a comma terminates the subject.
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
round trips through the narrow formatted-I/O interfaces on every target. It
shall also retain the documented decimal accuracy counterexample, exact-integer
boundary, C-locale termination, and a controlled 17-digit decimal format/parse
envelope.
REQ-0031 supplies matching wide-interface partitions.

## Relationships

- **Derived from:** C99 §7.20.1.3 and formatted-I/O numeric conversions
- **Depends on:** REQ-0024, REQ-0028, REQ-0029, and REQ-0030
- **Conflicts with:** Delegating conversion to an unavailable host CRT

## Tailoring

WCRT supports the C locale only. Decimal universal correct rounding is C99
recommended practice rather than a required hosted-library result and is
explicitly omitted from the 1.0.0 profile. Hexadecimal correct rounding remains
required. The selected conversion policy is independent of Annex F claims.

## Implementation Record

`src/stdlib.c` recognizes hexadecimal subjects, signed infinity, and quiet
NaN forms without a host-CRT import. Hexadecimal conversion constructs
binary32 and binary64 representations directly, including ties-to-even,
subnormal, overflow, underflow, signed-zero, end-pointer, and `ERANGE`
behavior; `strtof` does not pass hexadecimal input through binary64 first.
The Windows `long double` path intentionally follows the documented binary64
model.

TC-0039 covers exact and adjacent binary vectors, binary32 halfway cases,
range cases, known boundary spellings, 519 finite `%a` round trips, controlled
17-digit decimal format/parse values, and the documented one-ULP decimal
accuracy counterexample. Exact-revision native x86/x64/ARM64 TC-0039 passes in
GitHub Actions run `32027269426`.

## T3 Wide-character Implementation Allocation

REQ-0031 brings the wide-input forms of integer and floating numeric text,
including formatted wide I/O, into T3. The implementation checked-adapts the
narrow subject-sequence and rounding logic and replays exact-value, range,
end-pointer, signed-zero, infinity, NaN, hexadecimal, malformed-subject, and
round-trip cases through the wide entry points. Locale/environment expansions
beyond the C-locale model remain later work.
