# REQ-0078: Microsoft narrow 64-bit integer conversions

WCRT shall declare `_i64toa`, `_ui64toa`, `_atoi64`, `_strtoi64` and
`_strtoui64` in `<stdlib.h>` in C89/C99 modes without `WCRT_POSIX`.
Microsoft API types `wcrt_ms_int64` and `wcrt_ms_uint64` in
`<wcrt/ms_types.h>` shall denote signed/unsigned 64-bit integers, using
`__int64` with MSVC and `long long` with TinyCC. `<limits.h>` shall expose
`_I64_MIN`, `_I64_MAX` and `_UI64_MAX` with corresponding exact values
and signedness in both modes. ISO C99 declarations remain edition-selected.

`_i64toa(value, buffer, radix)` and `_ui64toa(value, buffer, radix)` shall
write terminated lowercase digits for bases 2 through 36 and return `buffer`.
The signed function emits a minus only in base 10; other bases represent its
unsigned 64-bit bit pattern. The signed minimum is handled without signed
overflow. Caller capacity is required (65 bytes suffice); no size or overlap
checking is supplied. Null buffer or invalid radix returns null and EINVAL.
Success preserves errno, including through the existing 32-bit writer family.

`_strtoi64(string, end_pointer, base)` and `_strtoui64` shall parse C-locale
white space, optional sign, digits and base prefixes for bases 2-36 or
autodetection base zero. Base zero selects octal for leading zero, hexadecimal
for a 0x prefix followed by a valid hexadecimal digit, decimal otherwise. The
stopping pointer refers to the first unused byte, or the original input on no
conversion. No conversion returns zero and preserves errno. Signed overflow
returns the corresponding limit; unsigned magnitude overflow returns the
unsigned maximum; each sets ERANGE and consumes remaining valid digits.
Representable negative unsigned magnitudes use unsigned negation.

Null input or a base other than zero or 2-36 returns zero, sets EINVAL, and
sets a requested stopping pointer to the input (including null). `_atoi64`
uses signed decimal conversion without a stopping pointer and shares its
overflow/null behavior. Wide, secure and locale-object variants and Microsoft
invalid-parameter handlers are outside scope.

## Baseline and impact

These interfaces extend the roadmap's Microsoft numeric conversion family.
The contemporary Microsoft [writer reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/itoa-itow),
[signed reader reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strtoi64-wcstoi64-strtoi64-l-wcstoi64-l)
and [decimal reader reference](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/atoi64-atoi64-l-wtoi64-wtoi64-l)
are supplementary sources. Exact VC6 inventory membership and genuine
compiler/ABI/differential qualification remain Unknown; no modern reference
run substitutes for VC6. The existing ISO parser is reused behind Microsoft
argument validation. TC-0078 covers declarations, every radix, extrema,
end pointers, range and errors. TC-0056 and TC-0028 retain 32-bit and ISO
regression. The ABI addition does not alter existing symbols, storage,
imports, dependencies, privileges or Windows baseline.
