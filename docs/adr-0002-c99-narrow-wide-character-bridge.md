# ADR-0002: C99 Narrow Formatted-I/O Wide-Character Bridge

**Content type:** Architecture decision record

**Status:** Accepted

**Date:** 2026-08-09

## Context

C99 adds `l` character conversions to byte-oriented `printf` and `scanf`
functions. Those conversions accept or produce `wint_t` and `wchar_t` while
the complete wide-character headers, restartable conversions, locales, and
stream orientation are scheduled for T3. Omitting the conversions would leave
REQ-0029 and REQ-0030 incomplete; prematurely baselining all of T3 would mix
unrelated scope.

WCRT already exposes a 16-bit `wchar_t`, implements the C locale, and provides
the C89 single-byte multibyte conversion functions. Windows CRT interfaces use
a 16-bit unsigned `wint_t` representation.

## Decision Drivers

- Complete the byte-oriented C99 formatted-I/O clauses in T2.
- Preserve the Windows UTF-16 scalar ABI selected for later wide interfaces.
- Avoid exposing incomplete `<wchar.h>`, `mbstate_t`, or stream orientation.
- Make the character-conversion dependency visible to T3.
- Keep C-locale behavior independently testable on every architecture.

## Considered Options

1. Implement the `l` character conversions through a minimal C-locale bridge
   and bind T3 to the same 16-bit ABI.
2. Defer the conversions to T3 and classify T2 as partial.
3. Pull the complete T3 wide-character surface into T2.
4. Implement private character types that may differ from the future public
   ABI.

## Decision

T2 implements `lc`, `ls`, and `l[` in the byte-oriented formatted-I/O
functions through a minimal C-locale bridge. The bridge uses 16-bit
`wchar_t`. Only single-byte C-locale characters are accepted or produced
during T2.

The implementation may use internal state equivalent to a zero-initialized
conversion state, but T2 does not expose `mbstate_t`, `mbrtowc`, `wcrtomb`,
wide formatted-I/O functions, or stream orientation. ADR-0003 supersedes this
record's anticipated 16-bit `wint_t` with a 32-bit unsigned public type. The
change does not alter T2's 16-bit `wchar_t` objects or C-locale encoding, but
T3 shall update and reverify the variadic `lc` bridge against the accepted
public type.

The T2 personal design review accepted this decision on 2026-08-09. Local
x86 and x64 behavior tests pass; ARM64 compile/link passes and native behavior
remains a CI verification obligation.

## Rationale

The bridge completes the narrow clauses without making the formatted-I/O
engine depend on an unfinished public wide library. The C-locale restriction
matches WCRT's only currently supported locale. ADR-0003's promotion-safe
`wint_t` requires a deliberate T3 varargs review instead of copying the
Microsoft UCRT typedef.

## Consequences

### Positive

- T2 can verify every applicable C99 character conversion.
- The future `wint_t` and `wchar_t` ABI is explicit before varargs code exists.
- No incomplete public wide header or restartable-conversion contract leaks.

### Negative

- T3 inherits a compatibility constraint from an earlier tranche.
- T2 supports only single-byte C-locale conversion, not general UTF-16 text
  interchange.
- Tests must distinguish the byte-oriented bridge from complete wide I/O.

### Follow-up

- Verify `lc`, `ls`, and `l[` in TC-0029 and TC-0030 on every target.
- Add the public types and restartable conversions in REQ-0031/REQ-0033.
- Review this decision during T3 requirements and ABI design.

## References

- [REQ-0029 — C99 narrow formatted output](req-0029-stdio-c99-format.md)
- [REQ-0030 — C99 narrow formatted input](req-0030-stdio-c99-scan.md)
- [C99 platform model](c99-platform-model.md)
- [T2 work log](work/c99-t2-narrow-formatted-io.md)
