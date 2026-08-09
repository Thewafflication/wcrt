# REQ-0015 — `<time.h>`

**Content type:** Project requirement

**Status:** Implemented

**Source:** §4.12 Date and time

## Scope

### Required files

- `include/time.h` — public types, macros, structure, and declarations.
- `src/time.c` — clock, calendar conversion, and formatting.
- `src/platform/windows/clock.c` — wall-clock and processor-time backends.
- `tests/c89/time.c` — conversion, normalization, formatting, and range tests.

### Functions

| Draft clause | Required functions |
| --- | --- |
| §4.12.2 Time manipulation | `clock`, `difftime`, `mktime`, `time` |
| §4.12.3 Time conversion | `asctime`, `ctime`, `gmtime`, `localtime`, `strftime` |

### Other public surface

Types `clock_t`, `time_t`, and `size_t`; macros `NULL` and `CLOCKS_PER_SEC`;
and `struct tm` with `tm_sec`, `tm_min`, `tm_hour`, `tm_mday`, `tm_mon`,
`tm_year`, `tm_wday`, `tm_yday`, and `tm_isdst`.

## Requirement

- WCRT shall document `time_t` representation, epoch and range; `clock_t`
  interpretation and wrap behavior; and the meaning of processor time.
- `mktime` shall normalize fields, determine calendar fields and daylight time,
  and report an unrepresentable result as specified.
- `gmtime` and `localtime` shall populate every required `struct tm` member and
  obey static-storage overwrite rules.
- `asctime` and `ctime` shall produce the required fixed-format representation
  for representable inputs.
- `strftime` shall implement every C89 conversion specifier, locale-dependent
  text, buffer limit, and return rule.
- Timezone and daylight-saving behavior shall work without assuming Windows
  APIs newer than the baseline target.

## Rationale

WCRT must provide its own `<time.h>` contract to supply C89-conforming behavior without depending on a host C runtime.

## Verification

**Method:** Automated test and inspection

**References:** `TC-0015`

Tests shall cover epoch/range boundaries, leap years, month lengths,
normalization, weekday/year-day calculation, daylight transitions where stable,
static result buffers, all `strftime` conversions, insufficient buffers, and
clock progression. Shared gates apply.

## Relationships

- **Derived from:** §4.12 Date and time
- **Depends on:** Shared build, ABI, and quality gates in `docs/REQUIREMENTS.md`
- **Conflicts with:** None known

## Tailoring

WCRT groups the related obligations for this C89 conformance unit under one
stable requirement identifier. Each observable obligation remains explicit in
this record and is verified collectively by `TC-0015`.

## Implementation Record

`time_t` is a signed 64-bit count of seconds since the Unix epoch and
`clock_t` is a signed millisecond count with `CLOCKS_PER_SEC` equal to 1000.
Wall and processor clocks use `GetSystemTimeAsFileTime` and `GetProcessTimes`.
Local conversion uses `FileTimeToLocalFileTime` and its inverse, all available
on Windows 2000. Calendar arithmetic uses the proleptic Gregorian calendar.
Conversion results share one `struct tm`; text results share one 26-byte
buffer. Daylight status is reported as indeterminate (`tm_isdst == -1`) while
Windows still applies its timezone rules during UTC/local conversion.

## T3 Wide-character Implementation Impact

REQ-0031 allocates `wcsftime`. The narrow formatter now supplies the audited
C99 conversion delta, including ISO week/year and the C-locale `E`/`O`
modifiers, before `wcsftime` applies the same results with wide format and
destination buffer-length rules. Narrow time regressions and TC-0031 pass on
x86/x64 and compile/link for ARM64.
