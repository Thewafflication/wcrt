# WCRT 1.1.0 Compatibility Roadmap

**Status:** Proposed release scope

**Theme:** VC6-era Microsoft C compatibility and a practical POSIX source
compatibility profile for Windows

## Objective

WCRT 1.1.0 is a compatibility-focused release built on the 1.0.0 C99
baseline. It shall make a useful, explicitly selected subset of Windows C
programs written for the Visual C++ 6 era or for common POSIX interfaces build
and run with WCRT without weakening the ISO C89/C99 contracts.

The release defines two related but separate profiles:

- **VC6 profile:** source and documented C ABI compatibility with the public
  C runtime surface selected from Visual C++ 6.0 with its final service pack.
  The profile is an x86 consumer target. It includes compilation of controlled
  C consumers with the genuine VC6 C compiler and linking them to a WCRT DLL
  through a Microsoft-format import library.
- **POSIX profile:** source and behavioral compatibility with a deliberately
  bounded subset of POSIX.1-2017 file, descriptor, directory, string, time, and
  process-identification interfaces. It is a WCRT portability layer, not a
  claim that Windows or WCRT is a conforming POSIX environment.

The VC6 profile describes both runtime compatibility and compiler-consumer
compatibility. Building WCRT itself with VC6 is not required for 1.1.0; the
supported TinyCC baseline remains the WCRT build toolchain.

## Release principles

- Additive 1.1 interfaces shall preserve the public C89/C99 API and ABI shipped
  in 1.0.0. A correction to a previously shipped compatibility symbol requires
  an impact analysis, an explicit selector or versioned alternative, and a
  migration note.
- Microsoft, POSIX, and ISO contracts shall be tested independently. An alias
  may share an internal implementation only when its types, error behavior,
  path rules, text/binary behavior, and edge cases actually agree.
- New POSIX names shall be behind an explicit WCRT compatibility selection.
  WCRT shall not infer or advertise a `_POSIX_VERSION` value until every
  obligation associated with that value is satisfied.
- VC6-era behavior shall be based on retained documentation, header and
  library inventories, and reference tests from a licensed VC6 SP environment;
  modern UCRT behavior is not an acceptable substitute when the two differ.
- The VC6 profile is x86-only. Shared Microsoft and POSIX implementations
  shall also pass on x64 and ARM64 unless a profile entry explicitly documents
  why it is x86-only.
- Every x86 path shall retain the Windows 2000 import allowlist. A newer API may
  be used only through runtime discovery with a tested legacy fallback.

## Scope boundary

### Required for 1.1.0

The exact declarations and behaviors will be frozen by the profile inventories
in tranche R0. The release floor includes these families:

- VC6 header and ABI fundamentals: x86 calling conventions and decoration,
  VC6-parseable integer and time types, structure packing, `errno` translation,
  file-descriptor representation, and a VC6-compatible DLL import library.
- Microsoft low-level I/O and files: the core `_open`, `_sopen`, `_close`,
  `_read`, `_write`, `_lseek`, `_tell`, `_commit`, `_dup`, `_dup2`, `_pipe`,
  `_isatty`, `_setmode`, `_access`, `_chmod`, `_unlink`, `_fstat`, and `_stat`
  families, their required flags and structures, and handle conversion where
  the VC6 inventory requires it.
- Microsoft directories and paths: the core `_chdir`, `_getcwd`, `_mkdir`,
  `_rmdir`, drive selection, `_fullpath`, `_makepath`, and `_splitpath`
  families selected by the VC6 inventory.
- VC6-era strings, conversions, and formatting: the common case-insensitive
  aliases, duplication/case/reversal helpers, numeric conversion helpers, and
  the `%I64` formatting contract. Existing 1.0 Microsoft
  extensions remain regression obligations even when they postdate VC6.
- Microsoft 32-bit time/file metadata forms needed by VC6 programs, while
  retaining the explicit-width 1.0 interfaces.
- POSIX types and headers needed by the selected surface: `<unistd.h>`,
  `<fcntl.h>`, `<dirent.h>`, `<strings.h>`, `<sys/types.h>`, and compatible
  additions to `<sys/stat.h>` and `<utime.h>`.
- POSIX descriptor and file operations: `open`, `close`, `read`, `write`,
  `lseek`, `dup`, `dup2`, `pipe`, `isatty`, `fsync`, `fileno`, `fdopen`,
  `access`, `stat`, `fstat`, `chmod`, `unlink`, and the required types and
  constants.
- POSIX paths, directories, strings, time, and process identity: `chdir`,
  `getcwd`, `mkdir`, `rmdir`, `opendir`, `readdir`, `rewinddir`, `closedir`,
  `strcasecmp`, `strncasecmp`, `strdup`, `utime`, and `getpid`.

An R0 inventory may remove a listed spelling only when it proves that the
spelling is not part of the selected VC6 or POSIX baseline. Adding another
family to the 1.1.0 floor requires a roadmap impact note and new estimates.

### Deferred beyond 1.1.0

- C++ ABI, the Visual C++ standard library, exceptions, RTTI, MFC, ATL, and
  undocumented or private Microsoft runtime exports.
- A claim that WCRT can replace every version of `msvcrt.dll`, or bug-for-bug
  compatibility outside a controlled requirement.
- Building the WCRT implementation or a static WCRT archive with VC6. The
  1.1.0 VC6 deliverable is a tested C consumer path through the WCRT DLL.
- Full `_exec*`/`_spawn*` coverage, multithreaded CRT state, wide Microsoft
  extension families, locale-object APIs, and the remaining MS3--MS6 backlog
  in the project roadmap.
- `fork`, POSIX signals beyond the ISO C surface, pthreads, `mmap`, sockets,
  `select`/`poll`, `termios`, dynamic loading, user/group databases, symlinks,
  and Unix device or permission emulation.
- C11 or later standard-library milestones. The 1.1.0 release shall not expose
  newer ISO facilities merely to implement a compatibility extension.

## Tranche order

```text
R0 contract and inventory
  -> R1 headers, ABI, and packaging
    -> R2 shared descriptor and path core
      -> R3 Microsoft VC6 surface ----\
       -> R4 POSIX surface ------------+-> R5 consumer and release closure
```

R3 and R4 may proceed in parallel only after R2 freezes the shared descriptor,
path, error, and stream-coherence rules.

## Required test additions

The following are release work, not optional follow-up. R0 shall assign formal
`TC-NNNN` identifiers after it freezes the requirement families. The headings
below are planning groups and do not replace controlled test specifications.

### Test control and inventory

- [ ] Create separate controlled manifests and aggregate runners for the VC6
      and POSIX profiles. Each runner shall reject missing, duplicate, skipped,
      or unmanifested requirements and test implementations.
- [ ] Map every required profile row to a requirement, test specification,
      presence or absence source, behavioral source, runner, and evidence
      field. Matrix classifications and test manifests shall be checked for
      drift in CI.
- [ ] Update `docs/TEST-STRATEGY.md`, `docs/REQUIREMENTS.md`, and the test
      report inputs for the new profiles before an implementation checkbox can
      be marked complete.

### Header, selector, and ABI tests

- [ ] Compile every new or changed header alone, twice, and in every relevant
      include order under TinyCC C89/C99 and genuine VC6 C mode. Treat a host
      header fallback as a test failure.
- [ ] Add strict-ISO absence tests plus Microsoft-only, POSIX-only, and combined
      profile presence tests. Verify conflicting spellings, types, constants,
      and macros do not change meaning with include order.
- [ ] Generate x86 ABI reference probes for structure size, alignment, member
      offsets, packing, calling convention, decorated symbol, integer width,
      `time_t`, `off_t`, and file-size variants.
- [ ] Add an explicit `_stat` migration test that preserves the 1.0 ABI while
      proving the separately selected VC6 layout and decorated link symbol.
- [ ] Inspect the DLL export table, `.def`, and Microsoft-format import library;
      link a positive VC6 consumer and reject missing, wrongly decorated, or
      wrong-architecture symbols.

### Descriptor, stream, path, and error tests

- [ ] Cover descriptor allocation and reuse, duplicate-handle lifetime,
      inheritance, append atomicity, seek boundaries, EOF, commit, terminal
      detection, text/binary translation, and native-handle ownership.
- [ ] Mix `FILE *` and descriptor operations on the same file and verify shared
      offsets, buffered data, `fileno`/`fdopen`, close ordering, double-close
      rejection, and deterministic errors.
- [ ] Exercise short reads and writes, broken pipes, sharing violations,
      access denial, invalid descriptors, large offsets, overflow, disk-full
      behavior where safely injectable, and partial-operation return values.
- [ ] Cover drive-relative, rooted, UNC, slash/backslash, dot-segment, reserved,
      trailing-separator, maximum-length, invalid-encoding, and wildcard paths
      without depending on the runner's current directory.
- [ ] Add concurrency stress for descriptor reuse, duplicate operations,
      simultaneous stream/descriptor access, and process teardown. Retain the
      seed and operation log for a failure.

### VC6 compatibility tests

- [ ] Compile and run the same selected behavior cases against the retained VC6
      reference CRT and WCRT. Record and approve every intentional difference;
      a modern UCRT result shall not replace the VC6 result.
- [ ] Add boundary and negative tests for every selected `<io.h>`, `<fcntl.h>`,
      `<share.h>`, `<direct.h>`, metadata, time, path, string, conversion, and
      `%I64` interface.
- [ ] Verify 32-bit time and file-size limits, structure initialization,
      wildcard enumeration, `errno`/OS-error mapping, legacy aliases, and
      text/binary defaults in the genuine VC6 environment.
- [ ] Build a no-default-Microsoft-CRT `/TC` DLL consumer and inventory any C
      compiler helper symbols. Also test an ordinary source-porting consumer
      so compiler compatibility and runtime replacement remain distinct.

### POSIX compatibility tests

- [ ] Compile one portable behavioral source set against WCRT on Windows and a
      retained POSIX.1-2017 reference environment. Keep shared assertions for
      promised behavior and explicit assertions for documented Windows
      deviations.
- [ ] Test `mode_t`, `off_t`, `ssize_t`, `pid_t`, and `struct stat` widths and
      overflow; file type and permission mappings; timestamps; directories;
      pipes; and descriptor return-value and `errno` contracts.
- [ ] Test deletion and rename of open files, directory iteration and rewind,
      root and drive behavior, slash handling, executable-status mapping, and
      cases that Windows must reject rather than emulate inaccurately.
- [ ] Prove that no `_POSIX_VERSION` or unsupported function is advertised and
      that the published profile matrix exactly matches the declarations and
      exports present in the package.

### Consumer, architecture, and WSP evidence tests

- [ ] Run TinyCC static and DLL consumers for shared features on x86, x64, and
      native ARM64; run the genuine VC6 DLL/import-library consumers on x86;
      and run at least one packaged consumer for each compatibility profile.
- [ ] Repeat symbol, PE architecture, dependency, and Windows 2000 x86 import
      inspection on Debug, tagged Release, and packaged DLL artifacts.
- [ ] Satisfy `WSP-TEST-0016` by testing the successful and deliberately
      failing Debug-artifact paths. Retain machine-readable results, readable
      diagnostics, exact tested binaries, symbols, source/toolchain identity,
      and applicable Debug package inputs for every architecture.
- [ ] Satisfy `WSP-TEST-0017` by executing the complete applicable 1.1.0
      behavior and consumer inventory on `windows-11-arm`, recording the
      runner architecture, and rejecting cross-build-only evidence as a native
      Pass.
- [ ] Apply the tailored `WSP-TEST-0018` diagnostic self-test to each Debug
      architecture and both new aggregate runners. Require the TinyCC native
      frames, preserve the original failure result and executable, and record
      the independently classified GDB attempt.
- [ ] Verify fresh install, packaged-header compilation, static/DLL selection,
      removal, and rollback for each architecture without using build-tree
      headers or libraries accidentally.

### Minimum execution matrix

| Test family | TinyCC x86 | TinyCC x64 | TinyCC ARM64 | Genuine VC6 x86 | POSIX reference |
| --- | --- | --- | --- | --- | --- |
| Header self-containment and selectors | Required | Required | Required | Required for VC6 profile | Compile comparison |
| ABI, symbols, and package inspection | Required | Required | Required | Required | Not applicable |
| Shared descriptor/path behavior | Required | Required | Required native | Required for selected VC6 calls | Required for common POSIX assertions |
| Microsoft differential behavior | Required | Required for shared calls | Required for shared calls | Required reference and WCRT consumer | Not applicable |
| POSIX differential behavior | Required | Required | Required native | Not applicable | Required reference |
| Debug artifact and failure diagnostics | Required | Required | Required native | Consumer diagnostics only | Not applicable |

An `Unknown`, missing artifact, skipped required row, unclassified differential,
or cross-build substituted for native execution is not Pass.

### R0 — Contract, inventory, and migration decisions

- [ ] Record the exact VC6 compiler, service-pack, linker, librarian, public
      headers, import libraries, documentation, and Windows reference
      environments used as evidence. Do not redistribute proprietary inputs.
- [ ] Produce a VC6 matrix of public C headers, names, macros, types, layouts,
      decorated x86 symbols, return values, `errno`/OS-error behavior, and
      known version-specific semantics. Classify every entry as 1.0-existing,
      1.1-required, deferred, unsupported, compiler-provided, or OS-provided.
- [ ] Produce the equivalent POSIX.1-2017 subset matrix, including each
      deliberate Windows deviation and the observable behavior promised by
      WCRT rather than merely listing familiar function names.
- [ ] Adopt an ADR for compatibility selection, header exposure, shared
      internals, x86 decoration, structure packing, descriptor ownership,
      `FILE`/descriptor coherence, path conversion, and error translation.
- [ ] Resolve the existing x86 `_stat`/`_stat64` alias and layout risk without
      silently changing a 1.0 consumer ABI. Specify the VC6 32-bit `time_t`,
      file-size, and packing model separately from explicit 64-bit forms.
- [ ] Allocate requirement and test-case records for every selected family and
      capture estimates, risks, review points, and supported-target evidence in
      a 1.1.0 work log. Cover the WSP 1.1.0 milestone plan, design, review, and
      closeout records, using separate files or clearly identified sections as
      appropriate to tranche risk.

**Exit condition:** both matrices and the ADR are approved; no selected name
has an unknown owner, ABI, semantic contract, or test destination.

### R1 — VC6-readable headers, ABI probes, and package format

- [ ] Make the selected public headers parse as C under the genuine VC6
      compiler without leaking C99-only syntax. Centralize compiler spelling
      differences such as 64-bit integer types and declaration attributes.
- [ ] Add compile-time and generated binary probes for sizes, alignments,
      offsets, packing, calling conventions, decorated names, and constants.
      Compare VC6 reference objects with the selected WCRT profile.
- [ ] Define Microsoft and POSIX feature selection without breaking the 1.0
      visibility of already shipped Microsoft extensions. Test strict ISO,
      VC6, POSIX, and combined selection in every include order.
- [ ] Generate a versioned x86 Microsoft-format import library for the WCRT
      DLL and include it in the package with the existing `.def` and TinyCC
      archives.
- [ ] Add one minimal VC6 `/TC` DLL consumer that links without an accidental
      dependency on a Microsoft C runtime. Inventory any compiler-emitted C
      helper dependencies; do not broaden this into C++ runtime support.

**Exit condition:** the controlled VC6 headers, ABI probes, package inspection,
and minimal consumer pass in the retained VC6 environment.

### R2 — Shared Windows descriptor, path, and error substrate

- [ ] Specify descriptor allocation, validation, duplication, inheritance,
      append, seek, EOF, commit, terminal detection, and native-handle
      ownership. Ensure `FILE *` and descriptor operations observe one
      coherent offset and close state.
- [ ] Implement one internal operation layer for files, pipes, directories,
      metadata, and timestamps. Keep Microsoft and POSIX adapters separate
      where their contracts differ.
- [ ] Specify UTF-16 boundary conversion for narrow paths, accepted separators,
      drive-relative paths, UNC paths, reserved names, trailing separators,
      maximum lengths, and invalid encodings.
- [ ] Define deterministic mapping from Windows errors to WCRT `errno`, and
      test invalid handles, access failures, sharing violations, broken pipes,
      short I/O, disk-full behavior, and interrupted/partial operations.
- [ ] Preserve binary byte semantics for the POSIX profile and explicitly test
      Microsoft text/binary translation and `_fmode` interactions.
- [ ] Add concurrency and lifetime tests for descriptor reuse, duplicated
      handles, simultaneous stream/descriptor access, and teardown.

**Exit condition:** focused internal tests pass on x86, x64, and ARM64, and the
x86 import audit remains within the Windows 2000 allowlist.

### R3 — VC6-era Microsoft surface

- [ ] Implement the R0-selected `<io.h>`, `<fcntl.h>`, `<share.h>`,
      `<direct.h>`, `<sys/types.h>`, `<sys/stat.h>`, and time declarations and
      functions, including constants and legacy aliases.
- [ ] Complete the selected path construction/decomposition, integer
      conversion, string helper, and `%I64` formatting behaviors.
- [ ] Verify VC6-specific 32-bit time and file-size overflow behavior,
      large-file alternatives, structure initialization, wildcard enumeration
      when selected, and precise `errno` results.
- [ ] Run presence, negative, behavioral, ABI, symbol, and import tests against
      both static and DLL WCRT builds; run the controlled consumer through the
      VC6 DLL import-library path.
- [ ] Publish a VC6 compatibility profile showing Pass, intentional deviation,
      and unsupported entries. Do not describe modern secure-CRT interfaces as
      VC6 facilities merely because WCRT also provides them.

**Exit condition:** every required VC6 matrix row has a requirement, passing
test evidence, and documented compatibility classification.

### R4 — Selected POSIX surface

- [ ] Implement the selected POSIX headers, types, constants, descriptor/file
      operations, directory iteration, string helpers, timestamps, and process
      identification without exposing them in strict ISO mode.
- [ ] Document and test Windows mappings for mode bits, executable status,
      directory links, file identity, timestamps, deletion of open files,
      rename behavior, pipes, roots, drive letters, and path separators.
- [ ] Use width-checked conversions for `off_t`, `ssize_t`, `mode_t`, `pid_t`,
      and `struct stat`; report overflow rather than truncate silently.
- [ ] Run the same portable behavioral sources against WCRT on Windows and a
      retained POSIX reference environment. Classify intentional platform
      differences instead of weakening assertions until they pass.
- [ ] Build at least one representative portable C consumer using only the
      published POSIX profile and record any source changes or exclusions.

**Exit condition:** every required POSIX matrix row is implemented and tested,
or is removed from the claimed subset with an approved scope correction.

### R5 — Compatibility and release closure

- [ ] Run all 1.0 C89, C99, Microsoft-extension, startup, consumer,
      source-quality, traceability, dependency, packaging, and release gates
      on the exact 1.1.0 candidate.
- [ ] Run strict-ISO absence tests proving that POSIX and newly selected
      Microsoft names do not leak into an unselected header surface.
- [ ] Run VC6 x86 and TinyCC x86/x64/ARM64 static/DLL consumer matrices, ABI
      and export inventories, Windows 2000 import inspection, and package
      installation/removal tests.
- [ ] Publish the VC6 and POSIX profile matrices, compatibility-selection
      guide, porting examples, implementation-defined behavior, deviations,
      and unsupported list.
- [ ] Complete independent review, a 1.1.0 release-readiness record, and a
      postmortem. A missing genuine-VC6 result is Unknown and blocks the VC6
      claim; it cannot be replaced by a modern MSVC result.

**Exit condition:** all required profile rows and release gates are Pass, all
deviations are approved and visible, and representative VC6 and POSIX consumers
build and run from the packaged artifacts.

## AI-assisted work estimate

These are rough-order-of-magnitude planning estimates, not delivery limits or
billing forecasts. They include repository context, tool results, reasoning,
generated changes, review, and expected repair iterations. They exclude human
review time, CI queue time, acquisition or licensing of VC6, and unattended
test execution time.

For this estimate:

- one **prompt** is one outcome-oriented Codex turn that inspects, changes,
  verifies, and reports one bounded work slice; tool calls within that turn do
  not count as additional prompts;
- **gross model tokens** means the combined model-visible input/context, tool
  output, reasoning, and response tokens reported by the execution environment
  where available; it is a project planning unit rather than an API invoice;
- repair after a failed test, an independent review, and closeout are separate
  prompts; and
- the estimates include all test work in the Required test additions section.

### Tranche estimate

| Tranche | Expected prompts | Gross model tokens | Recommended model routing | Dominant prompt work |
| --- | ---: | ---: | --- | --- |
| R0 — Contract and inventory | 12–18 | 300k–500k | GPT-5.6 Sol, high; xhigh for `_stat` migration decision | Reference inventory, compatibility matrices, ADR, requirement/test allocation |
| R1 — Headers, ABI, packaging | 18–28 | 500k–850k | Sol high for ABI; Terra medium/high for probe and packaging implementation | VC6 parsing, selectors, layouts, symbols, import library, minimal consumer |
| R2 — Descriptor/path substrate | 35–55 | 1.00M–1.70M | Sol high/xhigh for ownership and failure design; Terra high for implementation | Descriptor core, stream coherence, paths, error mapping, concurrency and failure tests |
| R3 — Microsoft VC6 surface | 45–70 | 1.40M–2.20M | Terra high for bounded API slices; Sol high for ABI and tranche review | Microsoft headers/functions, VC6 differential tests, time/file limits, consumers |
| R4 — POSIX surface | 40–65 | 1.20M–2.00M | Terra high for bounded API slices; Sol high for semantic/deviation review | POSIX headers/functions, reference differential tests, Windows mappings, consumers |
| R5 — Release closure | 20–35 | 600k–1.10M | Sol high; xhigh for unresolved release blockers; Terra medium for mechanical evidence reconciliation | Full matrices, package tests, retained evidence, independent review, readiness and closeout |
| **Planned total** | **170–271** | **5.00M–8.35M** | Tiered routing | Complete 1.1.0 release floor |
| **Management reserve** | **34–55** | **1.25M–2.10M** | Sol for triage, then route by work type | Approximately 20% prompt and 25% token contingency |
| **Planning envelope** | **204–326** | **6.25M–10.45M** | Tiered routing | Planned work plus contingency |

The range assumes the genuine VC6 and POSIX reference environments are already
available, R0 does not expand the release floor, and the R2 design does not
require replacement of the complete 1.0 stream implementation. Adding a new
API family, provisioning a legacy toolchain, or redesigning `FILE` invalidates
the affected tranche estimate and requires replanning.

### Recommended AI algorithm

Use a **requirement-first, test-driven agent loop with differential oracles**.
This is preferable to generating a large compatibility layer in one pass
because VC6, Microsoft, POSIX, and ISO interfaces often share names while
differing in ABI or observable behavior.

For each bounded requirement family:

1. **Retrieve and inspect:** read the current requirement, implementation,
   headers, tests, exports, platform layer, and retained reference evidence.
2. **Freeze the oracle:** specify types, ABI, success behavior, failures,
   deviations, target matrix, and reference-comparison rules before coding.
3. **Write the controlled tests:** add presence/absence, behavioral, boundary,
   ABI, differential, import, and negative tests needed for that slice.
4. **Implement the smallest vertical slice:** change the shared operation and
   only the selected Microsoft or POSIX adapters; avoid unrelated cleanup.
5. **Verify progressively:** run focused tests, the affected aggregate, strict
   ISO isolation, shared x86/x64/ARM64 tests, and then the full regression set.
6. **Review independently:** use a fresh GPT-5.6 Sol high-reasoning prompt to
   inspect the diff, contracts, failure paths, ABI, tests, and evidence. Use
   xhigh only for unresolved ABI, ownership, concurrency, or release decisions.
7. **Close and record:** resolve findings, update matrices and evidence, record
   actual prompts/tokens and defects, and complete the WSP tranche closeout.

Model roles follow the current
[official OpenAI model guidance](https://developers.openai.com/api/docs/models):

- **GPT-5.6 Sol:** default for architecture, ABI, requirement interpretation,
  difficult debugging, security/reliability analysis, independent review, and
  release readiness.
- **GPT-5.6 Terra:** default for bounded implementation and test-writing after
  the contract is frozen; escalate to Sol when semantics or architecture are
  uncertain.
- **GPT-5.6 Luna:** use only for high-volume mechanical inventory extraction,
  manifest normalization, and documentation formatting with deterministic
  checks. Luna output shall not approve an ABI, deviation, or release gate and
  shall receive Terra or Sol review before acceptance.

Reconfirm model availability and official guidance at the start of each
tranche because model catalogs change. Model choice is an execution aid, not
release evidence and not a substitute for the controlled compiler, runtime,
and differential tests.

### Prompt budget and replanning rules

- Target one public API family or one cross-cutting invariant per implementation
  prompt. Do not combine unrelated Microsoft and POSIX families merely to use
  remaining context.
- Budget an ordinary family at 4–7 prompts and 100k–220k gross tokens. Budget
  an ABI-, stream-, concurrency-, or packaging-sensitive family at 8–14 prompts
  and 250k–500k gross tokens.
- Start a distinct review prompt after each requirement-sized implementation
  slice and a distinct closeout prompt after its full regression evidence is
  available.
- Record actual prompts and gross tokens by WSP phase in the tranche work log.
  Do not reconstruct unavailable token data after the fact; mark it Unknown.
- Re-estimate when a tranche consumes 60% of its upper token estimate before
  half its required exit criteria pass, when three consecutive repair prompts
  fail for the same cause, or when a compatibility decision changes a public
  ABI, supported target, test oracle, or release artifact.
- Consuming the estimate or reserve never waives an exit criterion. Stop scope
  growth, preserve the evidence, and replan the remaining work.

## Release acceptance summary

WCRT 1.1.0 is ready only when all of the following are true:

- The VC6 and POSIX matrices contain no unclassified or merely planned entry
  within the release floor.
- A genuine VC6 C compiler builds and links the controlled x86 DLL consumers
  using the packaged Microsoft-format import library.
- The selected portable consumers pass on WCRT, and differential tests against
  the POSIX reference have no unexplained difference.
- Every new interface has a requirement, test specification, implementation,
  traceability entry, supported-target result, and user documentation.
- The VC6 and POSIX aggregate runners reject manifest drift, and every selected
  profile row maps to retained presence/absence, behavior, ABI, or documented
  analysis evidence as applicable.
- Strict C89 and C99 modes remain compatible with 1.0.0; all 1.0 regression
  suites pass without source changes.
- The x86 DLL and consumers retain the Windows 2000 import contract; x64 and
  ARM64 results are Pass or explicitly outside a VC6-only matrix cell.
- Package contents, version metadata, installation, rollback, signing, hashes,
  and release approval satisfy the project release process.
- Debug artifacts, native ARM64 execution, and tailored failure diagnostics
  satisfy `WSP-TEST-0016` through `WSP-TEST-0018` for the new aggregates.

## Principal risks

| Risk | Required control |
| --- | --- |
| VC6 headers use types or layouts that differ from the current compatibility surface | Freeze reference ABI probes in R0/R1; selector-gate incompatible layouts and publish migration guidance |
| VC6 tooling is proprietary and unsuitable for redistribution on hosted CI | Use a licensed retained VM or self-hosted runner; record tool hashes and keep an absence of evidence release-blocking |
| Microsoft and POSIX spellings look alike but have different semantics | Share only the internal Windows operation; retain separate contract adapters and differential tests |
| `FILE *`, WCRT descriptors, and native handles drift out of sync | Centralize ownership/state in R2 and stress duplicate, close, seek, buffering, and reuse behavior |
| Windows cannot reproduce Unix permissions, unlink, identity, or path behavior exactly | Bound the profile, document mappings, and return explicit errors for unsupported cases |
| Legacy compatibility introduces newer Windows imports | Enforce the existing x86 import allowlist for every tranche and test runtime fallbacks |
| The release expands into all of MSVCRT or all of POSIX | Enforce the release-floor matrices and require an impact note before adding a new family |
