# WCRT Design for Security

**Content type:** Project Design for Security (DFS)

**Project:** Waughtal C Run Time (WCRT)

**Version:** 1.0 candidate baseline

**Status:** Implemented baseline; independent approval pending

**Approval:** Required by the WCRT 1.0.0 release-readiness decision

## Scope and Environment

This DFS covers WCRT public headers, runtime libraries, optional startup
objects, build and test automation, the WPM package, dependency acquisition,
and release evidence. WCRT executes inside a consuming Windows process and has
the privileges of that process. It is not a sandbox, access-control boundary,
cryptographic library, or defense against a malicious caller with process
memory access.

The supported release matrix is Windows 2000 x86 for the legacy import tier
and supported Windows 10/11 x86, x64, and ARM64 systems. Windows 2000 support
does not extend to x64 or ARM64. The C99 profile and target evidence boundaries
are controlled by `docs/c99-conformance-profile.md` and
`docs/arm64-support.md`.

## Security Goals and Non-Goals

| ID | Goal or non-goal | Rationale | Related requirement |
| --- | --- | --- | --- |
| SG-001 | Preserve memory and control-flow integrity for inputs within documented C contracts | Runtime defects execute in the consumer's trust boundary | REQ-0001--REQ-0042 |
| SG-002 | Build and distribute artifacts from identified source and dependencies | Substitution or drift invalidates review and verification | REQ-0042 |
| SG-003 | Authenticate release artifacts and retain exact identities | Users and responders need to distinguish official bytes | WSP-SIGN-0001--0014 |
| SG-004 | Preserve the Windows 2000 x86 import boundary | Accidental new imports break supported systems | REQ-0042 |
| NG-001 | WCRT does not validate a caller's C preconditions | The C library cannot make arbitrary invalid pointers or races safe | N/A |
| NG-002 | WCRT does not provide process isolation or a cryptographic trust service | These belong to the host and release infrastructure | N/A |

## Assets and Consequences

| Asset | Required property | Consequence of compromise |
| --- | --- | --- |
| Source and requirements baseline | Integrity and traceability | Reviewed behavior no longer identifies shipped code |
| Compiler, WPM, cv2pdb, WSP, and action inputs | Integrity and provenance | Build or evidence can be substituted |
| DLL, static library, startup objects, headers, and package | Integrity and authenticity | Consumer code can execute unintended behavior |
| Signing identities | Confidentiality and controlled use | Malicious artifacts can appear project-authenticated |
| Test and scan evidence | Integrity and exact-artifact binding | A release can be approved on unrelated bytes |
| Vulnerability reports | Availability and appropriate confidentiality | Fixes can be delayed or reporters exposed |

## Trust Model and Boundaries

- Source, controlled requirements, reviewed build scripts, protected release
  environments, and approved signing services are trusted only at recorded
  revisions and configurations.
- Caller pointers, lengths, format strings, file paths, stream contents,
  environment variables, locale names, and process state are untrusted inputs.
- Windows APIs, runner images, TinyCC, cv2pdb, WPM, WSP, and GitHub actions are
  external dependencies. Their exact versions or revisions must be recorded.
- GitHub downloads and release assets cross a network trust boundary. A URL or
  TLS connection alone is not provenance; pinned revisions, signed WPM
  metadata, trust keys, and SHA-256 values provide the selected controls.
- The WPM Ed25519 signature, Authenticode signature, SHA-256 checksum,
  Defender result, and SmartScreen reputation are distinct trust layers.

## Threats and Abuse Cases

| ID | Threat or abuse case | Affected asset | Control | Residual risk |
| --- | --- | --- | --- | --- |
| THR-001 | Malformed sizes, numeric text, or formats trigger overflow or out-of-bounds access | Consumer process | Boundary, range, parser, formatter, and regression tests | C remains memory-unsafe; incomplete input partitions can contain defects |
| THR-002 | ABI mismatch corrupts calls or saved state | Consumer process | Per-target type assertions, native ABI tests, consumers, setjmp/fenv tests | Native target evidence is mandatory; cross-linking is insufficient |
| THR-003 | Dependency or action drifts after review | Build integrity | Exact versions/revisions, release-key hashes, action SHAs, executable hashes | Runner image contents remain externally maintained |
| THR-004 | New Windows import breaks the legacy target | Availability | Repository-owned PE import parser and allowlist | Runtime-only OS differences still require supported-system tests |
| THR-005 | Artifact is replaced, modified after scan, or bound to stale evidence | Release integrity | Signing order, SHA-256 manifest, immutable artifact names, evidence digest checks | External publication controls must preserve reviewed bytes |
| THR-006 | Signing credential is copied or misused | Release identity | Managed or hardware-backed key, least privilege, audit trail, no repository key | Signing service and identity are not yet selected for 1.0.0 |
| THR-007 | A Defender result is assumed from no reported failure | Users | Exact final-byte scan record with engine/intelligence versions | New detections can occur after release |
| THR-008 | Required target result is inferred from another target | Conformance and availability | Per-target Pass/Fail/Unknown profile and retained native output | Native ARM64 Debug passes; tagged ARM64 Release remains an R1 Unknown |

## Security Controls

- The build uses pinned WSP, GitHub action commits, WPM 1.0.16, TinyCC
  `0.9.28-rc.1442+2474e1c2`, and cv2pdb 0.54.0. Release records retain
  resolved executable and artifact digests.
- Public APIs are isolated by C edition. Header self-containment, constant
  expression types, data models, ABI layouts, and C89 exclusions are tested.
- WCRT builds avoid a host CRT dependency and inspect x86 PE imports against
  the Windows 2000 allowlist.
- Required tests fail closed. Compiler-blocked, optional, deviating, and
  Unknown states are separate from Pass.
- Release order is: freeze source and dependencies; build and verify PEs;
  Authenticode-sign and timestamp final PEs; verify signatures; scan those
  exact PEs; package without modifying them; sign and verify the WPM envelope;
  calculate checksums; approve exact identities; publish only with separate
  authorization.
- Private keys must not enter source control, packages, command output, or
  retained logs. No private-key operation is authorized by candidate
  preparation.

## Security Verification

| Requirement or threat | Verification | Evidence | Status |
| --- | --- | --- | --- |
| SG-001 / THR-001 | C89, C99, compatibility, negative, boundary, and source-quality tests | Controlled TC records and `docs/evidence/c99-t6/local-verification.md` | Exact-revision native Debug matrix Pass; tagged optimized Release matrix is an R1 Unknown |
| SG-002 / THR-003 | Traceability, dependency pins, hashes, WSP tests, clean revision | Workflow, `docs/evidence/c99-t6/dependency-provenance.json`, local verification | Pass: exact archives/executables, sources, actions, runner images, jobs, and artifact IDs retained |
| SG-004 / THR-004 | Parse x86 DLL imports and bind output to DLL SHA-256 | Local verification and `docs/evidence/c99-t6/release-candidate-manifest.json` | Exact x86 Release candidate Pass |
| THR-002 / THR-008 | Native x86/x64/ARM64 tests, consumers, and startup checks | Per-target retained results | Debug Pass all targets; tagged optimized Release matrix is an R1 Unknown |
| SG-003 / THR-005--007 | Signature verification, Defender scan, package verification, checksums | Candidate manifest and release-readiness record | Fail/Unknown; unsigned package and DLLs, no final signed-byte evidence |

## Vulnerability and Incident Response

GitHub issues are the public defect and vulnerability intake. Reports that
could expose users before a fix should use the repository's private security
advisory channel when available. The maintainer owns triage, affected-version
analysis, corrective tests, release or rollback disposition, coordinated
disclosure, and evidence retention. No response-time promise is made by this
personal project.

The current 1.x release line is supported until an end-of-support notice is
published. A compromised dependency or signing identity blocks publication,
requires exact affected-artifact analysis, and may require removing package
repository references and marking the release withdrawn.

## Residual Risk and Approval

| Risk | Rationale | Owner | Approval | Review condition |
| --- | --- | --- | --- | --- |
| Memory-unsafe implementation language | Required by project scope; testing reduces but cannot eliminate defects | Maintainer | Release decision | Every security defect or major parser/allocator change |
| External compiler, runner, OS, and package tools | WCRT cannot reproduce their full supply chains | Maintainer | Release decision | Dependency or runner baseline change |
| Signing identity and service not selected | Candidate preparation may remain unsigned, but release may not proceed | Maintainer | None | Before 1.0.0 approval |
| Tagged optimized Release evidence unavailable | Debug behavior and cross-build cannot prove optimized Release behavior on every target | Maintainer | None | Successful tag-triggered native x86/x64/ARM64 Release matrix before publication |

## Change History

| Date | Version | Change | Approval |
| --- | --- | --- | --- |
| 2026-08-17 | 1.0 candidate baseline | Initial project-owned DFS from pinned WSP template | Pending release review |
