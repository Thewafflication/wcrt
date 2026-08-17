# REQ-0033 — Windows UTF-16 and wide-character ABI

**Content type:** Project requirement

**Status:** Implemented; exact native Debug x86/x64/ARM64 verification passes

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.17, §7.24.1,
§7.24.6, and §7.25.1; Windows x86, x64, and ARM64 ABI

## Scope

This requirement controls the public scalar and state types, the C-locale
multibyte model, stream-orientation state, and Windows interoperability on all
supported architectures. It is the design and ABI prerequisite for REQ-0031
and REQ-0032. It does not add `wWinMain` or promise Unicode external stream
encoding.

### Required files

- `include/wchar.h`, `include/wctype.h`, and `include/stdint.h` — public types,
  limits, state, and end-of-file constants.
- `src/internal/file.h` and the stdio implementation — durable per-stream
  orientation, conversion state, and wide pushback.
- The restartable conversion implementation — C-locale state transitions and
  errors.
- `tests/c99/wide-abi.c` — type, layout, conversion, orientation, and target
  verification.

## Requirement

- `wchar_t` shall be the compiler's unsigned 16-bit `__WCHAR_TYPE__` on x86,
  x64, and ARM64 and shall be layout-compatible with Windows `WCHAR` and
  UTF-16 code units.
- `wint_t` shall be a 32-bit unsigned integer type unchanged by the integer
  promotions. `WEOF` shall be the all-ones `wint_t` value and shall not equal
  any `wchar_t` value. `WINT_MIN` and `WINT_MAX` shall describe that type.
- `mbstate_t` shall be an 8-byte, 4-byte-aligned complete object type on every
  supported architecture. Its all-zero representation shall be the initial
  conversion state; callers may copy it as an object but shall not depend on
  private member meaning.
- WCRT's only supported locale shall remain the C locale. Its multibyte
  encoding is stateless, has `MB_CUR_MAX == 1`, maps bytes `0x00` through
  `0xff` one-to-one to equal-valued wide characters, and rejects other wide
  values with `EILSEQ` where the C interface reports encoding errors.
- Restartable conversions shall define reset calls, null source pointers,
  zero-length input, null output pointers, partial buffers, source-pointer
  updates, terminating null handling, invalid values, and caller-supplied or
  internal state. A completed C-locale conversion shall leave the state
  initial; no incomplete multibyte sequence exists in this encoding.
- Every open stream shall begin unoriented. The first byte or wide I/O
  operation shall bind its orientation; `fwide` shall query or bind it without
  transferring data. `freopen` shall reset orientation and conversion state.
  Positioning shall clear pushback and conversion state without changing an
  established orientation.
- Opposite-orientation I/O is a checked WCRT extension: it shall transfer no
  data, set the stream error indicator, set `errno` to `EINVAL`, and return the
  function's failure result. Existing correctly oriented narrow I/O behavior
  shall otherwise remain unchanged.
- A stream shall retain independent byte pushback, one wide pushback value,
  and conversion state. `ungetwc` shall reject `WEOF`, preserve orientation,
  clear EOF on success, and fail when the wide pushback slot is occupied.
- `fpos_t` shall retain its signed 64-bit ABI. Because the supported encoding
  is stateless, no additional conversion-state payload is required in a
  position object.
- Public headers, structure layout, variadic wide arguments, compile/link
  behavior, and Windows `WCHAR` interoperation shall be verified separately
  for x86, x64, and ARM64. Production sources shall remain compilable under
  the project's C89 implementation dialect while the new public surface is
  absent from selected C89 headers.

## Rationale

Windows APIs exchange 16-bit UTF-16 code units, but C99 requires `wint_t` to be
unchanged by integer promotion. Separating those types preserves Windows
object interoperability and gives `WEOF` a value outside the complete
`wchar_t` range. A target-invariant state object avoids architecture-specific
public layouts and reserves room for a future locale without silently changing
ABI.

## Verification

**Method:** Automated compile-time and behavioral test, ABI inspection,
analysis, and review

**References:** `TC-0033`

TC-0033 shall verify exact sizes, signedness, alignment, constants, initial
state, every conversion-state transition and error partition, stream
orientation transitions and conflicts, pushback, position/reset behavior,
C89 isolation, C89 narrow-stream regression, and target-specific Windows
interoperability. ARM64 behavior evidence shall be retained from native CI;
cross-compilation alone is not target completion.

## Relationships

- **Derived from:** C99 wide-character common definitions and conversion
  clauses plus the supported Windows ABIs
- **Depends on:** REQ-0003, REQ-0006, REQ-0012, REQ-0013, REQ-0016,
  REQ-0022, ADR-0002, and ADR-0003
- **Required by:** REQ-0031 and REQ-0032
- **Conflicts with:** A 16-bit `wint_t`, process-global stream orientation, or
  treating UTF-16 memory representation as an external stream encoding

## Tailoring

No locale other than C is supplied. UTF-16 surrogate code units are valid
`wchar_t` objects for Windows interoperation but are not representable in the
C-locale multibyte encoding. This tranche deliberately does not add
`wWinMain`; that requires a separately approved work item after REQ-0031
through REQ-0033 are complete.

## Implementation Record

`include/wchar.h` defines the promotion-safe 32-bit `wint_t`, distinct
`WEOF`, and fixed eight-byte `mbstate_t`; `include/stdint.h` publishes the
matching `WINT_MIN` and `WINT_MAX`. `src/wchar_convert.c` implements the
stateless C-locale transition rules. `src/internal/file.h`, `src/stdio.c`,
`src/platform/windows/file.c`, and `src/wchar_io.c` implement per-stream
orientation, conversion state, wide pushback, reset, positioning, and checked
orientation conflicts without widening `fpos_t`.

TC-0033 verifies type promotion, size, alignment, all-zero initial state,
partial and invalid conversions, source-pointer transitions, orientation,
pushback, reopen/position reset, no-transfer conflicts, and C89 isolation. Its
Windows probe passes WCRT `wchar_t` storage containing BMP and surrogate code
units directly to `lstrlenW` on native x86, x64, and ARM64 in exact-revision
run `32027269426`. Independent review remains required before release
acceptance.
