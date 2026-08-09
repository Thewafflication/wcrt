# C99 T3 Wide ABI and Character Core Work Log

**Status:** Implemented and locally verified; native ARM64 CI, retained
exact-revision evidence, and independent review pending

**Owner:** WCRT maintainer

**Created:** 2026-08-09

**Inspected source baseline:**
`98b568b14ef92240fb08ec5f2f86a5032caf072c`
(`Implement C99 T2 narrow formatted I/O`)

## Baseline and Scope

T3 will baseline and implement REQ-0033 (Windows UTF-16 ABI) first, followed
by REQ-0031 (`<wchar.h>`) and REQ-0032 (`<wctype.h>`), on the supported x86,
x64, and ARM64 Windows targets. The ordering is normative for this tranche:
no public wide header or production wide-library source may be added until the
REQ-0033 type, encoding, locale, state, orientation, and interoperability
contract and its controlled test specification are reviewed.

REQ-0033 will define:

- the width, signedness, promotion, limits, sentinel, and cross-header ownership
  of `wchar_t` and `wint_t`;
- the exact size, alignment, zero representation, and target invariance of
  `mbstate_t`;
- the supported extended-character and multibyte encoding, `MB_CUR_MAX`,
  initial conversion state, internal null-state objects, incomplete and invalid
  sequence behavior, `EILSEQ`, and `LC_CTYPE` interaction;
- the orientation, encoding-rule capture, conversion-state, pushback,
  positioning, reopen, EOF/WEOF, and error invariants of `FILE`; and
- Windows UTF-16 code-unit interoperability, including BMP values, surrogate
  code-unit pairs in memory, alignment, and calling behavior on all targets.

REQ-0031 will cover the complete C99 7.24 public surface: 59 `<wchar.h>`
functions, the public types and macros, formatted and unformatted wide I/O,
stream orientation, wide numeric conversions, wide string and memory
operations, `wcsftime`, single-byte/wide conversions, and restartable character
and string conversions. It also closes the two 7.8.2.4 wide greatest-width
integer conversions, `wcstoimax` and `wcstoumax`, that are missing from the
implemented REQ-0027 baseline.

REQ-0032 will cover all 18 C99 7.25 classification, descriptor, case-mapping,
and extensible-mapping functions together with `wctype_t`, `wctrans_t`,
`wint_t`, and `WEOF` consistency.

The following existing baselines are explicit impact and regression work, not
contracts silently merged into T3:

- REQ-0006 locale selection and the mandatory C locale;
- REQ-0012 C89 stream, position, file, and narrow-I/O behavior;
- REQ-0013 C89 multibyte conversions and `MB_CUR_MAX`;
- REQ-0015 `strftime`, because conforming `wcsftime` is defined in terms of the
  C99 time-format surface rather than only the current C89 subset;
- REQ-0022 `WINT_MIN` and `WINT_MAX`;
- REQ-0027 `wcstoimax` and `wcstoumax`;
- REQ-0029 and REQ-0030 exact T3 conversion/ABI integration for the narrow
  `l` character forms; and
- the wide numeric-text allocation of REQ-0039.

T3 excludes additional installed locales, locale-specific Unicode databases,
normalization, grapheme processing, Microsoft wide-path or secure-CRT
extensions, C11 `<uchar.h>`, `wWinMain`, release publication, and unrelated
stream buffering work. The only supported locale remains `C` unless the
requirements and this estimate are explicitly re-baselined before source
implementation.

`wWinMain` is prohibited in T3. It may be considered only after REQ-0031
through REQ-0033 are complete and a separate approved work item admits it.

## Size and Effort Estimate

**Controlled size:** Three primary requirement/test conformance units, nine
existing-requirement impact reviews, approximately 77 C99 wide-header
functions, two deferred 7.8 wide integer conversions, and the associated
type/macro, stream-state, build, review, and evidence obligations.

**Estimated change size after repository inspection:** 40-56 source-controlled
artifacts; 4,000-6,800 runtime/header lines, 2,600-4,500 test/runner lines, and
1,200-2,000 controlled-document/integration lines. The complete patch is
expected to contain **7,800-13,300 changed lines**. This estimate includes
requirements, test specifications, C89-isolation probes, runner/manifest/build
integration, implementation, and retained-evidence support; it is not an
implementation-only LOC estimate.

| Phase | Planned focused hours |
| --- | ---: |
| Plan and estimate | 8-14 |
| Specify controlled requirements and tests | 24-40 |
| Design and design review | 32-52 |
| Implement | 90-150 |
| Personal code and test review | 28-46 |
| Verify and retain evidence | 38-64 |
| Postmortem and re-estimate | 6-10 |
| **Base total** | **226-376** |

The completion forecast is **260-489 focused hours**, including the work
plan's 15-30 percent T3 risk reserve (34-113 hours after rounding). The reserve
is held for the unresolved `wint_t` compatibility decision, formatted-wide-I/O
reuse limits, stream-state regressions, and native-target defects; it is not
silently allocated as implementation time. No calendar completion date is
committed because maintainer weekly capacity and comparable actual phase time
remain unavailable.

The prior 140-230-hour and 2,500-4,500-LOC T3 entry is not retained. It did not
identify the 59-function `<wchar.h>` surface, the 18-function `<wctype.h>`
surface, two deferred `<inttypes.h>` functions, C99 time-format dependency,
existing stream-core changes, controlled tests, or evidence work. T2's final
local snapshot (28 artifacts and 3,361 changed lines) confirms that controlled
records, tests, runners, and integration are a material part of tranche size,
but its empty time log provides no basis for reducing T3 hour ranges.

## Repository Reconciliation and Discrepancies

1. REQ-0031, REQ-0032, and REQ-0033 are planned identifiers only. There are no
   controlled requirement records, test specifications, public wide headers,
   production wide units, runners, manifest rows, or retained results.
2. The work-plan row describes three conformance units but materially
   understates their surface. C99 7.24 contains 59 functions and C99 7.25
   contains 18; completing the dependent 7.8 wide conversions brings the
   tranche behavior total to approximately 79 functions.
3. REQ-0027 states that `wchar_t`/`wint_t` integer conversion facilities are
   not part of C99 7.8. C99 7.8.2.4 requires `wcstoimax` and `wcstoumax`.
   Neither declaration, implementation, nor controlled test exists.
4. REQ-0022 correctly leaves `WINT_MIN` and `WINT_MAX` pending. `<stdint.h>`
   currently defines only `WCHAR_MIN` and `WCHAR_MAX`.
5. ADR-0002 and the platform model anticipate unsigned 16-bit `wint_t` to
   match the Microsoft UCRT. Such a type is promoted to `int`, conflicting
   with C99 7.24.1's requirement that `wint_t` be unchanged by default argument
   promotions. The installed Windows UCRT confirms its 16-bit choice and
   `WEOF == 0xffff`; WCRT must choose ISO conformance or UCRT `wint_t` binary
   compatibility explicitly rather than claim both.
6. The inspected compiler evidence confirms two-byte `wchar_t` on x86, x64,
   and ARM64. Windows SDK headers likewise define `WCHAR` as a 16-bit type.
   Width is established, but calling, alignment, signedness, surrogate-memory,
   and native interoperation tests are still absent.
7. REQ-0013 names `src/internal/mbstate.h`, but that file does not exist. The
   C89 conversion functions are implemented directly in `src/stdlib.c`, have
   no retained conversion object, accept every non-null byte as one C-locale
   character, and reject wide values above 255 only in the output direction.
8. `MB_CUR_MAX` is a constant 1 and the locale implementation supports only
   `C` plus the empty-name alias. This is a coherent C89 baseline, but T3 must
   explicitly decide whether to preserve it. Adding UTF-8 or a Windows code
   page would change scope, state behavior, old tests, and this estimate.
9. `struct wcrt_file` has no orientation, encoding rule, `mbstate_t`, or wide
   pushback. `fgetpos`/`fsetpos` store only a signed 64-bit offset, and the
   byte I/O paths do not commit or check orientation.
10. The T2 `printf` wide bridge accepts output code units only through 127 and
    the scanner directly widens bytes. It does not call the future restartable
    conversion primitives, and its `%lc` variable-argument type must be
    reconciled with the accepted public `wint_t`.
11. The implemented REQ-0039 record leaves wide numeric text to a later
    tranche. Complete `wcstof`, `wcstod`, `wcstold`, and wide formatted numeric
    I/O require moving that allocation into T3 with explicit shared-oracle
    regression coverage.
12. `wcsftime` is required by `<wchar.h>`, but the current `strftime` is the
    C89 surface. C99 additions such as ISO week/year, composite forms, and
    `E`/`O` modifiers are not controlled. T3 must either close that existing-
    header impact now or record a visible deviation; deferring it while
    claiming complete `<wchar.h>` is not acceptable.
13. Production builds enumerate source files and compile the library with
    `-std=c89`. Focused C89, C99, compatibility, ARM64-cross, and build runners
    also enumerate source sets independently. New implementation units must be
    C89-source-compatible and must be added consistently, while the public
    wide surface remains absent under `WCRT_C89`.
14. T2 requirements and local tests are implemented, but native ARM64,
    immutable retained CI evidence, and independent review remain pending in
    the T2 work log. T3 may plan from T2 source but cannot treat those target
    gates as completed evidence.
15. The roadmap's 2B checkboxes remain open even though REQ-0027 through
    REQ-0030 have local implementations. They must not be marked complete
    until their supported-target evidence gates are actually closed.

## Accepted Controlled Baseline

| Unit | Accepted boundary | Planned verification | Key dependencies |
| --- | --- | --- | --- |
| REQ-0033 / TC-0033 | Exact Windows wide scalar/state ABI; C-locale external encoding; initial and internal states; conversion and stream-state invariants; implementation-defined choices and deviations | Compile-time type/promotion/layout assertions; conversion state tables; native Windows UTF-16 calling and memory tests; x86/x64/ARM64 results | REQ-0006, REQ-0011, REQ-0013, REQ-0022, ADR-0002, proposed ADR-0003 |
| REQ-0031 / TC-0031 | Complete C99 7.24 header, 59 functions, stream orientation, strings/memory, numeric/time conversions, restartable conversions, plus the dependent `wcstoimax`/`wcstoumax` closure | Header isolation; state-transition, decision-table, boundary, malformed-input, buffer-sentinel, round-trip, stream-remainder, format/scan matrix, and architecture tests | Accepted REQ-0033; REQ-0012, REQ-0013, REQ-0015, REQ-0027 through REQ-0030, REQ-0039 |
| REQ-0032 / TC-0032 | Complete C99 7.25 C-locale classifications, descriptors, mappings, type/sentinel consistency, and invalid-name behavior | All named classes/mappings, direct/descriptor equivalence, ASCII and boundary partitions, WEOF, unsupported code units, locale consistency, and all targets | Accepted REQ-0033 and conversion primitives from REQ-0031; REQ-0002 and REQ-0006 |

The controlled requirements shall distinguish ISO-defined behavior,
undefined caller cases, WCRT defensive extensions, implementation-defined
encoding choices, and deliberate compatibility deviations. Tests shall not
turn undefined opposite-orientation calls, overlapping non-move operations,
invalid format/type pairs, or out-of-domain `wctype` arguments into accidental
portable C promises.

## Accepted Architecture Baseline

The detailed durable decision is recorded in accepted
`docs/adr-0003-c99-wide-character-and-stream-abi.md`. REQ-0033/TC-0033,
REQ-0031/TC-0031, and REQ-0032/TC-0032 are baselined before production-source
changes. The accepted baseline is:

1. Preserve unsigned 16-bit `wchar_t` and direct Windows `WCHAR` code-unit
   layout on x86, x64, and ARM64.
2. Use 32-bit `unsigned int wint_t`, `WEOF == UINT_MAX`, and matching
   `WINT_MIN`/`WINT_MAX`; explicitly decline Microsoft UCRT `wint_t` binary
   compatibility so the public type satisfies C99 promotion rules.
3. Fix `mbstate_t` at eight bytes and four-byte alignment on every target, with
   all-zero representation as the initial state and opaque reserved fields for
   later stateful encodings.
4. Retain the stateless one-byte C-locale encoding and `MB_CUR_MAX == 1`.
   Every byte is a complete input character; wide values above 255 have no
   multibyte representation and produce `EILSEQ` as applicable. No UTF-8 or
   code-page locale is admitted in this baseline.
5. Treat UTF-16 as the in-memory wide ABI. Wide string and memory functions
   operate on 16-bit code units. They preserve surrogate pairs but do not
   validate, combine, normalize, or case-map them in the C locale.
6. Make all new and predefined streams initially unoriented. Capture the
   current C-locale encoding when wide orientation is established, maintain a
   per-stream state and wide pushback slot, and reset both only through the
   specified reopen/close lifecycle.
7. Retain the signed 64-bit `fpos_t` because the only supported stream
   conversion state is always initial at completed I/O boundaries. Any later
   stateful stream encoding requires a new ABI impact review.
8. Separate wide formatting/scanning character-width adapters from the proven
   T2 numeric and exact-varargs core. Do not route wide streams through byte
   I/O functions or merge `swprintf` bounds with `snprintf` or Microsoft
   bounded contracts.
9. Reject opposite-orientation I/O without transfer as a documented WCRT
   safety extension, while labeling such tests non-ISO because the standard
   forbids those calls.

## Dependencies, Risks, and Responses

- **`wint_t` conformance/compatibility conflict — critical:** the documented
  T2 future choice and Microsoft UCRT conflict with the C99 promotion rule.
  Accept proposed ADR-0003 or approve a visible deviation before any public
  typedef, varargs test, or exported function is written.
- **Encoding-scope expansion — critical:** adding UTF-8, ACP/OEM code pages,
  or UTF-16 external stream encoding changes state transitions and old C89
  behavior. Keep T3 C-locale-only; re-baseline requirements, size, risks, and
  evidence before admitting another locale.
- **Stream-state regression — critical:** every byte I/O path currently lacks
  orientation logic. Centralize orientation entry checks, specify transition
  tables first, and rerun all C89/narrow/MS compatibility streams after each
  stream-core stage.
- **Formatted-wide-I/O complexity — high:** wide format/scan is a complete
  family, not a character-by-character wrapper around narrow functions. Reuse
  reviewed numeric and exact-type helpers behind width-specific sources and
  sinks; preserve distinct `swprintf`, stream, and narrow bounded contracts.
- **Partial, invalid, and EOF state — high:** incomplete input, invalid output,
  null conversion, WEOF, stream EOF, error indicators, pushback, and source
  pointer advancement can be conflated. Use explicit state tables and retained
  first-failure tests before implementation.
- **Buffer/count risk — high:** wide widths, precisions, destination lengths,
  conversion byte counts, and terminators are caller-controlled. Use checked
  arithmetic, no caller-sized stack arrays, guard code units/bytes, and tests
  at zero, one, exact, truncated, and maximum practical bounds.
- **`wcsftime` hidden dependency — high:** the current time formatter is C89.
  Review and test the C99 `strftime` delta before implementing `wcsftime` so
  the wide function is not declared complete over a partial engine.
- **Numeric duplication — high:** copying wide text to unbounded narrow
  staging can add allocation and end-pointer defects. Define a shared
  character-reader numeric core or a bounded, checked adapter, then replay the
  T2 exact hexadecimal and range vectors through wide entry points.
- **Surrogate semantics — high:** 16-bit `wchar_t` is a UTF-16 code unit, not a
  full Unicode scalar. Test Windows memory/calling interoperability and paired
  code-unit preservation separately from C-locale conversion, which rejects
  code units above 255.
- **C89 isolation/build risk — high:** the library build uses C89 source mode
  and TinyCC's `__STDC_VERSION__` is not a reliable edition selector. Use
  `WCRT_C89`, C89-compatible production syntax, absence probes, and all source
  enumeration audits.
- **Architecture evidence — high:** x64 cannot substitute for x86 varargs or
  native ARM64 stream behavior. Execute ABI and behavioral matrices natively
  on all three targets; classify cross-linking as interim evidence only.
- **Legacy import risk — medium:** production calls to newer Unicode helpers
  could break Windows 2000 x86. Prefer internal conversion algorithms and run
  the import allowlist after every final architecture build.
- **Evidence retention — medium:** current local JSON is ignored and mutable.
  Retain exact-revision, per-target CI artifacts with toolchain identity,
  commands, timestamps, failures, corrections, and reruns.
- **T2 dependency evidence — medium:** native ARM64 and independent T2 review
  are still open. Close or explicitly carry those obligations before T3 is
  represented as target-complete.

## Completion Criteria

T3 is complete only when all of the following are true:

- REQ-0033 and TC-0033 are reviewed and baselined before any production wide
  source; proposed ADR-0003 is accepted or superseded with a compatibility
  impact analysis.
- REQ-0031/TC-0031 and REQ-0032/TC-0032 are reviewed before their production
  implementation, with every C99 7.24 and 7.25 facility inventoried.
- Impact updates to REQ-0006, REQ-0012, REQ-0013, REQ-0015, REQ-0022,
  REQ-0027, REQ-0029, REQ-0030, and REQ-0039 are reviewed and traceable.
- `<wchar.h>` and `<wctype.h>` are self-contained and repeatable in C99 mode;
  their C99-only names do not leak into the selected C89 library surface.
- The accepted `wchar_t`, `wint_t`, `mbstate_t`, `WEOF`, `WCHAR_*`, and
  `WINT_*` definitions have exact compile-time and native ABI evidence on x86,
  x64, and ARM64.
- Every implementation-defined encoding, invalid-code-unit, locale, initial-
  state, internal-state, stream-orientation, external representation,
  positioning, and surrogate policy is published in controlled records.
- Restartable conversions pass null, zero-length, complete, incomplete,
  invalid, length-only, capacity-stop, source-pointer, errno, and independent
  internal-state partitions without out-of-bounds access.
- Wide strings, memory, numeric conversions, collation, time formatting, and
  all classification/mapping descriptors pass boundary and negative tests.
- Wide formatted output and input pass every applicable conversion/length
  matrix, direct and `va_list` paths, exact argument/destination types,
  width/precision/buffer rules, matching/input failure, and stream-remainder
  behavior.
- Fresh/open/predefined/reopened streams pass the orientation state matrix;
  EOF/WEOF, error, pushback, CR-LF, binary/text, position, and no-transfer
  conflict behavior are dispositioned without changing valid narrow behavior.
- Windows UTF-16 memory and calling interoperability passes natively on x86,
  x64, and ARM64, including BMP code units and unmodified surrogate pairs.
- The complete C89, earlier C99, Microsoft compatibility, startup, consumer,
  build, source-quality, traceability, evidence, WSP tool, and Windows 2000 x86
  import gates remain green.
- Personal design, code, and test review is complete; every finding has a fix,
  approved deferral, or accepted-risk record with verification.
- Retained evidence identifies the exact source/test revisions, target,
  configuration, operating system, TinyCC identity, commands, timestamps,
  outcomes, and artifact locations. Native ARM64 execution is mandatory.
- Actual size, phase effort, defect/fix effort, quality activity, residual
  risks, estimate variance, and the tranche postmortem are recorded.
- No `wWinMain` source, declaration, object, test, or completion claim is added.

## Quality Plan and Tests Specified Before Implementation

### Requirements and design review

- Review C99 as corrected through TC3: 7.8.2.4, 7.18.3, 7.19.2, 7.20.7,
  7.20.8, 7.23.3.5, 7.24, and 7.25. Maintain a clause/function inventory and
  reconcile it against each header declaration, source definition, test, and
  manifest row.
- Review the accepted type/promotion/sentinel table, public type ownership,
  include graph, structure size/alignment/offset table, stream transition
  table, conversion transition table, and exact wide-format varargs table
  before code review begins.
- Review all proposed implementation-defined choices against the C89
  requirements and the 1.0 conformance-profile obligations. Record ISO
  deviations separately from Windows/UCRT compatibility differences.

### TC-0033 ABI and conversion matrix

- Compile header-only type identity, size, alignment, signedness, integer-
  promotion, constant-expression, `WEOF`, `WCHAR_*`, `WINT_*`, all-zero state,
  repeated-inclusion, C89-absence, and cross-header-order probes.
- Exercise `mbrlen`, `mbrtowc`, `wcrtomb`, `mbsinit`, `mbsrtowcs`, and
  `wcsrtombs` through caller state and null internal state: null character,
  ordinary byte, 0x80/0xff, `n == 0`, destination-null length query, zero and
  exact capacities, capacity stop before a character, source pointer update,
  reset, invalid wide code unit, `EILSEQ`, and independent internal objects.
- Cross-check the old C89 `mblen`, `mbtowc`, `wctomb`, `mbstowcs`, and
  `wcstombs` behavior against the accepted encoding model.
- Link and execute a Windows wide-API probe that consumes WCRT `wchar_t`
  arrays, verifies code-unit count and layout for BMP data and a surrogate
  pair, and introduces no production dependency. Run it natively per target.

### TC-0031 stream, library, and format matrix

- Verify all 59 `<wchar.h>` declarations, qualifiers, return types, macros,
  incomplete `struct tm`, `FILE`/`va_list` usability, C89 absence, and every
  definition's export presence.
- Drive streams through unopened-slot reuse, fresh/unoriented, explicit
  positive/negative `fwide`, first byte operation, first wide operation,
  repeated query, `freopen`, close/reuse, position save/restore, EOF, error,
  byte pushback, wide pushback, and conflict partitions. Run text CR-LF,
  binary, redirected standard-input, and standard-stream child cases.
- Test unformatted functions at empty input, one character, newline, exact
  array capacity, short read, invalid output code unit, WEOF, pushback at EOF,
  repeated pushback, and first-unread preservation.
- Test `wcs*` and `wmem*` at empty, one, prefix, exact count, unterminated
  bounded arrays, embedded nulls for memory functions, overlap only where
  permitted, token continuation, no match, high code units, and guard regions.
- Replay integer, hexadecimal floating, infinity/NaN, signed-zero, rounding,
  range, end-pointer, and errno vectors through all wide numeric functions;
  include `wcstoimax`/`wcstoumax` and cross-check narrow/wide equivalence for
  C-locale spellings.
- Verify every C99 time conversion required by `wcsftime`, `E`/`O` behavior in
  the C locale, ISO week/year boundaries, length-only reasoning, exact and
  insufficient buffers, terminator rules, and equivalence with `strftime`.
- For formatted wide output, use a conversion-by-length decision table for
  integer, floating, character, string, pointer, `%n`, width, precision, flags,
  direct/`va_list`, stream, and bounded-wide destinations. Distinguish wide
  `c`/`s` rules from byte-oriented rules and keep `swprintf` bounds separate
  from `snprintf` and `_snprintf`.
- For formatted wide input, cover string, file, and standard input; exact
  destination types; matching versus input failure; suppression; `%n`;
  scansets; width at lexical boundaries; invalid conversion to narrow output;
  first-unread wide characters; repeated calls; and direct/`va_list` paths.

### TC-0032 classification and mapping matrix

- Test all twelve named C99 classes directly and through `wctype`/`iswctype`,
  including `blank`, every required descriptor string, invalid/empty/case-
  changed names, and descriptor equivalence.
- Test lower/upper functions directly and through `wctrans`/`towctrans`, both
  required mapping names, invalid descriptors/names where defined, identity
  mapping, and locale stability.
- Partition values across WEOF, null, ASCII boundaries, 0x7f, 0x80, 0xff,
  values above the C-locale multibyte range, surrogate code units, and maximum
  `wchar_t`. Do not invoke standard functions with arguments outside their
  specified domain.

### Personal review and formal verification

- Perform focused reviews of public include cycles and guards, exact ABI
  types, integer promotions, varargs ownership, state transitions, source-
  pointer updates, buffer arithmetic, terminators, overlap, stream indicators,
  pushback, orientation, position restoration, locale capture, and every
  allocation/error exit before formal verification.
- Compile with warnings as errors in C99 header/test mode and C89 production
  mode. Inspect symbols and imports, run `git diff --check`, PowerShell parsing,
  traceability, evidence validation, source documentation, WSP tool tests, all
  native available targets, and ARM64 CI.
- Preserve each formal failure before correction and link it to the defect and
  successful rerun. Cross-build evidence shall never be labeled native
  execution evidence.

## Exact Implementation Order

1. Review and approve this proposed T3 work baseline, size/effort range,
   exclusions, and risk reserve; record any change before implementation.
2. Resolve the `wint_t` decision. Accept proposed ADR-0003 or replace it with
   an explicit ISO/UCRT compatibility decision and impact analysis.
3. Draft and review REQ-0033 first, including exact scalar/state ABI tables,
   C-locale encoding rules, initial/internal state, stream model, errors,
   surrogate policy, and all architecture invariants.
4. Draft and review TC-0033 with compile-time ABI, state-transition,
   invalid/incomplete conversion, Windows calling, and native-target matrices.
5. Record REQ-0033 impacts in REQ-0006, REQ-0012, REQ-0013, REQ-0022,
   REQ-0029, and REQ-0030; update ADR-0002 and the platform model consistently.
6. Draft and review REQ-0031 from a complete 7.24 inventory. Include all 59
   functions and the dependent `wcstoimax`/`wcstoumax`, wide REQ-0039
   allocation, and C99 time-format impact.
7. Draft and review TC-0031 with the exact stream, conversion, string/memory,
   numeric, time, formatted-output, formatted-input, and buffer matrices above.
8. Draft and review REQ-0032 and TC-0032 with all 18 functions, descriptor
   values, mappings, C-locale partitions, WEOF, and target consistency.
9. Add presence, C89-absence, behavior, architecture, and runner/manifest tests
   for TC-0033, TC-0031, and TC-0032 before production implementation. Run and
   retain the expected failing entry results without weakening an oracle.
10. Implement the shared public type ownership and ABI first: `<wchar.h>` type
    foundation, `wint_t`, `mbstate_t`, `WEOF`, `<stdint.h>` WINT macros,
    include-order guards, and compile-time target assertions.
11. Implement the C-locale conversion state core and restartable single-
    character/string conversions; refactor the C89 conversion wrappers to use
    the same encoding rules without changing their valid results.
12. Add orientation, captured encoding/state, and wide pushback to the private
    stream core. Update open, standard-stream initialization, byte-I/O entry,
    position, reopen, close/reuse, and error handling before adding wide I/O.
13. Implement `fwide` and unformatted wide stream I/O, then verify orientation,
    WEOF/EOF, text/binary translation, pushback, position, and conflict
    behavior in isolation.
14. Implement wide memory, string, search, token, comparison, collation, and
    transformation functions; complete their guard/overlap/boundary tests.
15. Generalize or safely adapt the numeric text core for wide input; implement
    all seven `<wchar.h>` numeric functions plus `wcstoimax`/`wcstoumax`, then
    replay the REQ-0027/REQ-0028/REQ-0039 oracles.
16. Complete the C99 `strftime` impact needed by `wcsftime`, verify the narrow
    time regression, then implement and verify `wcsftime`.
17. Implement wide formatted output over reviewed wide sinks and the shared
    numeric/exact-type core. Complete stream and `swprintf` paths, all `v`
    entry points, character/string inversions, and count/buffer checks.
18. Implement wide formatted input over wide string/stream sources. Complete
    failure-state, first-unread, scanset, width, exact destination, conversion-
    to-narrow, and all `v` entry points.
19. Implement C-locale `<wctype.h>` direct classification/mapping functions,
    then descriptors and direct/descriptor equivalence.
20. Rework the T2 `lc`/`ls`/`l[` bridge to use the accepted conversion and
    exact `wint_t` ABI. Rerun focused REQ-0029/REQ-0030 tests before proceeding.
21. Run TC-0033, TC-0031, TC-0032 and their focused impacted regressions on x86
    and x64; cross-compile/link ARM64 and label it interim only. Disposition
    every failure before expanding verification.
22. Perform the complete personal design/code/test review, including written
    state walks for partial/invalid/EOF/orientation paths and cell-by-cell
    varargs, buffer, and ABI audits. Record defect and fix effort.
23. Run the full local C89/C99/compatibility, build, consumer, startup,
    source-quality, traceability, evidence, dependency, PowerShell, WSP-tool,
    and Windows 2000 x86 import gates.
24. Run native x86/x64/ARM64 CI for the exact revision, retain complete
    evidence, and obtain independent review. Do not substitute local ARM64
    compile/link for native execution.
25. Update requirement implementation records, platform/encoding records,
    actual size/effort, checklist, defect dispositions, evidence inventory,
    residual risk, postmortem, and remaining-program estimates. Stop before
    `wWinMain`, commit, push, release, or publication unless separately
    authorized.

## Implementation Personal Review and Residual Risk

The local patch received personal requirements, architecture, code, test,
compatibility, buffer, and evidence review. The review walked public type
ownership and include order; exact promoted and destination types; independent
conversion states; partial, invalid, reset, and source-pointer transitions;
stream orientation, EOF/WEOF, pushback, reopen, position, and no-transfer
conflicts; format/scan `c`/`s` inversions; numeric and time adapters; allocation
and terminator exits; and x86/x64/ARM64 source enumeration. Findings T3-D001
through T3-D017 have a correction, explicit open evidence obligation, or
accepted-risk disposition below.

The review found and removed wrong wide `c`/`s` modifier semantics, two unsafe
attempts to reuse byte scanning for arbitrary in-memory UTF-16, a stream-scan
modifier handoff error, an empty-time-result ambiguity, test-oracle mistakes,
and an ARM64 cross-runner source omission. Focused regressions were added or
strengthened before the final aggregate runs. No `wWinMain` declaration,
source, object, or test was added.

Residual local implementation risk is **medium**. The highest remaining
behavioral uncertainty is native ARM64 execution, especially variadic and
stream-state paths that local cross-linking cannot execute. Wide stream scan
formats are deliberately restricted to code units representable in the
captured C-locale external encoding; TC-0031 verifies the documented
`EILSEQ`/no-transfer result, but a future broader encoding or fully
encoding-independent format parser requires impact review. Ad hoc parallel
focused runners also share architecture output directories and must be
serialized. Immutable exact-revision CI evidence and independent review are
unavailable and prevent target-complete acceptance.

## Entry Evidence

- The repository was clean at inspected revision `98b568b` before these
  planning records were created.
- Existing generated capability records report TinyCC 0.9.28rc and two-byte
  `wchar_t` for x86, x64, and ARM64. They are useful inspected evidence but are
  ignored mutable output, not immutable T3 acceptance evidence.
- Existing x86/x64 extension JSON predates this planning record and contains no
  T3 cases. ARM64 has capability and compile/link evidence but no T3 or native
  behavior result.
- No T3 test was executed and no production header or source was changed during
  this planning pass.
- WG14/N1256 was used to inventory C99 7.24/7.25 and identify the `wint_t`,
  `wcstoimax`/`wcstoumax`, stream orientation/state, and `wcsftime` obligations.

## Controlled Implementation Entry Evidence

On 2026-08-09, after the requirements and test specifications were baselined
and before any production header or source change, the new x64 runners were
executed with the repository TinyCC package. TC-0033 and TC-0031 failed at the
controlled header gate because `include/wchar.h` was missing; TC-0032 failed at
the same gate because `include/wctype.h` was missing. Those are the expected
entry failures and establish that the tests do not silently use host wide
headers. The commands were:

- `tests/c99/run-tc-0033.ps1 -TinyCc ../tcc_package/out/build/x64-debug/package/tcc.exe`
- `tests/c99/run-tc-0031.ps1 -TinyCc ../tcc_package/out/build/x64-debug/package/tcc.exe`
- `tests/c99/run-tc-0032.ps1 -TinyCc ../tcc_package/out/build/x64-debug/package/tcc.exe`

## Local Implementation Evidence

The staged implementation was reviewed and exercised on 2026-08-09 with the
repository TinyCC 0.9.28rc packages:

- TC-0033, TC-0031, and TC-0032 pass presence, C89-isolation, behavior, ABI,
  and Windows `lstrlenW` interoperation on native x86 and x64. All controlled
  sources and probes compile/link for ARM64 through explicit `-CompileOnly`
  modes; execution is recorded as not run.
- Full TC-0001 through TC-0015 C89 aggregates pass on x86 and x64. The ARM64
  C89 cross aggregate passes all fifteen units after T3-D014 was corrected.
- The complete extension aggregate passes all 23 C99 and Microsoft
  compatibility cases on x86 and x64, including the T2 narrow format/scan
  regressions and the three T3 cases.
- Fresh Release DLL, static library, and startup-object builds pass for x86,
  x64, and ARM64. Built-library consumers execute on x86/x64 and compile/link
  for ARM64. Startup-object consumers execute on x86/x64. Native ARM64 startup
  and behavior execution remain CI obligations.
- The Windows 2000 x86 import allowlist reports no unexpected imports. WSP
  common-tool tests pass, traceability reports 41 requirements, 41
  specifications, and 41 implementations, and TC-0016 reports zero violations
  across 157 C/header files. `git diff --check` reports no patch errors.

Generated local aggregate and build output remains ignored developer evidence.
It identifies a dirty implementation derived from the accepted T3 baseline,
not the immutable post-commit revision. It therefore cannot substitute for
the required native ARM64 CI results, retained per-target metadata, or
independent review.

## Time Log

| Date | Phase | Focused minutes | Excluded interruption | Note |
| --- | --- | ---: | --- | --- |

No focused minutes are inferred from chat duration, shell wall time, or
automated execution. The maintainer may add observed phase time.

## Defect Log

| ID | Type | Affected baseline | Observed behavior | Priority | Injected | Removed | Fix minutes | Owner and disposition / verification |
| --- | --- | --- | --- | --- | --- | --- | ---: | --- |
| T3-D001 | interface/ABI | ADR-0002 / C99 platform model | Planned 16-bit `wint_t` is changed by integer promotions and conflicts with C99 7.24.1. | Critical | T2 design | T3 specification | - | Removed by accepted ADR-0003/REQ-0033 and verified natively on x86/x64 and by ARM64 compile/link; native ARM64 execution remains open. |
| T3-D002 | requirements | REQ-0027 | Requirement incorrectly excludes `wcstoimax` and `wcstoumax`, which C99 7.8.2.4 requires. | High | T1 requirements | T3 implementation | - | Removed by REQ-0031 declarations, implementations, presence checks, and numeric tests; x86/x64 pass and ARM64 compile/link passes. |
| T3-D003 | documentation | C99 work-plan T3 estimate | The estimate counts neither the full function inventory nor test/evidence/integration size. | High | Program planning | Planning/postmortem | - | Removed from the planning basis by the inspected range and compared with the staged snapshot below. |
| T3-D004 | documentation | REQ-0013 implementation record | Scope names `src/internal/mbstate.h`, but no such file or state object exists. | Medium | C89 requirements | Requirements closeout | - | Removed by correcting the impact record to the actual public state and conversion units; no fictional internal file was added. |
| T3-D005 | design | REQ-0012 stream core | `FILE` has no orientation, conversion state, or wide pushback and byte paths cannot protect orientation. | Critical | C89 stream design | Implementation | - | Removed by private stream state and centralized orientation checks; transition/no-transfer, reopen, position, pushback, C89, and compatibility tests pass x86/x64. |
| T3-D006 | requirements | REQ-0015 / REQ-0031 | `wcsftime` requires a C99 time-format surface while the controlled `strftime` implementation was C89-only. | High | Tranche allocation | Implementation/test | - | Removed by completing the C99 C-locale time-format delta before `wcsftime`; narrow and wide boundary tests pass. |
| T3-D007 | requirements | REQ-0039 | Wide numeric text was allocated later despite complete T3 wide numeric and formatted I/O depending on it. | High | T2 tailoring | Requirements/implementation | - | Removed by the controlled T3 allocation and checked wide adapters; exact numeric, end-pointer, malformed, and range cases pass. |
| T3-D008 | test | T2-to-T3 dependency | T2 lacks retained native ARM64 and independent review evidence. | Medium | T2 verification | Open | - | Maintainer; carried with T3 rather than represented as closed. Native ARM64 CI and independent review remain required. |
| T3-D009 | interface/ABI | Initial wide formatter | Wide output initially treated unmodified `c`/`s` as wide and `l` forms as narrow, reversing C99 wide-format rules. | High | Implementation | Test/personal review | - | Removed by exact `int`/`wint_t` and narrow/wide pointer selection; direct and `va_list` matrices pass x86/x64 and compile/link ARM64. |
| T3-D010 | design/state | Initial wide stream scanner | A provisional adapter temporarily routed a wide-oriented stream through public byte scanning, violating orientation and state ownership. | Critical | Implementation | Personal design review | - | Removed by the internal wide-input scanner mode using `fgetwc`/`ungetwc`; no public byte operation is used and orientation tests pass. |
| T3-D011 | data/encoding | Initial `swscanf` adapter | Narrow staging replaced UTF-16 code units above 255, so in-memory high literals, strings, and scansets could not be preserved. | High | Implementation | Test | - | Removed by a native UTF-16 string scanner; high-code-unit literal and character/string tests pass. |
| T3-D012 | test | Initial TC-0031 | Two expected counts and distinct-string-literal pointer comparisons produced false test failures. | Medium | Test implementation | Test review | - | Removed by recalculated counts and content/index comparisons; controlled reruns pass. |
| T3-D013 | algorithm | Initial `wcsftime` adapter | A successful empty result was indistinguishable from a narrow formatting failure returning zero. | High | Implementation | Test/personal review | - | Removed with a destination sentinel and explicit empty-format/empty-result tests. |
| T3-D014 | build/tooling | ARM64 C89 cross runner | TC-0015 omitted `src/errno.c`, causing unresolved `wcrt_errno` after the time impact was linked. | High | Existing runner | Full verification | - | Removed by adding the missing controlled source; all fifteen ARM64 C89 cross units rerun and pass. |
| T3-D015 | test infrastructure | Focused T3 runners | Parallel focused runs share an architecture build directory and can collide with transient permission errors. | Medium | Test design | Accepted operational constraint | - | Serialize focused runners; aggregate and CI entry points are serial. A future isolated-output runner change is recommended. |
| T3-D016 | interface/ABI | Initial wide stream scan adapter | Modifier translation changed wide `l` character destinations back to wide objects, breaking the required inverse destination rule. | High | Implementation | Integration test | - | Removed by preserving the format modifier and selecting destinations from wide-input mode; high and ordinary stream scans pass. |
| T3-D017 | conformance/encoding | Wide stream scan formats | Format code units above 255 cannot be represented by the captured C-locale stream encoding. | Medium | T3 encoding scope | Accepted documented risk | - | REQ-0031 documents `EILSEQ` before transfer; TC-0031 verifies no input is consumed. Revisit with any broader encoding or independent wide-format parser. |

No fix minutes are recorded because maintainer stopwatch data was not
provided. Planning diagnosis time is not inferred.

## Planning Checklist

- [x] Repository status, process, work plan, prior tranche logs, requirements,
      source, tests, build scripts, platform records, and available evidence
      were inspected before estimating.
- [x] C99 7.24/7.25 function inventory and dependent 7.8, 7.18, 7.19, 7.20,
      and 7.23 obligations were reconciled.
- [x] Scope, exclusions, size, phase effort, reserve, dependencies, risks,
      completion criteria, quality plan, test design, and exact order are
      proposed here.
- [x] A durable ABI/encoding/stream proposal was recorded in ADR-0003 before
      any production-source change.
- [x] Proposed ADR-0003 is reviewed and accepted or superseded.
- [x] REQ-0033 and TC-0033 are baselined.
- [x] REQ-0031/TC-0031 and REQ-0032/TC-0032 are baselined.
- [x] Controlled tests are implemented and failing for the expected missing
      behavior before production implementation.
- [x] Production implementation and personal review are complete for the
      local working patch.
- [x] Native x86/x64 verification and ARM64 compile/link verification are
      complete.
- [ ] x86/x64/ARM64 verification, retained evidence, and independent review
      are complete.
- [x] Actual size, unavailable effort, residual risk, and local postmortem are
      recorded.

## Postmortem

**Completed:** 2026-08-09, using the staged diff, local test output, personal
review findings, empty time log, and defect log. This is a local postmortem,
not a claim that the tranche has met its supported-target completion gates.

### Planned versus actual

| Measure | Plan | Actual evidence | Assessment |
| --- | --- | --- | --- |
| Controlled scope | Three primary requirement/test units, nine existing-requirement impacts, approximately 79 behaviors plus ABI/build/evidence work | REQ-0033, REQ-0031, and REQ-0032; nine impact records; ADR/platform, implementation, runner, and build integration | Matched the controlled functional scope. |
| Artifacts | 40-56 source-controlled artifacts | 64 artifacts | Eight above the upper bound; risk separation added files while shared runners limited line volume. |
| Runtime/header change | 4,000-6,800 lines | 2,318 changed lines across `src/` and `include/` | 1,682 below the lower bound because the accepted C locale is stateless and existing numeric/scan cores were adapted. |
| Test/runner change | 2,600-4,500 lines | 935 changed lines under `tests/` | 1,665 below the lower bound; dense focused cases and one shared runner covered the matrix. |
| Controlled documentation/integration | 1,200-2,000 lines | 908 changed lines across controlled records and build integration | 292 below the lower bound; impact notes reused existing controlled records. |
| Total change | 7,800-13,300 changed lines | 4,033 inserted, 128 removed, 4,161 changed lines | 3,639 below the lower bound; the forecast overestimated code density, not functional scope. |
| Phase effort | 226-376 focused hours plus reserve | Unavailable: the time log has no focused-minute entries | No valid effort variance can be calculated. |
| Schedule | No completion date without weekly capacity | Unavailable: no focused capacity or phase boundaries were recorded | No schedule variance can be calculated. |

The size figures are a staged-diff snapshot at local postmortem completion;
ignored generated output and commit metadata are excluded. The implementation
is materially smaller than forecast because the stateless one-byte encoding
needs no transition tables or dynamic codec, existing numeric and scanning
cores were safely adapted, descriptor data is integral, and the focused tests
use shared runners. The higher artifact count reflects splitting the risky
wide conversion, I/O, formatting, scanning, string, time, and classification
concerns into reviewable units. Without observed effort, size variance alone
does not establish a new productivity or schedule model.

### Defects and quality activity

Eight entry findings were recorded from planning and inspected baselines.
Seven are removed in this patch; T3-D008 remains open because T2 and T3 still
lack native ARM64 retained evidence and independent review. Nine additional
implementation, test, integration, or conformance findings were recorded.
Seven were removed before final local verification; T3-D015 is an accepted
serialized-runner constraint and T3-D017 is a documented C-locale format
restriction. No known defect escaped x86/x64 local verification. That is not
evidence of zero ARM64 or CI escapes.

Quality activity completed includes controlled requirement/test and ADR
review; test-before-production entry failures; focused native x86/x64 and
ARM64 compile/link verification; full x86/x64 C89 and extension/compatibility
aggregates; the ARM64 C89 cross aggregate; Release builds and consumers on all
three targets; x86/x64 startup execution; Windows API ABI probes; the Windows
2000 x86 import gate; export inspection; source quality; traceability;
PowerShell parsing; WSP tool tests; and `git diff --check`. Native ARM64
behavior/startup execution, immutable per-target exact-revision evidence, and
independent review remain incomplete and prevent target-complete acceptance.

### Estimate disposition

The original T3 roadmap estimate was correctly rejected as incomplete, but
the inspected replacement substantially overestimated implementation and test
line volume. The accepted architecture eliminated the largest potential codec
and state-machine expansion, and shared T2 numeric/scan cores reduced duplicate
code. Future estimates for similarly broad standard-library surfaces should
inventory functions and evidence as T3 did, then estimate simple stateless
families separately from parser/stream cores instead of applying one density
range to the complete inventory. The focused-hour estimate is not revised
without observed effort data.

### Process improvements adopted

1. Wide and narrow formatted-I/O reviews shall use a cell-by-cell table of
   function family, modifier, promoted argument, destination type, and
   character encoding before implementation and again before aggregate tests.
2. Stream adapters shall include an explicit orientation/state ownership walk;
   no public operation of the opposite width may be used as an internal reuse
   path.
3. Empty-success results, malformed longest input items, first-unread state,
   and high in-memory code units shall be mandatory adapter tests rather than
   inferred from ordinary examples.
4. Focused architecture runners should receive isolated output directories
   before parallel execution is admitted. Until then they are serialized.
5. The maintainer should record focused phase minutes and preserve each final
   target run with revision, toolchain, command, timestamp, and artifact path;
   missing observations remain unavailable rather than inferred.

No completion date, actual focused effort, native ARM64 execution, retained
exact-revision evidence, independent review, release readiness, or `wWinMain`
work is claimed.
