# C99 T2 Narrow Formatted I/O Work Log

**Status:** Proposed baseline; implementation not started

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

## Time Log

| Date | Phase | Focused minutes | Excluded interruption | Note |
| --- | --- | ---: | --- | --- |

No focused minutes are inferred from chat, command wall time, or automated
execution time. The maintainer may add observed focused time.

## Defect Log

| ID | Type | Affected baseline | Observed behavior | Priority | Injected | Removed | Fix minutes | Owner and disposition / verification |
| --- | --- | --- | --- | --- | --- | --- | ---: | --- |
| T2-D001 | numeric | REQ-0012 shared formatter | Floating digits are truncated; signed zero, special values, large finite values, `%e`, and `%g` are not handled to the requested rules. | High | Existing implementation | Open | - | WCRT maintainer; redesign in steps 6-8, verify TC-0029/TC-0039 and TC-0012 regression. |
| T2-D002 | resource/lifetime | REQ-0012, REQ-0019, REQ-0020 | A fixed 128-byte conversion buffer and unchecked width/precision/count arithmetic can overrun or overflow for caller-controlled fields. | High | Existing implementation | Open | - | WCRT maintainer; bounded representation and checked sink design, sentinel/length-only verification. |
| T2-D003 | interface/ABI | REQ-0012 shared scanner/formatter | Several signed, `ll`, and floating paths retrieve the wrong variable-argument or destination pointer type. | High | Existing implementation | Open | - | WCRT maintainer; exact type table and three-architecture audit/tests. |
| T2-D004 | algorithm | REQ-0012 shared scanner | Numeric width is applied after unbounded parsing; partial `ll` parsing ignores sign/base and malformed subjects are inconsistent. | High | Existing implementation | Open | - | WCRT maintainer; width-aware lexical state machine, TC-0030 boundary/malformed tests. |
| T2-D005 | resource/lifetime | REQ-0012 stream scanner | A 4096-byte line staging buffer reads ahead and discards unmatched input, breaking repeated scan state. | High | Existing design | Open | - | WCRT maintainer; input-source replacement and repeated-stream verification. |
| T2-D006 | interface/ABI | REQ-0019 and planned REQ-0030 | C99 bounded declarations can leak under `WCRT_C89`; the three `v*scanf` declarations/definitions are absent. | High | Existing header/baseline | Open | - | WCRT maintainer; edition absence probes and controlled interface tests. |
| T2-D007 | test | TC-0012, TC-0019, TC-0020, TC-0027 | Passing tests do not cover the requested matrix or expose the shared defects above; `PRI*`/`SCN*` are inspected but not executed. | High | Existing test baseline | Open | - | WCRT maintainer; reviewed controlled matrices and regressions in steps 3, 5, and 12. |
| T2-D008 | documentation | REQ-0012 implementation record | The record says the parsers cover all C89 rules although inspection shows material semantic and resource constraints beyond the documented 4096-byte limit. | Medium | Existing documentation | Open | - | WCRT maintainer; impact note and evidence-backed correction during requirements closeout. |
| T2-D009 | build/tooling | Project evidence baseline | Local aggregate results overwrite prior output and omit required release-evidence metadata. | Medium | Existing tooling | Open | - | WCRT maintainer; CI/evidence schema review and retained exact-revision results before closure. |
| T2-D010 | documentation | `docs/C99-REQUIREMENTS.md` | The REQ-0030 planning row cited the `vprintf` clause instead of all three `v*scanf` clauses. | Medium | Existing planning | Plan | - | Corrected to 7.19.6.9, 7.19.6.11, and 7.19.6.14 after inspection of WG14/N1256. Focused fix time was not recorded. |
| T2-D011 | documentation | `docs/C99-1.0-WORK-PLAN.md` | The prior 639-1,101-hour total was 5-10 hours lower than the sum of its own tranche rows. | Medium | Existing planning | Plan | - | Recomputed all rows after the T2 revision; the corrected total is 699-1,206 focused hours. Focused fix time was not recorded. |

No fix time is recorded because implementation has not started.

## Planning Checklist

- [x] Repository, process, prior tranche history, source, tests, and evidence
      were inspected before estimating.
- [x] Current x86/x64 entry regressions were refreshed without adding tests or
      changing production source.
- [x] Scope, exclusions, dependencies, risks, completion criteria, quality
      activities, estimate assumptions, and exact order are proposed here.
- [ ] REQ-0029, REQ-0030, and allocated REQ-0039 requirements are reviewed and
      accepted.
- [ ] TC-0029, TC-0030, and TC-0039 specifications are reviewed and accepted.
- [ ] Durable numeric, scanner, varargs, and character-ABI design decisions are
      approved.
- [ ] Implementation and personal review are complete.
- [ ] x86/x64/ARM64 verification and retained CI evidence are complete.
- [ ] Actual measures, postmortem, and remaining-tranche re-estimate are
      complete.

## Postmortem

Not started. No implementation, actual focused effort, completion schedule, or
T2 verification result is claimed by this proposed baseline.
