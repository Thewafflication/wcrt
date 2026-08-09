# ADR-0003: C99 Wide-Character and Stream ABI

**Content type:** Architecture decision record

**Status:** Accepted

**Date:** 2026-08-09

## Context

C99 tranche T3 adds the public wide-character types, restartable conversions,
wide streams, and classification facilities. These interfaces expose ABI and
encoding choices that must be stable across the supported x86, x64, and ARM64
Windows targets before implementation starts.

The inspected baseline already uses TinyCC's unsigned 16-bit `wchar_t` and a
single-byte, stateless C-locale conversion in the C89 `stdlib` functions.
ADR-0002 also anticipated an unsigned 16-bit `wint_t` to match the Microsoft
UCRT. That anticipated type is changed by the integer promotions, however,
while C99 7.24.1 requires `wint_t` to be unchanged by the default argument
promotions and to provide a value outside the supported extended character
set. WCRT has not yet exposed a public `wint_t`, so T3 is the last point at
which this conflict can be resolved without breaking an implemented WCRT
public declaration.

The current `FILE` implementation has no orientation or conversion state and
uses a byte-only pushback slot. The current `fpos_t` is a signed 64-bit file
offset. Only the mandatory C locale is implemented; both `"C"` and the empty
locale name select it.

## Decision Drivers

- Meet the C99 type and stream contracts rather than silently inheriting a
  known Microsoft UCRT deviation.
- Preserve direct Windows UTF-16 code-unit interoperability for `wchar_t`
  strings on x86, x64, and ARM64.
- Preserve the implemented C89 single-byte C locale and narrow-stream results.
- Make initial state, encoding errors, stream orientation, and position-state
  restoration explicit and testable.
- Leave room for later multibyte locales without changing the T3 public
  `mbstate_t` layout.
- Avoid new production imports and preserve the Windows 2000 x86 import gate.

## Considered Options

1. Use 16-bit `wchar_t` and 32-bit `wint_t`, preserving Windows string
   interoperability while satisfying the C99 promotion and sentinel rules.
2. Use the Microsoft UCRT model of 16-bit `wchar_t`, 16-bit `wint_t`, and
   `WEOF == 0xffff`, and publish the resulting C99 deviation.
3. Change both types to 32 bits, losing direct Windows `WCHAR` interoperability.
4. Add a UTF-8 or other stateful locale in T3 and change the C89 conversion
   baseline at the same time.

## Decision

### Scalar and state ABI

- `wchar_t` remains TinyCC's unsigned 16-bit `__WCHAR_TYPE__` on all three
  targets. Its object representation, alignment, and array layout are the same
  as Windows `WCHAR`/UTF-16 code units. Wide string and memory functions
  operate on code units; they do not combine or normalize surrogate pairs.
- `wint_t` is `unsigned int`, 32 bits on all three targets. It is unchanged by
  the integer promotions and can represent every `wchar_t` value plus a
  distinct sentinel. `WEOF` is `(wint_t)UINT_MAX`; `WINT_MIN` is zero and
  `WINT_MAX` is `UINT_MAX`.
- This `wint_t` is intentionally not binary-compatible with the Microsoft
  UCRT's 16-bit `wint_t`. Windows APIs do not use `wint_t`; interoperability at
  the operating-system boundary is through 16-bit `wchar_t`/`WCHAR` strings.
- `mbstate_t` has a fixed, public eight-byte, four-byte-aligned layout on x86,
  x64, and ARM64: one 32-bit accumulator and two 16-bit state fields. All-zero
  object representation is the initial conversion state. Field names are
  implementation-reserved, and applications shall treat the type as opaque.
  The layout provides capacity for later stateful encodings without a public
  ABI change.

### Encoding and locale model

- T3 retains only the mandatory C locale. The empty locale name remains an
  alias for `"C"`; T3 does not add a UTF-8 or Windows-code-page locale.
- The C-locale multibyte encoding remains stateless and single-byte with
  `MB_CUR_MAX == 1`. A non-null byte maps to the same-valued `wchar_t`; the null
  byte maps to the null wide character. Consequently every input byte is a
  complete valid C-locale multibyte character, while a `wchar_t` value above
  `UCHAR_MAX` has no C-locale multibyte representation and produces `EILSEQ`
  where C99 requires an encoding error.
- `mbrtowc` reports an incomplete sequence only when no input byte is
  available (`n == 0`). No supported T3 locale has a non-initial shift state;
  successful complete conversions and null conversions leave the state
  initial. Internal states used when `ps == NULL` remain separate per function
  as C99 requires.
- UTF-16 describes the in-memory wide ABI, not the external encoding of a
  wide-oriented stream. In T3, wide streams use the C-locale single-byte
  external encoding captured when the stream becomes wide-oriented. Code units
  above `UCHAR_MAX`, including surrogate code units, therefore cannot be
  transferred through a C-locale wide stream and fail with `EILSEQ`.

### Stream model

- Every newly opened stream and all three predefined streams start unoriented.
  The first byte I/O operation makes a stream byte-oriented; the first wide I/O
  operation makes it wide-oriented. `fwide` may select an unoriented stream or
  query an oriented stream but cannot reverse an established orientation.
- A successful `freopen` clears orientation, byte/wide pushback, and conversion
  state. Closing and reusing a dynamic stream slot also starts from that state.
- Each wide-oriented stream owns an `mbstate_t` and one guaranteed wide
  pushback slot. Byte and wide pushback state are never active together.
- The supported external encoding is stateless and every successful wide
  character transfer ends in the initial state. `fpos_t` therefore remains the
  signed 64-bit offset ABI: saving and restoring a position also restores the
  only supported stream conversion state, the initial state, without adding
  bytes to the public type. This choice must be revisited before any locale
  introduces a non-initial stream state.
- ISO C programs shall not apply a byte I/O function to a wide-oriented stream
  or a wide I/O function to a byte-oriented stream. As a defensive WCRT
  extension, the implementation rejects such an operation
  without transferring data, set the stream error indicator and `errno` to
  `EINVAL`, and return the applicable failure sentinel. Extension tests shall
  be labeled separately from ISO conformance tests.

## Compatibility Impact

If accepted, this decision supersedes only ADR-0002's anticipated 16-bit
public `wint_t`; it retains ADR-0002's 16-bit `wchar_t` and C-locale bridge.
Because `<wchar.h>` and `wint_t` do not exist in the current public baseline,
there is no implemented WCRT header ABI to break. The T2 `%lc` path and its
tests must nevertheless be reviewed and updated to retrieve the exact promoted
public `wint_t` argument type. REQ-0022 must add `WINT_MIN`/`WINT_MAX`, and
REQ-0029, REQ-0030, their tests, and the platform-model record require impact
updates.

Choosing 16-bit `wint_t` instead would preserve UCRT binary compatibility but
would require an explicit C99 deviation in REQ-0033 and the 1.0 conformance
profile. That alternative must not be substituted during implementation
without re-reviewing this ADR and the affected variadic ABI.

## Consequences

### Positive

- The public `wint_t` and `WEOF` model satisfies the applicable C99 type rules.
- Windows `WCHAR` arrays remain directly interoperable on every target.
- C89 conversions and ordinary narrow-stream results do not change.
- `mbstate_t` and stream state have one architecture-independent ABI.

### Negative

- WCRT wide functions are not binary-compatible with UCRT functions whose
  signatures contain Microsoft's 16-bit `wint_t`.
- T3 does not provide general Unicode-to-multibyte interchange; UTF-16 code
  units above 255 cannot be represented in the only supported multibyte locale.
- Opposite-orientation defensive failures add review and regression work beyond
  the minimum ISO contract.
- A future stateful locale may require changing `fpos_t`; that cannot occur in
  the 1.x ABI without a compatibility decision.

## Approval and Implementation Follow-up

The WCRT maintainer accepted this decision with the T3 planning baseline on
2026-08-09. Before production-source changes, REQ-0033/TC-0033 were baselined,
the affected requirements and T2 bridge design were updated, and controlled
tests recorded the expected missing-header failures.

The implementation now follows this ABI in the public headers, conversion
core, stream state, formatted/unformatted I/O, and target probes. Native x86
and x64 tests and the Windows `lstrlenW` calling probe pass; ARM64 compiles and
links. Native ARM64 execution and immutable exact-revision evidence remain
open target-completion obligations rather than changes to this decision.

`wWinMain` remains excluded. It requires a separate approved work item after
REQ-0031 through REQ-0033 are complete.

## References

- [ADR-0002 — C99 narrow formatted-I/O wide-character bridge](adr-0002-c99-narrow-wide-character-bridge.md)
- [C99 compiler capabilities and Windows data models](c99-platform-model.md)
- [C99 requirements backlog](C99-REQUIREMENTS.md)
- [T3 work log](work/c99-t3-wide-abi-character-core.md)
- WG14/N1256, 7.19.2, 7.24, and 7.25
