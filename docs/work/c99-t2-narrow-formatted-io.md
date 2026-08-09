# C99 T2 Narrow Formatted I/O Work Log

**Status:** Local postmortem complete; native ARM64 execution, retained CI
evidence, and independent review pending

**Owner:** WCRT maintainer

**Created:** 2026-08-09

**Inspected source baseline:**
`15db51d3ee4e8ec95549c0ac612cc614ada7b10c`

## Baseline and Scope

T2 will baseline and implement REQ-0029 (C99 narrow formatted output),
REQ-0030 (C99 narrow formatted input), and the narrow numeric-text allocation
of REQ-0039 for the supported x86, x64, and ARM64 Windows targets.

REQ-0029 includes the `hh`, `ll`, `j`, `z`, and `t` length modifiers for every
applicable integer and `%n` conversion; `%a`, `%A`, and `%F`; the `L` floating
argument path; and the C99 `l` character-conversion forms of `%c` and `%s` for
the supported C locale. The formatting rules apply consistently to the
`printf`, `fprintf`, `sprintf`, and `v*` families and to the formatting stage
shared by the independently controlled bounded functions.

REQ-0030 includes the same applicable integer and `%n` modifiers, all C99
floating conversion spellings, the `l` character-conversion forms of `%c`,
`%s`, and `%[`, correct matching/input-failure and assignment-count behavior,
and `vfscanf`, `vscanf`, and `vsscanf`.

The T2 allocation of REQ-0039 includes hexadecimal floating subject sequences,
`INF`/`INFINITY`, `NAN` subject sequences, signed zero, rounding and range
behavior for `strtof`, `strtod`, and `strtold`, and their narrow formatted-I/O
integration. The controlled REQ-0039 record must state exactly what remains for
T6 rather than representing the entire backlog item as complete.

The following are explicit regression obligations, not contracts merged into
REQ-0029 or REQ-0030:

- REQ-0012 C89 formatted I/O and stream behavior;
- REQ-0019 C99 `snprintf`/`vsnprintf` bounds, termination, length-only, and
  would-have-written return semantics;
- REQ-0020 Microsoft `_snprintf`/`_vsnprintf` exact-limit, truncation,
  termination, and null-buffer query semantics;
- REQ-0027 execution of the applicable `PRI*` and `SCN*` macro spellings; and
- REQ-0028 decimal conversion, end-pointer, range, and error behavior.

T2 excludes the wide formatted-I/O functions, complete `<wchar.h>` and
`<wctype.h>`, stream orientation, non-C locales, public C99 math and
floating-environment families, complex conversion, release signing, commit,
push, and publication. Correcting shared formatter/scanner defects is included
when the defect prevents the requested T2 behavior or a named regression gate;
unrelated stream and filesystem work remains outside T2.

**Controlled size:** Three requirement records and three matching controlled
test specifications, with impact updates to the independently controlled
requirements above.

**Estimated change size after repository inspection:** 30-42 artifacts;
1,200-2,100 runtime/header LOC, 1,100-1,900 test/runner LOC, and 700-1,100
controlled-document/integration lines. Replacing unsafe shared paths is
expected to produce **3,200-5,600 changed lines** in the complete patch.

**Forecast:** **130-225 focused hours**. No completion date is committed
because weekly focused capacity and maintainer-recorded phase time are not
available.

| Phase | Planned focused hours |
| --- | ---: |
| Plan and estimate | 6-10 |
| Specify controlled requirements and tests | 12-20 |
| Design and design review | 18-30 |
| Implement | 48-85 |
| Personal code and test review | 16-28 |
| Verify and retain evidence | 25-43 |
| Postmortem and re-estimate | 5-9 |
| **Total** | **130-225** |

**Observed final-local-verification size snapshot:** 28 source-controlled
artifacts, 3,049 inserted lines, 312 removed lines, and 3,361 changed lines.
This is two artifacts below the planned lower bound, although its changed-line
size is within the planned range. The numeric work stayed in `stdlib.c`, the
formatter and scanner cores were replaced in place, and the three cases share
one runner. No actual focused-hour value is inferred.

## Repository Reconciliation and Discrepancies

- REQ-0029, REQ-0030, and REQ-0039 are still planned identifiers. No
  `req-0029`, `req-0030`, or `req-0039` record, matching controlled
  specification, implementation runner, manifest row, or retained result
  exists. Source behavior is not evidence for these planned requirements.
- The REQ-0030 backlog row cited 7.19.6.10 for the new variadic input
  functions, but that clause specifies `vprintf`. The official C99 TC3 draft
  places `vfscanf`, `vscanf`, and `vsscanf` in 7.19.6.9, 7.19.6.11, and
  7.19.6.14; the planning row is corrected with this baseline.
- `src/stdio_format.c` and `src/stdio_scan.c` partially parse `ll`, and
  `inttypes.h` publishes `ll`-based format macros. That execution surface was
  implemented ahead of its controlled requirement and is not exercised by
  TC-0027, which verifies macro spellings only.
- The output parser does not recognize `hh`, `j`, `z`, or `t`; does not
  implement `%a`, `%A`, or `%F`; and ignores `l` for character conversions.
  The scanner has the corresponding gaps, and `<stdio.h>` declares none of
  `vfscanf`, `vscanf`, or `vsscanf`.
- The current floating formatter truncates fractional digits instead of
  rounding, does not distinguish signed zero, NaN, or infinity, casts the
  integral part of every finite value to `unsigned long long`, implements only
  a partial `%e`/`%g`, and stores the result in a fixed 128-byte array. Large
  finite values and caller-controlled precision therefore cannot be completed
  by adding conversion letters to the current routine.
- Integer output omits required precision interactions and several alternate-
  form zero cases. Width and precision accumulation and the logical `int`
  output count can overflow. These common-path issues affect REQ-0012 and both
  bounded contracts when T2 expands the supported format matrix.
- The scanner retrieves unsigned destination pointers for signed integer
  conversions, treats its partial `ll` path as unsigned decimal-only, and
  retrieves `double *` for both `%lf` and `%Lf`. These are `va_arg` type and
  pointed-to-type defects even where the Windows representation happens to
  make a sample appear to work.
- Numeric scan width is checked only after unbounded `strtoul` or `strtod`
  consumption. A valid width-limited prefix can therefore be reported as a
  matching failure. Sign, base prefixes, exponent rollback, and malformed
  subject sequences are not modeled consistently.
- Stream scanning reads a complete line or up to 4095 bytes into a staging
  array before matching, discards unconsumed bytes, and imposes a limit not
  present in the string scanner. This prevents correct repeated `fscanf` calls,
  exact input-failure behavior, and retained stream state.
- `snprintf` and `vsnprintf` are guarded only by `__STDC_VERSION__`. The
  observed TinyCC defines a later value during project C89 compilation, so the
  declarations leak unless `WCRT_C89` is also honored. Existing C89 presence
  tests check required declarations but not absence of the C99 additions.
- Existing TC-0012, TC-0019, and TC-0020 tests are green but intentionally
  small. They do not cover the T2 modifier/conversion matrix, numeric
  classification, rounding, malformed scans, stream remainder, large
  length-only results, or a systematic `va_list` path matrix.
- The local x86/x64 aggregate JSON is ignored developer output and is
  overwritten on rerun. Its rows do not contain the full source revision,
  compiler identity, configuration, command, and per-test timestamps required
  of release evidence. CI artifact retention remains the authoritative way to
  close those evidence fields.
- `wchar_t` is already a 16-bit ABI type and C89 single-byte multibyte
  conversions exist, but `wint_t`, `mbstate_t`, and restartable conversions
  remain planned for T3. The `l` character forms therefore require a durable
  C-locale ABI decision that T3 must preserve.
- All three targets use binary64 `double` and `long double`; x86 is ILP32 and
  x64/ARM64 are LLP64. This reduces numeric-format variation but makes `z` and
  `t` argument types architecture-sensitive and does not permit retrieval by a
  merely same-sized type.
- Build and focused test runners enumerate source files explicitly. Adding a
  shared numeric-text implementation unit requires corresponding build,
  C89-cross, C99, and compatibility runner updates.
- The work plan's prior 639–1,101-hour program total did not equal its tranche
  rows; it was low by 5–10 hours before the T2 revision. The revised total is
  recomputed from every row rather than applying only the T2 delta.

## Proposed Controlled Baseline

| Unit | Proposed acceptance boundary | Planned verification | Key dependency |
| --- | --- | --- | --- |
| REQ-0029 / TC-0029 | C99 deltas for byte-oriented formatted output, including applicable `l` character forms; shared formatting behavior without redefining REQ-0019 or REQ-0020 bounds | Declaration/isolation inspection plus conversion, field, classification, rounding, sink, direct/`va_list`, and architecture matrices | REQ-0012, REQ-0019, REQ-0025, REQ-0027, C-locale wide ABI decision |
| REQ-0030 / TC-0030 | C99 deltas for byte-oriented formatted input and the three `v*scanf` functions, including stream consumption and applicable `l` character forms | Declaration/isolation inspection plus matching, input-failure, assignment, width, malformed-sequence, destination-type, stream-remainder, direct/`va_list`, and architecture matrices | REQ-0012, REQ-0025, REQ-0027, input-source design |
| REQ-0039 / TC-0039 | Narrow hexadecimal, infinity, NaN, signed-zero, rounding, and range conversion for `strtof`/`strtod`/`strtold` and formatted-I/O integration; remaining allocation explicitly deferred | Exact/adjacent boundary values, round trips, overflow/underflow, end pointers, `errno`, classification, precision, and cross-interface tests | REQ-0024, REQ-0028, binary32/binary64 model; later REQ-0035/REQ-0036 public environment |

The requirement review must resolve standard undefined cases rather than turn
them into accidental WCRT promises. In particular, tests will not require a
specific value for a `scanf` numeric conversion whose mathematical result is
not representable in the destination type. Defined range behavior will be
verified through `strto*`, floating classification, and representable scan
boundaries.

## Requirements and Design Review Results

- REQ-0029, REQ-0030, and the T2 allocation of REQ-0039 now have matching
  controlled TC-0029, TC-0030, and TC-0039 specifications and index entries.
- The official clause allocation is carried into the controlled records;
  `vfscanf`, `vscanf`, and `vsscanf` map to 7.19.6.9, 7.19.6.11, and
  7.19.6.14 respectively.
- ADR-0002 records the minimal C-locale bridge for `lc`, `ls`, and `l[` and
  binds the future public `wint_t` to the unsigned 16-bit Windows ABI. Full
  wide-library behavior remains isolated to T3.
- The existing formatter cannot be safely extended in place around its fixed
  conversion array and partial floating algorithm. Production work will first
  establish a bounded shared numeric representation and checked sink counts.
- The existing scanner line staging is rejected for T2. String and stream
  sources will share width-aware lexical state while retaining the first unread
  byte and exact failure state.
- Exact variable-argument and destination types are a reviewed design input.
  Same-size substitutions are prohibited on all three architectures.

## Design Review Baseline

1. Parse each directive into one explicit descriptor containing flags, width,
   precision, length enum, and conversion. Reject or classify malformed
   combinations before consuming a variable argument.
2. Use a reviewed conversion/length table to select the exact promoted
   `va_arg` type or exact destination pointer type. `intmax_t`, `uintmax_t`,
   `size_t`, `ptrdiff_t`, and their corresponding signed/unsigned forms remain
   distinct even where target widths agree. Retrieve `%L` as `long double` on
   every target despite the binary64 Windows representation.
3. Keep one formatting engine but separate output policies: unbounded string,
   stream, C99 bounded, and Microsoft legacy bounded wrappers retain distinct
   termination and return contracts. Logical counting must continue after a
   bounded sink stops storing bytes.
4. Do not allocate stack storage proportional to caller width or precision.
   Use a bounded internal numeric representation justified by binary32/binary64
   limits, emit padding and trailing zeroes through the sink, and check every
   count/width/precision arithmetic operation before conversion to `int`.
5. Centralize binary classification, exact hexadecimal decomposition, decimal
   digit generation, and rounding so output and `strto*` agree on signed zero,
   infinities, NaNs, normal/subnormal boundaries, and the documented
   round-to-nearest policy. `strtof` must avoid an unreviewed double-rounding
   path; `strtold` follows the documented binary64 ABI.
6. Replace line staging with an input-source abstraction that supports string
   and stream peek/get behavior, one-character lookahead, width limits, exact
   consumed counts, and preservation of the first unread byte. Conversion
   state must distinguish input failure from matching failure.
7. Share lexical numeric rules between `strto*` and scanning without copying an
   arbitrarily wide field. The scanner must apply its width during recognition,
   not reject a conversion after an unbounded parser has consumed it.
8. Implement `vfscanf`, `vscanf`, and `vsscanf` as the primary `va_list`
   entry points; variadic wrappers only establish/end their own list. Review
   `va_list` ownership and consumption separately from format semantics.
9. Implement `l` character forms against the supported C locale using the
   existing single-byte conversion behavior and a recorded `wint_t` ABI choice.
   This is not approval of the T3 wide-header, orientation, or locale scope.
10. If the shared numeric design or the `wint_t` choice creates a durable
    architecture constraint not adequately captured by the requirement and
    platform-model records, add an ADR before source implementation.

## Implementation and Personal Review Results

- ADR-0002 is accepted. The implementation uses the recorded 16-bit
  `wchar_t` ABI and permits only single-byte C-locale values in `lc`, `ls`,
  and `l[`.
- Hexadecimal `strtof` and `strtod` construct binary32 and binary64 bits
  directly. The review covered the normal/subnormal transition, halfway
  rounding, carry into the exponent, maximum finite result, signed zero, and
  special values. `strtold` deliberately follows the documented Windows
  binary64 model.
- The formatter now uses explicit length state and exact argument-selection
  helpers. The reviewed mapping is promoted `int` for `hh`/`h`, `long` for
  `l`, `long long`/`intmax_t` for `ll`/`j`, `ptrdiff_t` and `size_t` for the
  platform's signed/unsigned `z` and `t` forms, and the corresponding signed
  pointer for every `%n`. `%L` retrieves `long double` even though its target
  representation is binary64.
- The scanner uses the corresponding exact destination pointer types and
  primary `vfscanf`, `vscanf`, and `vsscanf` paths. Its tri-state conversion
  result distinguishes an assigned item, a successful suppressed item, and a
  matching failure; input failure is separately represented.
- The output review found no remaining precision-sized stack object. Radix
  digits use a proven 65-byte bound; ordinary fields use 96 bytes and switch
  to checked heap storage for larger integer precision. Floating capacity is
  checked before allocating precision plus the binary64 decimal bound. The
  sink never writes beyond its selected policy and rejects a logical count
  that cannot be returned as `int`.
- The scanner uses checked growable storage for only the current numeric input
  item. This is a deliberate implementation change from design-baseline item
  7, which proposed avoiding an arbitrarily wide copy entirely. It retains no
  line record, applies width during recognition, checks allocation growth, and
  restores its one-byte stream lookahead. A future streaming numeric parser
  could reduce resource use without changing the public contract.
- The formatter likewise uses checked heap materialization for
  caller-proportional precision rather than the baseline's fully bounded
  numeric representation. This removes the fixed-buffer defect and safely
  reports allocation/count failure, but it does not promise constant-memory
  formatting. The requirement and implementation records now describe the
  implemented policy rather than the initial design assumption.
- Personal review exposed and corrected copied-`va_list` consumption,
  negative-value decimal normalization, `%g` integer-zero trimming, integer
  precision allocation, suppressed-scan status, and two incorrect test return
  oracles. The findings and verification are recorded in the defect log.
- TC-0029, TC-0030, and TC-0039 execute successfully on x86 and x64. Their
  exact-type matrix covers all six integer conversions for `hh`, `ll`, `j`,
  `z`, and `t`; TC-0039 includes boundary spellings and 519 finite binary64
  `%a` round trips. All three cases compile/link for ARM64 but cannot execute
  on this x64 host.

## Dependencies, Risks, and Responses

- **Numeric correctness and estimate risk:** correctly rounded decimal/hex
  conversion, subnormals, and direct binary32 rounding are substantially larger
  than the current formatter. Review the algorithm and boundary oracle before
  code; use exact bit-pattern vectors and prove scratch-space bounds. Any
  imported algorithm requires license and provenance review.
- **Varargs ABI risk:** a same-width `va_arg` type is not interchangeable, and
  x64/ARM64 `z`/`t` differ from x86. Maintain a conversion-by-length matrix,
  review every cell, and execute typed sentinel cases on all architectures.
- **Character-conversion dependency:** C99 `l` character forms touch the future
  wide ABI. Baseline the minimal C-locale `wint_t`/`wchar_t` decision and obtain
  T3 compatibility review before implementation.
- **Scanner state risk:** malformed prefixes and exponents interact with the
  longest-input-item rule and stream lookahead. Specify expected consumed and
  unread bytes before implementing the source abstraction.
- **Buffer and count risk:** width, precision, length-only calls, and long
  fields are caller-controlled. Use checked arithmetic, bounded scratch, guard
  bytes, exact-limit tests, and repeated calls that prove stream remainder.
- **Contract-merging risk:** the shared sink invites accidental use of C99
  termination/return behavior for Microsoft names. Keep separate wrapper tests
  below, at, and above the bound in every formatter regression run.
- **C89-isolation risk:** TinyCC's language-mode macro is not sufficient.
  Compile direct absence probes with `WCRT_C89=1` and rerun the entire C89
  suite after header and source changes.
- **Evidence risk:** local x86/x64 output is useful entry evidence but is not
  target-complete release evidence. Record exact revision/toolchain metadata,
  preserve failures, and require retained native ARM64 CI results for closure.
- **External-target risk:** native ARM64 is unavailable on the current x64
  host. Local ARM64 compile/link is interim evidence only; a native
  `windows-11-arm` CI pass is a completion gate.
- **Legacy-import risk:** numeric code must not introduce host-CRT or newer
  Windows imports. Re-run the Windows 2000 x86 allowlist against a fresh DLL
  and inspect all new dependencies.

## Completion Criteria

- REQ-0029, REQ-0030, and the allocated REQ-0039 obligations are accepted
  with reviewed TC-0029, TC-0030, and TC-0039 specifications before the
  implementation is represented as conforming.
- Every required declaration is self-contained and correctly `restrict`-
  qualified in C99 mode; `vfscanf`, `vscanf`, `vsscanf`, `snprintf`, and
  `vsnprintf` are absent from the selected C89 surface.
- The applicable `hh`, `ll`, `j`, `z`, `t`, `L`, and character-conversion
  argument/destination types are correct on x86, x64, and ARM64, including
  every `%n` form and execution of representative `PRI*`/`SCN*` macros.
- `%a`, `%A`, and `%F`, hexadecimal floating input, infinity, NaN, signed zero,
  normal/subnormal boundaries, precision, rounding, overflow, underflow,
  end-pointer, and `errno` behavior meet the controlled requirements.
- Matching failures, input failures, assignment suppression/counts, `%n`,
  whitespace, widths, scansets, malformed subject sequences, EOF, and first-
  unread stream bytes meet the controlled input specification for string,
  file, standard-input, variadic, and direct entry points.
- Length-only and truncating C99 bounded output still returns the untruncated
  count and terminates every positive-size destination; the Microsoft path
  retains its separately specified exact-limit and truncation behavior.
- Personal design, code, and test reviews cover the complete `va_arg`/pointer
  matrix, integer/count overflow, scratch bounds, stream lifetime/state, and
  all findings recorded below. No material finding is closed without a fix,
  approved deferral, or accepted-risk record.
- TC-0012, TC-0019, TC-0020, TC-0027, TC-0028, all earlier C89/C99 and
  compatibility tests, startup, consumers, builds, source quality,
  traceability, evidence validation, WSP tool tests, and Windows 2000 x86
  imports are green on every applicable available target.
- x86 and x64 execute locally; ARM64 compiles/links locally and executes in
  native CI. Required CI evidence identifies the exact source revision,
  test-specification revision, target, configuration, operating system,
  TinyCC identity, timestamps, command/runner, outcome, and retained artifacts.
- Actual size and phase effort are recorded only from observed maintainer data;
  the postmortem compares the baseline with actuals and re-estimates T3-T6/R1.

## Quality Plan

- Review each requirement against C99 clauses 7.19.6.1, 7.19.6.2, 7.19.6.4,
  7.19.6.7, 7.19.6.9, 7.19.6.11, 7.19.6.14, and 7.20.1.3, and record impact
  on REQ-0012, REQ-0019, REQ-0020, REQ-0027, and REQ-0028.
- Review TC-0029, TC-0030, and TC-0039 before source changes. Use equivalence
  partitioning, boundary-value analysis, decision tables for conversion/length
  combinations, state-transition tests for scanning, and metamorphic/round-
  trip checks for numeric text.
- Test output through stream, unbounded string, C99 bounded, Microsoft bounded,
  direct variadic, and `va_list` paths. Use sentinel buffers around zero, one,
  exact, truncated, large-width, and length-only cases.
- Test input through string, file, and standard-input sources, including
  repeated stream calls that prove the first unread byte, EOF before/after an
  assignment, suppression, `%n`, malformed prefixes/exponents, and widths at
  every lexical boundary.
- Use exact binary32/binary64 bit-pattern vectors around zero, minimum
  subnormal, minimum normal, halfway cases, powers of two, maximum finite,
  overflow, infinity, and quiet NaN. Compare classification and round trips;
  use Microsoft UCRT only as supporting comparison, not as the ISO oracle.
- Compile presence, absence, self-containment, and repeated-inclusion probes
  with warnings as errors. Execute architecture-sensitive modifier cases
  natively on x86/x64/ARM64 and inspect ARM64/x86/x64 compile-time ABI facts.
- Perform a focused personal review of format parsing, exact `va_arg` types,
  destination pointers, sign/magnitude handling, checked arithmetic, scratch
  indices, output termination, stream lookahead, `va_list` lifetime, and error
  paths before formal verification.
- Run fresh builds, consumers, startup, full C89 and extension aggregates,
  traceability, TC-0016, WSP tool self-tests, evidence validators, import
  inspection, PowerShell parsing, `git diff --check`, and final-patch review.
- Retain the original failing result for every formal failure and link the
  defect, correction, and successful rerun. Do not replace a target result
  with cross-build or later-run evidence.

## Exact Implementation Order

1. Review and accept this proposed baseline; record any scope or estimate
   change before implementation starts.
2. Draft REQ-0029, REQ-0030, and the precisely allocated REQ-0039 record,
   including impact notes for REQ-0012, REQ-0019, REQ-0020, REQ-0027, and
   REQ-0028; complete the requirements review.
3. Draft and review TC-0029, TC-0030, and TC-0039 with explicit matrices,
   oracles, environments, pass criteria, and evidence metadata.
4. Freeze the formatter descriptor, output-sink policies, scanner input-source
   state machine, numeric representation/rounding method, exact varargs type
   table, and minimal C-locale wide ABI decision. Add an ADR if the durable
   choices exceed the requirement/platform records.
5. Add the declaration, C89-absence, typed modifier, numeric-vector, stream-
   remainder, and contract-partition tests plus runners and manifest entries so
   the new controlled cases initially expose the missing behavior.
6. Implement and verify the shared narrow numeric-text core first: lexical
   forms, classification, binary32/binary64 rounding, signed zero, range/error
   behavior, and `strtof`/`strtod`/`strtold` integration.
7. Replace the formatter directive/field/count infrastructure and implement
   integer/`%n` length modifiers using the reviewed type table; keep all four
   sink policies separate.
8. Implement floating output, including corrected common decimal behavior,
   `%a`/`%A`, `%F`, `L`, special values, rounding, width, precision, and
   length-only emission without precision-sized stack buffers.
9. Implement the applicable C-locale `l` output character conversions and
   verify their compatibility with the recorded future wide ABI.
10. Replace scanner line staging with the input-source state machine; implement
    matching/input-failure distinctions, width-limited lexical recognition,
    assignment counts, exact destination types, and first-unread preservation.
11. Integrate all integer, floating, character, scanset, and `%n` input
    conversions, then expose `vfscanf`, `vscanf`, and `vsscanf` and make the
    variadic functions thin wrappers.
12. Run TC-0029/TC-0030/TC-0039 and focused REQ-0012/REQ-0019/REQ-0020/
    REQ-0027/REQ-0028 regressions on x86 and x64; disposition every failure
    before expanding verification.
13. Perform the personal design/code/test review, including a cell-by-cell
    varargs audit and buffer/count analysis; record defects and correction
    effort before formal verification.
14. Run the full local C89/C99/compatibility, build, consumer, startup,
    source-quality, traceability, evidence, dependency, and x86 import gates;
    cross-compile/link ARM64 without calling it executed.
15. Run native ARM64 and changed-workflow CI, retain complete per-target
    evidence for the exact revision, and obtain independent pull-request review.
16. Update requirement implementation records, actual size/effort, checklist,
    evidence inventory, postmortem, and remaining-program estimates. Stop before
    commit, push, release, or publication unless separately authorized.

## Entry Evidence

At the inspected `HEAD`, the existing controlled C89 aggregate (TC-0001 through
TC-0015) and extension aggregate (TC-0019, TC-0021 through TC-0028, and the
eight Microsoft-compatibility cases) were refreshed and passed locally on x86
and x64 with TinyCC 0.9.28rc. These executions establish a current local entry
baseline only. They do not verify T2, replace native ARM64 execution, or satisfy
final retained-evidence metadata and review gates.

## Local Implementation Evidence

The uncommitted implementation was reviewed and exercised on 2026-08-09 with
the packaged TinyCC 0.9.28rc toolchains:

- TC-0029, TC-0030, and TC-0039 pass behavior and C89-isolation checks on x86
  and x64. ARM64 presence, C89-isolation, behavior-source compile, and link
  pass through the explicit `-CompileOnly` runner mode; execution is recorded
  as not run.
- Full TC-0001 through TC-0015 C89 aggregates pass on x86 and x64.
- The extension aggregate passes all twelve C99 cases, including TC-0019 and
  TC-0021 through TC-0030/TC-0039, and all eight compatibility cases on x86
  and x64. REQ-0019 and REQ-0020 therefore remain separate and green.
- Fresh Release DLL, static library, and startup-object builds pass for x86,
  x64, and ARM64. Static/DLL consumers execute on x86/x64 and compile/link for
  ARM64. x86 and x64 console/GUI startup execution passes. The x86 Windows
  2000 import allowlist passes with no unexpected import.
- TC-0016 reports zero violations across 141 C/header files; traceability
  reports 38 requirements, 38 specifications, and 38 implementations; all WSP
  common-tool self-tests pass; `git diff --check` reports no patch errors.

Generated local aggregate and build output remains ignored developer evidence.
It identifies a dirty version derived from baseline commit `c0e7ca6`, not an
immutable implementation revision, and therefore cannot substitute for the
required exact-revision native ARM64 CI and independent pull-request review.

## Time Log

| Date | Phase | Focused minutes | Excluded interruption | Note |
| --- | --- | ---: | --- | --- |

No focused minutes are inferred from chat, command wall time, or automated
execution time. The maintainer may add observed focused time.

## Defect Log

| ID | Type | Affected baseline | Observed behavior | Priority | Injected | Removed | Fix minutes | Owner and disposition / verification |
| --- | --- | --- | --- | --- | --- | --- | ---: | --- |
| T2-D001 | numeric | REQ-0012 shared formatter | Floating digits are truncated; signed zero, special values, large finite values, `%e`, and `%g` are not handled to the requested rules. | High | Existing implementation | Implementation | - | Removed by rounded decimal and exact hexadecimal formatting; TC-0012, TC-0029, and TC-0039 pass on x86/x64. |
| T2-D002 | resource/lifetime | REQ-0012, REQ-0019, REQ-0020 | A fixed 128-byte conversion buffer and unchecked width/precision/count arithmetic can overrun or overflow for caller-controlled fields. | High | Existing implementation | Implementation/review | - | Removed by checked sink counts and checked dynamic precision storage; 300-digit length/truncation and both bounded-contract regressions pass. |
| T2-D003 | interface/ABI | REQ-0012 shared scanner/formatter | Several signed, `ll`, and floating paths retrieve the wrong variable-argument or destination pointer type. | High | Existing implementation | Implementation/review | - | Removed using exact type helpers; full new modifier/radix and `%n` matrices pass x86/x64 and compile/link ARM64. |
| T2-D004 | algorithm | REQ-0012 shared scanner | Numeric width is applied after unbounded parsing; partial `ll` parsing ignores sign/base and malformed subjects are inconsistent. | High | Existing implementation | Implementation | - | Removed by width-aware lexical recognition; TC-0030 width and malformed-item partitions pass. |
| T2-D005 | resource/lifetime | REQ-0012 stream scanner | A 4096-byte line staging buffer reads ahead and discards unmatched input, breaking repeated scan state. | High | Existing design | Implementation | - | Removed by one-byte-lookahead input state; repeated scans and first-unread tests pass on x86/x64. |
| T2-D006 | interface/ABI | REQ-0019 and planned REQ-0030 | C99 bounded declarations can leak under `WCRT_C89`; the three `v*scanf` declarations/definitions are absent. | High | Existing header/baseline | Implementation | - | Removed by `WCRT_C89` guards, absence probes, declarations, and primary `v*scanf` implementations. |
| T2-D007 | test | TC-0012, TC-0019, TC-0020, TC-0027 | Passing tests do not cover the requested matrix or expose the shared defects above; `PRI*`/`SCN*` are inspected but not executed. | High | Existing test baseline | Test implementation/review | - | Removed for T2 by controlled cases, all-radix modifier matrices, macro execution, sink partitions, and aggregate integration. |
| T2-D008 | documentation | REQ-0012 implementation record | The record says the parsers cover all C89 rules although inspection shows material semantic and resource constraints beyond the documented 4096-byte limit. | Medium | Existing documentation | Requirements closeout | - | Removed by updating REQ-0012 to the implemented lookahead and growable numeric-item design. |
| T2-D009 | build/tooling | Project evidence baseline | Local aggregate results overwrite prior output and omit required release-evidence metadata. | Medium | Existing tooling | Open | - | WCRT maintainer; CI/evidence schema review and retained exact-revision results before closure. |
| T2-D010 | documentation | `docs/C99-REQUIREMENTS.md` | The REQ-0030 planning row cited the `vprintf` clause instead of all three `v*scanf` clauses. | Medium | Existing planning | Plan | - | Removed by correcting the allocation to 7.19.6.9, 7.19.6.11, and 7.19.6.14 after inspection of WG14/N1256. |
| T2-D011 | documentation | `docs/C99-1.0-WORK-PLAN.md` | The prior 639-1,101-hour total was 5-10 hours lower than the sum of its own tranche rows. | Medium | Existing planning | Plan | - | Removed by recomputing all rows; the corrected total is 699-1,206 focused hours. |
| T2-D012 | interface/ABI | Initial T2 formatter helpers | Passing TinyCC `va_list` by value to an argument helper advanced only a local copy, so repeated directives reused the first argument. | High | Implementation | Implementation test | - | Removed by passing the list address to exact-type helpers; initial TC-0029 code 1 became green and the full matrix passes x86/x64. |
| T2-D013 | test | Initial TC-0029 implementation | Two expected formatted return counts disagreed with their own expected strings. | Medium | Test implementation | Test review | - | Removed by recalculating lengths (`11` and `9`) and rerunning TC-0029 on x86/x64. |
| T2-D014 | algorithm | Initial T2 scanner | A zero result represented both a successful suppressed conversion and matching failure, allowing a failed suppressed directive to continue. | High | Implementation | Personal code review | - | Removed with explicit suppressed-success state and a negative `%*d` regression. |
| T2-D015 | numeric | Initial T2 formatter | Decimal helpers received a negative value after emitting its sign, causing invalid normalization; `%g` trimming could also remove integer zeroes. | High | Implementation | Personal code review | - | Removed by formatting the absolute representation and restricting trimming to the fractional suffix; negative, ties-to-even, and integer `%g` tests pass. |
| T2-D016 | buffer/count | Initial T2 formatter | Integer precision above the fixed ordinary-field buffer and very large hexadecimal precision arithmetic were not yet bounded. | High | Implementation | Personal buffer review | - | Removed with checked dynamic integer storage, overflow guards, and index arithmetic that stops shifting after 13 exact hex digits. |

No focused fix time is recorded because maintainer stopwatch data was not
provided; chat duration and automated command wall time are not substituted.

## Planning Checklist

- [x] Repository, process, prior tranche history, source, tests, and evidence
      were inspected before estimating.
- [x] Current x86/x64 entry regressions were refreshed without adding tests or
      changing production source.
- [x] Scope, exclusions, dependencies, risks, completion criteria, quality
      activities, estimate assumptions, and exact order are proposed here.
- [x] REQ-0029, REQ-0030, and allocated REQ-0039 requirements are drafted,
      indexed, and reviewed for implementation entry.
- [x] TC-0029, TC-0030, and TC-0039 specifications are drafted, indexed, and
      reviewed for implementation entry.
- [x] Durable numeric, scanner, varargs, and character-ABI design decisions are
      approved, with implementation deviations recorded above.
- [x] Implementation and personal design/code/test review are complete for the
      local working patch.
- [ ] x86/x64/ARM64 verification and retained CI evidence are complete.
- [x] Actual size, defect, quality, and postmortem comparison is complete;
      unavailable effort/schedule measures are recorded and no unsupported
      remaining-tranche re-estimate was made.

## Postmortem

**Completed:** 2026-08-09, using the recorded diff, test output, review
findings, empty time log, and defect log. This is a local postmortem, not a
claim that the tranche has met its supported-target completion gates.

### Planned versus actual

| Measure | Plan | Actual evidence | Assessment |
| --- | --- | --- | --- |
| Controlled scope | 3 requirement/test units | REQ-0029, REQ-0030, and T2 REQ-0039 allocation; matching records, runners, and manifest rows | Matched. |
| Artifacts | 30-42 | 28 source-controlled artifacts | 2 below the lower bound. Replacing existing cores and sharing one runner reduced file count. |
| Runtime/header change | 1,200-2,100 LOC | 1,778 changed lines across `src/` and `include/` | Within range. |
| Test/runner change | 1,100-1,900 LOC | 844 changed lines | 256 below the lower bound; focused tests and one shared runner were smaller than forecast. |
| Controlled documentation/integration | 700-1,100 lines | 739 changed lines | Within range; the controlled records and integration changes were modest. |
| Total change | 3,200-5,600 lines | 3,049 inserted, 312 removed, 3,361 changed lines across 28 artifacts | Within range. Runtime and total size estimates were sound; the test/runner allocation was high. |
| Phase effort | 130-225 focused hours, by phase | Unavailable: the time log has no focused-minute entries | No actual-versus-plan conclusion is valid. |
| Schedule | No completion date; capacity-based forecast only | Unavailable: no start/end dates or weekly focused capacity recorded | No schedule variance can be calculated. |

The size figures are a staged-diff snapshot at postmortem completion; generated
output and commit metadata are excluded. The artifact-count and test/runner
variance do not support changing a future tranche estimate by themselves,
particularly without actual effort or schedule data.

### Defects and quality activity

Eleven entry defects were recorded from the inspected baseline. Ten are
removed in this patch; T2-D009 remains open because local aggregate output is
overwritten and lacks immutable exact-revision CI evidence. Five additional
implementation/test defects (T2-D012 through T2-D016) were found and removed
before local verification. Four of those five were high priority. No defect is
recorded as escaping x86/x64 local verification. That is not evidence of zero
escapes on ARM64 or CI, because neither native ARM64 behavior nor independent
review has occurred.

The late-removed defects are concentrated in two recurring patterns:

- State and ABI handoff: copied `va_list` advancement and conflating suppressed
  conversion success with matching failure (T2-D012 and T2-D014), reinforcing
  the earlier destination-type findings in T2-D003.
- Numeric/boundary handling: signed decimal normalization, `%g` trimming,
  large precision allocation, and hexadecimal index limits (T2-D015 and
  T2-D016), continuing the fixed-buffer/count risks in T2-D001 and T2-D002.

Quality activity completed includes controlled requirements/tests and ADR
review; focused x86/x64 execution; ARM64 compile/link; full x86/x64 C89 and
extension/compatibility aggregates; Release builds and consumers on all three
targets; x86/x64 startup execution; the x86 Windows 2000 import gate; source
quality; traceability; PowerShell parsing; WSP tool tests; and `git diff
--check`. Retained native ARM64 behavior evidence, immutable per-target CI
evidence, and independent review remain incomplete and prevent target-complete
acceptance.

### Estimate disposition

The next-tranche estimate is unchanged. T2's runtime size fell within its
range, but the test/documentation delta was modestly below range and there is
no actual focused effort, duration, or comparable completed tranche data from
which to adjust hour or schedule forecasts responsibly. The next work log
should record observed phase minutes and calendar boundaries before a forecast
is recalibrated.

### Process improvements adopted

1. Before source changes, the controlled test review must explicitly verify
   expected character counts and exercise the full applicable typed conversion
   matrix, rather than relying on a small set of representative directives.
2. Parser and varargs personal review must include a written state walk for
   repeated argument consumption, suppression, matching failure, and input
   failure before the first aggregate run.
3. The maintainer must record phase start/stop minutes and preserve each
   target run with source revision, toolchain, command, timestamp, and artifact
   location as it occurs; missing data remains unavailable rather than inferred.

No completion date, actual focused effort, target-complete verification, or
release readiness is claimed. Publication remains out of scope.
