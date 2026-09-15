# Stream Allocation Thread-Safety Work Record

**Content type:** Consolidated milestone plan, design, work log, review, and
closeout record

**Work package:** REQ-0074 thread-safe stream allocation

**Period:** 2026-09-14

**Starting baseline:** WCRT `a2ea86b` on `master`

**Owner:** WCRT maintainer

**Status:** Implementation and native Debug CI verified; see the dated release
integration and publication evidence below. Historical pending cells are retained.

**Approval:** Review and merge of this work package

## Milestone Plan

### Objective and Scope

Make WCRT's dynamic stream table safe for concurrent callers, so that two
threads calling `fopen` at the same time receive two distinct streams. The
defect is corrective rather than new functionality: the table existed, was
reachable from the selected pthread surface added by REQ-0071 and the worker
pool added by REQ-0073, and had no serialization. The work package specifies
the guarantee, implements it, and verifies it.

Included work is a controlled requirement and test specification, an internal
serialization primitive usable below `<pthread.h>`, claim and release
discipline at every site that takes or returns a table entry, a concurrent
behavior test, and traceability.

Excluded work is C11 per-stream locking, `flockfile` and `funlockfile`, any
change to the representation of `FILE`, buffered-I/O reentrancy, and thread
safety of `errno`-adjacent state outside the stream table. The exclusion is
recorded in REQ-0074 itself rather than left implicit.

This package relates to the R2 stream-coherence scope of the 1.1.0 roadmap. It
does not close R2 and is not represented as an R2 exit.

### Baseline and Assumptions

- REQ-0012 remains the controlled owner of stream behavior; REQ-0074 adds a
  concurrency guarantee and changes no published stream contract.
- REQ-0071 is the controlled owner of the selected pthread surface. The stream
  layer shall not depend on it, because `src/platform/windows/pthread.c`
  already includes `<stdio.h>`.
- `FOPEN_MAX` is 32 and the standard streams live outside the dynamic table.
- Windows 2000 x86 remains the minimum target, so no import outside the
  existing allowlist may be introduced.
- The published stream ABI is frozen by ADR-0003 and checked by REQ-0042.
- Test runners glob `src/**/*.c`, so a new platform source needs no build
  change.

### Deliverables and Allocation

| Deliverable | Allocation | Completion evidence |
| --- | --- | --- |
| Concurrency guarantee and exclusion clause | REQ-0074 | Requirement review |
| Internal serialization primitive | `src/internal/lock.h`, `src/platform/windows/lock.c` | Source review |
| Claim and release discipline | `src/stdio.c` | Source review, TC-0074 |
| Concurrent behavior test | `tests/posix/stdio_threads.c`, `run-tc-0074.ps1` | TC-0074 |
| Traceability | `docs/REQUIREMENTS.md`, `tests/wcrt/manifest.md` | Traceability runner |
| Aggregate registration | `tools/run-extension-tests.ps1` | Aggregate inventory check |
| Library build allocation | `tools/build-wcrt.ps1` | Successful DLL and static-library link |

The slice affected twelve artifacts with 539 added and 14 deleted lines,
including this record. No size forecast was made before implementation, so no
forecast comparison is claimed. Focused effort was not instrumented and shall
not be inferred from chat or command wall time.

### Risks and Controls

| Risk | Impact | Control |
| --- | --- | --- |
| Stream layer depends on the POSIX surface | Layering inverts; strict profiles lose stdio | Primitive compiled unconditionally, no `WCRT_POSIX` guard |
| Occupancy state enlarges `FILE` | Published stream ABI breaks | Occupancy held in a separate private array; REQ-0042 checks size |
| Lock held across a file operation | A slow close stalls every open process-wide | Lock scope limited to the table scan; no Windows file call under it |
| New synchronization imports | Windows 2000 x86 target regresses | Reuse the imports `pthread.c` already declares |
| Initialization races on first use | Table corrupted before any lock exists | Named-mutex double-checked initialization, as used by `pthread.c` |
| Test passes without detecting aliasing | Defect ships behind a green result | Mutation testing of the test itself, recorded below |
| New test not registered in the aggregate | Aggregate inventory check fails the whole suite | Manifest, runner, and aggregate list updated together |

Rollback consists of reverting `src/stdio.c`, deleting the two new source
files and the two new controlled documents, and removing the traceability
and aggregate rows. No other component depends on the primitive.

## Design Record

### Components and Interfaces

| Component | Responsibility | Dependency | Allocation |
| --- | --- | --- | --- |
| `struct wcrt_lock` | Lazily initialized mutual exclusion | Windows 2000 synchronization imports | REQ-0074 |
| `lock.c` | Critical-section backing and one-time initialization | `CreateMutexA`, `InitializeCriticalSection` | REQ-0074 |
| `wcrt_stream_used[]` | Table occupancy, private to `stdio.c` | `wcrt_stream_lock` | REQ-0074 |
| `wcrt_allocate_stream`, `wcrt_claim_stream`, `wcrt_release_stream` | Claim and release discipline | Occupancy array | REQ-0074 |
| `stdio_threads.c`, `run-tc-0074.ps1` | Concurrent behavior verification | Selected pthread surface | TC-0074 |

The original allocator returned the first entry whose `handle` was null and
left the entry unclaimed until `__wcrt_file_open` wrote a handle. Two threads
could therefore observe the same entry as free. The correction separates
occupancy from stream contents: an entry is claimed in a private array before
the stream is opened and released only after it is closed, so no caller can be
given an entry another thread is preparing or tearing down. Because occupancy
no longer lives in `FILE`, the structure is unchanged and the published ABI is
unaffected.

Separating the two states also keeps the lock off the slow path. The lock
covers the table scan and the occupancy write only; `CreateFileA`,
`CloseHandle`, and `DeleteFileA` all run outside it, so a close on a slow
volume does not block unrelated opens.

The primitive composes its process-scoped initialization mutex name without
formatted output, because the stream layer that the lock protects is the layer
formatted output is built on. It declares no import that
`src/platform/windows/pthread.c` does not already declare, so the Windows 2000
x86 allowlist is unchanged.

### Verification Allocation

| Requirement or risk | Method | Evidence |
| --- | --- | --- |
| Distinct streams under concurrency | Barrier-synchronized behavior test | TC-0074 |
| Per-stream data integrity | Per-worker pattern write and read back | TC-0074 |
| Entry reuse after closure | 256 opens through a 32-entry table | TC-0074 |
| Test actually detects aliasing | Mutation of the test's own checks | Work log below |
| Unchanged stream representation | Existing compatibility ABI checks | REQ-0042 |
| Legacy Windows imports | PE import analysis | `test-windows-2000-imports.ps1` |
| Source quality and traceability | Repository-owned automated checks | TC-0016 and traceability runner |

## Work and Defect Log

| Order | Phase | Activity | Output |
| --- | --- | --- | --- |
| 1 | Inspect | Traced the stream table, every allocation site, and the release path | Defect D001 |
| 2 | Specify | Froze the guarantee, the exclusion, and the layering constraint | REQ-0074, TC-0074 |
| 3 | Design | Chose private occupancy over a sentinel in `FILE` to protect the ABI | Design record above |
| 4 | Implement | Added the primitive and claim and release discipline at seven call sites | Source change |
| 5 | Verify | Static checks, reference-libc execution, mutation of the test | Evidence below |
| 6 | Review | Inspected diff, layering, lock scope, and release pairing | Review record below |

| ID | Type | Injected | Removed | Disposition |
| --- | --- | --- | --- | --- |
| STR-D001 | Concurrency | Original stream table implementation | This work package | Unsynchronized allocator could return one entry to two threads, aliasing `FILE` objects and descriptors and leaking a Windows handle |
| STR-D002 | Resource | Original stream table implementation | This work package | `_open`, `_pipe`, and `freopen` had no release path on failed open, and `_dup2` never claimed its named target entry; both were latent because occupancy was inferred from `handle` |
| STR-D003 | Concurrency | Implementation | Implementation self-review | First `_dup`/`_dup2` draft released the target entry on both paths, so a double release could free an entry another thread had just claimed; release ownership moved to `_dup2` alone |
| STR-D004 | Documentation tooling | Implementation | Personal review | A text edit normalized `docs/REQUIREMENTS.md` from mixed to uniform CRLF terminators, producing a whole-file diff; corrected by a byte-level insert that preserved all 46 original CRLF lines |
| STR-D005 | Test integration | Implementation | Post-review inspection | TC-0074 was added to `tests/wcrt/manifest.md` but not to the `$tests` inventory in `tools/run-extension-tests.ps1`, so the aggregate would have thrown on the manifest-to-runner inventory check; the entry was added |
| STR-D006 | Build integration | Implementation | First local DLL build | `tools/build-wcrt.ps1` carries an explicit `$sources` list rather than globbing, so `lock.c` was absent from the DLL and static library and the link failed on `__wcrt_lock_acquire` and `__wcrt_lock_release`. The personal review had recorded build integration as passing on the untested assumption that every consumer globs |
| STR-D007 | Source | Implementation | First local DLL build | `lock.c` used `NULL` without including a header that defines it, which `pthread.c` masks by including `<stdlib.h>`; `<stddef.h>` was added. The file had not been compiled by any toolchain before the first build attempt |

No fix effort was recorded.

## Personal Review Record

**Reviewed baseline:** Current working diff against `a2ea86b`

**Reviewer:** Implementer personal review; independent pull-request review is
still required

| Area | Criterion | Result | Observation |
| --- | --- | --- | --- |
| Requirements | The guarantee and its boundary are both stated | Pass | REQ-0074 names every claim site and excludes per-stream locking explicitly |
| Layering | Stream code does not depend on the POSIX surface | Pass | Primitive compiles unconditionally; no `WCRT_POSIX` guard, no `<pthread.h>` include |
| ABI | `FILE` representation is unchanged | Pass | Occupancy is a separate private array; no member added or reordered |
| Lock scope | No Windows file operation runs under the table lock | Pass | Inspected all four acquisition sites |
| Pairing | Every claim has exactly one release | Pass | Verified per site after correcting STR-D003 |
| Stale entries | A claimed, unopened entry cannot be resolved by descriptor | Pass | `handle` is null on every such entry, which `wcrt_descriptor_stream` rejects |
| Imports | No new Windows dependency | Pass | All eight imports already appear in `windows-2000-x86-imports.json` |
| Build integration | New source reaches every build and runner | Pass after correction | The test runners glob `src/**/*.c`, but `build-wcrt.ps1` does not; see STR-D006 |
| Test integration | Manifest and aggregate inventory agree | Pass after correction | STR-D005 found and fixed after the first review pass |
| Quality | Line length, tabs, and trailing whitespace | Pass | 80-column and whitespace scans clean on all four C artifacts |
| Test adequacy | The test fails when the defect is present | Pass | Three mutations detected; see evidence below |

No unresolved personal-review finding remains. TinyCC execution, native
multi-architecture behavior, independent review, CI execution, and release
approval are outside this local review's evidence boundary.

## Local Verification Evidence

- The 80-column, tab, and trailing-whitespace scans pass on `src/stdio.c`,
  `src/internal/lock.h`, `src/platform/windows/lock.c`, and
  `tests/posix/stdio_threads.c`.
- `tests/posix/stdio_threads.c` compiles unmodified under
  `-std=gnu89 -Wall -Werror` against a reference POSIX libc and passes. This
  demonstrates the test is well formed and deadlock-free; it is not evidence
  about WCRT.
- Three mutations of the test were rejected with the expected exit codes:
  a duplicated descriptor (25), an aliased stream pointer (24), and a
  corrupted payload (21). The test detects what TC-0074 claims it detects.
- `docs/REQUIREMENTS.md` was confirmed byte-for-byte unchanged apart from the
  single inserted row, with its original mixed line endings preserved.
- `lock.c` compiles clean under `gcc -std=c89 -Wall -Wextra -Werror -pedantic`
  with Windows-only decoration stripped, confirming no further undeclared
  identifiers.
- The x64 Release DLL and static library link successfully with TinyCC
  0.9.28rc after STR-D006 and STR-D007 were corrected. x86, ARM64, and Debug
  configurations have not been built.
- TC-0074 has not been executed against WCRT with TinyCC on Windows. No pass
  is claimed for the requirement's own verification.

## Closeout and Handoff

| Exit criterion | Evidence | Gate | Status |
| --- | --- | --- | --- |
| Requirement and test specification controlled | REQ-0074, TC-0074 | Required | Pass |
| Implementation matches the specified discipline | Source review above | Required | Pass |
| Traceability rows agree with runners | `REQUIREMENTS.md`, `tests/wcrt/manifest.md` | Required | Pending runner execution |
| Library and static archive link on supported targets | `build-wcrt.ps1` output | Required | Partial: x64 Release links; x86, ARM64, and Debug pending |
| TC-0074 passes with TinyCC on x86 and x64 | `run-tc-0074.ps1` output | Required | Pending |
| C89, C99, Microsoft, and POSIX aggregates unregressed | Aggregate runner output | Required | Pending |
| Windows 2000 x86 import allowlist | `test-windows-2000-imports.ps1` | Required | Pending |
| Source quality and traceability gates | TC-0016 and traceability runner | Required | Pending |
| Native ARM64 behavior | `windows-11-arm` result | Release gate | Pending |
| Independent review and CI | Pull request and workflow evidence | Release gate | Pending |

The specification, design, and implementation slice is complete. The work
package shall not be closed until TC-0074 and the existing aggregate, import,
and quality gates pass on the supported targets. No pending gate is
represented as passing.

The working tree also carries an unrelated change to `tools/wcrt-version.ps1`
and to the `$Version` default in `tools/build-wcrt.ps1`, correcting release-tag
resolution and dirty-tree traceability under WSP-WINRES-0004. Only the
`lock.c` source-list line in `build-wcrt.ps1` belongs to this work package;
the version change is not claimed as an output of this slice and needs its own
impact note.

Next action for the maintainer is to run `tests/posix/run-tc-0074.ps1`,
followed by the aggregate, import, and traceability runners, and to record
their results in the verification evidence above.

## References

- [REQ-0074](../req-0074-thread-safe-streams.md)
- [TC-0074](../tc-0074-thread-safe-streams.tex)
- [REQ-0012](../req-0012-stdio.md)
- [REQ-0071](../req-0071-posix-threads.md)
- [REQ-0073](../req-0073-thread-pool.md)
- [ADR-0003](../adr-0003-c99-wide-character-and-stream-abi.md)
- [1.1.0 roadmap](../WCRT-1.1.0-ROADMAP.md)
- [Project process](../PROJECT-PROCESS.md)

## 2026-09-15 Release Integration Correction

CI run `34931422597` exposed STR-D008: the explicit source inventories in
seven focused test scripts omitted `src/platform/windows/lock.c`. The C89
stdio test and nine extension tests failed to link the two internal lock
functions. The earlier statement that every test runner globs was incorrect.
All explicit stdio inventories, including the stdin-only and ARM64 cross
builds, now include the lock source. No public ABI or runtime behavior changes.

Local TinyCC `0.9.28-rc.1446+07318c25` x64 verification passes all 15 C89
tests and all 55 extension tests, including TC-0074. CI must still verify
x86 and native ARM64 and the remaining release gates before tagging 1.2.4.
The existing tests reproduce the omitted dependency and verify its repair.

Release scope also includes the prior version-resource correction: implicit
build versions resolve the nearest release tag, while dirty builds retain
source identity. Explicit release versions remain authoritative. This affects
build metadata and package identity, with no public C interface change.

## 2026-09-15 Candidate Verification and Follow-up

Candidate `8e35b16` passes [CI run 35009561630](https://github.com/Thewafflication/wcrt/actions/runs/35009561630):
source quality, requirements traceability, x86/x64/native ARM64 Debug builds,
C89 and extension aggregates, native diagnostics, static/DLL consumers,
startup checks, the x86 legacy-import audit, and Debug package assembly.
This supersedes the historical execution-pending entries above. No separate
independent reviewer result is claimed by this update.

The maintainer requested release and continued work on 2026-09-15. Tag 1.2.4
identifies that exact candidate; its optimized builds and publication are
tracked by [run 35010196148](https://github.com/Thewafflication/wcrt/actions/runs/35010196148).

Post-release documentation work exposes REQ-0074's sharing boundary in the
README. Concurrent allocation does not serialize access to a shared `FILE`.
Future behavior work should extend TC-0074 beyond concurrent `fopen`/`fclose`
to failed-open reclamation, table exhaustion, and descriptor-adapter claim
paths. Those scenarios remain follow-up coverage, not evidence supplied by
the current four-worker test. Any such change should first state its expected
error, cleanup, and reuse behavior in the controlled test specification.

Process lesson: whenever an internal dependency is introduced, search both
recursive source collectors and explicit source arrays, including reduced
stdin-only and cross-build inventories. A successful library link alone does
not establish that focused tests still link.

### Publication Result

[WCRT 1.2.4](https://github.com/Thewafflication/wcrt/releases/tag/1.2.4)
is published. Tagged run `35010196148` completed successfully, including all
three optimized Release targets, WPM signature and packaged-DLL verification,
the four PDFs and offline documentation archive, Pages deployment, and GitHub
publication. The release contains all nine expected assets: the WPM package,
four PDFs, documentation ZIP, index, checksum list, and public verification key.
Authenticode and Defender remain deferred under the existing release policy.
The roadmap status note now reflects the published version.

## 2026-09-15 Stream Capacity and Failure-Path Follow-up

### Scope, Design, and Impact

The continued stream work extends TC-0074 with a separate deterministic
`stdio_slots.c` executable before the existing concurrent worker test. It
covers failed `fopen`, `_open`, and `freopen` reclamation; full-table errors;
pipe partial-reservation rollback; and `_dup`/`_dup2` target ownership. The
runner compiles each executable immediately before running it, retaining a
separate binary and a stage-specific failure message. The requirement,
controlled test specification, and WCRT manifest describe both sources.

The oracle uses WCRT's FOPEN_MAX dynamic slots, verifies live descriptor
uniqueness and contents, then proves complete capacity recovery. Missing-file
`fopen` checks require failure and cleanup without imposing a new errno
contract. `_open` checks retain its ENOENT contract. Error assertions execute
before worker creation because errno remains process-global. No per-stream
concurrency, public ABI, import, or supported-platform change is introduced.

### Defect and Review Record

STR-D009: the first exhaustion run failed with code 12. `fopen` returned NULL
with stale errno because the shared allocator did not report EMFILE, contrary
to REQ-0074. The allocator now sets EMFILE when its locked scan finds no free
entry. Existing descriptor adapters already report EMFILE. The same test
passes after the three-line correction, providing a failing-before and
passing-after regression case.

Personal review checked reservation/release accounting, readable descriptors
after exhaustion, duplicate lifetime after source close, unchanged pipe output
on failure, failed-reopen cleanup, and recovery of all slots. The test is
specific to the documented WCRT table, not a portable POSIX capacity claim.
Failure exits retain the numeric stage and may leave test files for diagnosis;
success closes every descriptor and removes the test file.

No advance effort estimate or token measurement was recorded. This is one
bounded requirement/test slice. Native ARM64 execution and independent review
remain outstanding; local cross-compilation is not runtime evidence.

### Verification Result

TinyCC `0.9.28-rc.1446+07318c25`: all 15 C89 and 55 extension cases pass
locally on both x86 and x64, including both TC-0074 executables. The two
TC-0074 sources also compile and link for ARM64 with the packaged cross
compiler. TC-0016 source quality passes with zero violations, requirements
traceability passes, and `git diff --check` is clean. Native ARM64 execution
and CI remain pending for this uncommitted follow-up. Evidence is retained in
`output/test-results/{x86,x64}` and the TC-0074 build directory.
