# WCRT 1.0.0-rc.1 Release Readiness Record

**Content type:** Project-owned release readiness record from pinned WSP
template `2198ccab08f969a789448767fe7017b774369adc`

**Project:** Waughtal C Run Time (WCRT)

**Release:** 1.0.0-rc.1 local unsigned candidate

**Source revision:** `aea690d2caf108054d1644eefb176efde779f524`

**Status:** Proposed and audited; release blocked

**Approval:** None

**Release-gate reconciliation:** 2026-08-20

## Release Scope

This candidate contains the C89 hosted-library baseline, C99 REQ-0019,
REQ-0021--REQ-0033, and REQ-0035--REQ-0042, the existing Microsoft
compatibility requirements, and optional console/GUI startup objects. The C99
profile contains 75 facility rows covering clauses 7.1--7.25 and all 24
standard headers. Decimal correct rounding through `DECIMAL_DIG` is omitted
recommended practice; Annex F, Annex G imaginary types, and the three
compiler-rejected standard pragmas are not represented as implemented Passes.

The intended platform matrix is Windows 2000 x86 for the legacy import tier
and Windows 10/11 x86, x64, and ARM64. Exact-revision native Debug evidence
exists for all three architectures in GitHub Actions run `32027269426`. Local
Release x86/x64 execution and ARM64 compile/link evidence use the exact pinned
TinyCC executables. The optimized tagged Release matrix has not run and remains
an R1 prepublication Unknown. This does not prevent committing a successor
candidate source revision, but it does prevent publication. Windows 2000
support is not claimed for x64 or ARM64.

REQ-0034 optional `wWinMain`, C11 and later library work, and the remaining
Microsoft CRT roadmap are excluded. No tag, private-key operation, signature,
release creation, artifact upload, or publication is part of this candidate.

## Gate Summary

| Gate | Evidence | Status | Exception or notes |
| --- | --- | --- | --- |
| Requirements baseline | `docs/REQUIREMENTS.md`; traceability result | Pass | 48/48/48 controlled records; baseline status is separate from target verification |
| C99 clause/facility profile | `docs/c99-conformance-profile.md`; TC-0041 | Pass | 25 clauses, 24 headers, 75 classified rows; compiler-blocked and optional rows remain explicit |
| Architecture and DFS review | `docs/security/design-for-security.md`; ADR-0001--0006 | Unknown | DFS, managed signing decision, and exact target evidence exist, but independent approval is absent |
| Build and package | verification record and candidate manifest | Fail | Build/package structure passes; WPM verification exits 1 because the package is unsigned |
| Verification and test report | `docs/evidence/c99-t6/local-verification.md`; tagged workflow | Unknown | Exact native Debug matrix passes; the required tagged optimized Release consumer/startup matrix has not run |
| Header/C89 isolation/ABI | TC-0040/TC-0042 and exact CI | Pass | All target header, constant-expression, layout, ABI, C89 isolation, and Debug native cases pass |
| Windows 2000 x86 imports | exact local Release import inspection | Pass | DLL `ddd4051a...`, machine `0x014C`, 27 allowlisted `kernel32.dll` imports, zero unexpected |
| Automatic built-in failure diagnostics | exact CI diagnostic JSON | Pass | TinyCC built-in crash trace identifies functions on all three targets |
| Independent GDB backtrace | exact CI diagnostic JSON | Unknown | GDB 17.1 Completed on x86/x64; ARM64 GDB 16.2 is explicitly `UnsupportedTarget`, not Pass |
| Defect and vulnerability review | T6 work log; DFS | Fail | Release-trust automation is prepared, but external identity/evidence, signed-byte scan design, and independent review remain open |
| Documentation | README, profile, implementation-defined/deviation, ARM64, release/signing process, DFS | Unknown | Signing controls are documented; final release notes/document and independent review are absent |
| Provenance and dependency record | `docs/evidence/c99-t6/dependency-provenance.json` | Pass | Exact archives/executables, sources, keys, action SHAs, runner images, jobs, and artifact IDs are retained |
| Checksums and exact identities | candidate manifest; local `SHA256SUMS` | Pass | Applies only to the exact unsigned local candidate; it is not a published release set |
| Windows signing and Defender scan | candidate manifest; ADR-0006; signing plan/workflow | Fail | The exact candidate DLLs are `NotSigned`. Managed signing automation is prepared but Azure provisioning and exercised evidence are Unknown; the unsigned-byte scan is informational and Defender design remains deferred |
| PDF metadata, digest, and provenance | None | Unknown | No project release-document PDF was produced or approved |
| PAdES signature, when selected | Release process | N/A | PAdES is not selected for this Markdown readiness record |
| Installation, rollback, and recovery | `docs/release-process.md` | Unknown | Procedure exists; clean-system install/remove/rollback was not authorized or executed |
| Support and communication | `docs/release-process.md`; DFS | Pass | GitHub issue/security-advisory intake and 1.x support boundary are documented; no response-time promise |
| Exact-revision CI | run `32027269426`; retained artifacts | Pass | Source quality, x86, x64, ARM64, and package jobs concluded success at the candidate source |
| Signing-control exact-revision CI | run `32337055333` at `3637a71b95c5b5cf9398b1813ba2577d8ea8d62e` | Pass | Workflow parsed; source quality, native x86/x64/ARM64 Debug matrices, and Debug package assembly passed. Tag-only Release/sign/package/publish jobs skipped, so Authenticode and WPM remain Unknown |

## Artifacts

| Artifact | Platform or purpose | Integrity value | Publication location |
| --- | --- | --- | --- |
| `wcrt-any-1.0.0-rc.1.23+aea690d2.zip` | Unsigned multi-architecture local candidate | SHA-256 `18be4aa5086d07ed9ff31e4f808b107d14c399408e394877c57e9f51f288e68b`; 418316 bytes | Local `output/candidate-packages-exact/`; not published |
| `SHA256SUMS` | Checksum for the exact local package | SHA-256 `29fcc6f24ce41bf7ea66f72162beb081c38951fa519e78f029efd789a11503ce`; 103 bytes | Local `output/release-candidate/1.0.0-rc.1-exact/`; not published |
| x86 Release `wcrt.dll` | Windows 2000 x86 / later x86 | SHA-256 `ddd4051a236e9c278ba1a3833e0f60d362dcf5218b79c0e45a85dac5e0d5791b`; 150528 bytes; `NotSigned` | Inside local package only |
| x64 Release `wcrt.dll` | Windows 10/11 x64 | SHA-256 `af0784a7570aa3c472ed796d2e28a6801a8c56e823f15250d41c357651c27511`; 170496 bytes; `NotSigned` | Inside local package only |
| ARM64 Release `wcrt.dll` | Windows 10/11 ARM64 | SHA-256 `a43dae26b787476429c3a7a1931003f14436a810816e89ccacb51b5195ffcb8b`; 196096 bytes; `NotSigned` | Inside local package only |

The machine-readable candidate manifest identifies every static library,
startup object, and ARM64 private ABI companion. Package metadata reports
source version `1.0.0-rc.1`, package version
`1.0.0-rc.1.23+aea690d2`, and short source revision `aea690d2`; this record
binds that short value to the full revision above.

This readiness reconciliation is post-candidate evidence. The package contains
the README from source `aea690d2`; it does not contain the later reconciled
record text. Incorporating the reconciled documentation into distributed bytes
requires a new source revision, package identity, checksum, and affected
verification. The Documentation gate therefore remains Unknown.

The exact CI Debug package
`wcrt-debug-any-0.0.0-dev.23+aea690d2.zip` is retained verification evidence,
not a candidate Release artifact. Its SHA-256 is
`a94c6bda2289623cf03c798b41a21bf15789e802a2cdbfd2400574d3d01438f5`
and its length is 1555555 bytes.

## Open Items and Accepted Risk

No release-blocking item is accepted for publication.

| Item | Impact | Owner | Approval | Completion or review condition |
| --- | --- | --- | --- | --- |
| Tagged optimized Release matrix | Optimized Release consumers/startup and exact release artifact behavior are not proved on all supported targets | Maintainer | None | Successful tag-triggered x86, x64, and ARM64 Release jobs at the committed candidate baseline, including native consumers/startup, x86 imports, and retained outputs/hashes; package and publication must remain downstream |
| Authenticode publisher identity | Distributed PE publisher and integrity cannot yet be verified | Maintainer | Logical publisher `Jordan Waughtal`, Azure Artifact Signing Public Trust, and Microsoft RFC 3161 service approved by ADR-0006 | Approve/configure `release` environment tag/reviewer/bypass protections; complete Azure identity validation/account/profile, profile-scoped RBAC, GitHub OIDC federation and environment secrets; freeze the exact returned subject; then obtain passing signature/timestamp evidence for every final DLL |
| Defender release gate design | An ad hoc unsigned scan cannot prove final release bytes | Maintainer | None | Design and review a repeatable release/GitHub Actions scan after signing; retain exact hashes, timestamps, engine/intelligence versions, exit status, and detections |
| WPM package signature | `wpm verify` rejects the candidate | Maintainer | Existing WPM key pair and `release_keys/wpm-release.public` selected; exercised approval none | After environment protection is approved, migrate `WPM_RELEASE_PRIVATE_KEY` from repository to `release` environment scope; sign after PE signing, trust the public key, and obtain exit 0 on the unchanged package |
| Installation and rollback | Install scripts, registry state, removal, and recovery are unexercised | Maintainer | None | Clean supported-system install/static+DLL consumer/remove/rollback log for the exact signed package on each applicable architecture |
| Release document/PDF | Selected WSP documentation gate has no final release document or provenance | Maintainer | None | Select the release-document format; generate and validate metadata, manifest, and digest; apply PAdES only if selected |
| Post-candidate documentation reconciliation | The exact package predates the final evidence wording | Maintainer | None | Freeze reconciled documentation in a new candidate source and rebuild/reverify all changed package bytes |
| Independent project review | Author evidence has not received independent PR review | Reviewer | None | Challenge all Pass cells and disposition every finding at the immutable candidate baseline |
| Stale dependency helper inventory | Two unused bootstrap scripts mention absent miniz/libsodium directories | Maintainer | None | Remove or replace the helpers before relying on them; current workflow provenance remains authoritative |

## Approval Decision

- **Decision:** Reject
- **Approver:** WCRT maintainer readiness audit; independent project approval
  absent
- **Date:** 2026-08-17
- **Rationale:** Exact-revision native Debug evidence passes on x86, x64, and
  ARM64, but the tagged optimized Release matrix has not run. The package and
  DLLs are unsigned, WPM verification fails, final-signed-byte Defender
  evidence and its repeatable design do not exist, install/rollback is
  Unknown, release documentation is incomplete, and independent review is
  absent. The source may be committed in this state; publication may not occur.
- **Support or communication actions:** Do not tag, publish, create a release,
  upload, or announce availability. The Authenticode design is approved, but an
  actual signing operation is blocked until the controlled Azure identity is
  provisioned. Retain this candidate for corrective verification only.

## Minimum Corrective Commands or Evidence

The final candidate source may be committed before Release evidence exists.
The minimum Release evidence is the successful GitHub tagged workflow, not a
maintainer-local ARM64 substitution. After every other prepublication blocker
is closed and separate tag/publication authority is given, the workflow order
is:

```text
build (complete Debug matrix)
  -> release (native optimized x86, x64, and ARM64 consumers/startup; x86 imports)
  -> sign (all DLLs; exact subject, SHA-256, RFC 3161 timestamp, trust, signed x86 imports)
  -> package (signed WPM assembly and verification)
  -> publish (GitHub Release creation and upload)
```

Retain the tagged run URL, source SHA, runner images, dependency records,
per-target Release artifacts and hashes, startup JSON, unsigned/signed x86
import JSON, all Authenticode/SignTool certificate and timestamp JSON, unchanged
packaged-DLL comparisons, signed package identity, and `wpm verify` output and
exit code. Because the current tag workflow publishes automatically after those
dependencies succeed, do not push the tag merely to collect evidence; tagging
requires the final authorization to publish.

An independent reviewer must inspect the candidate source, this readiness
record, run `32027269426`, and the exact local candidate manifest. Every
exception or correction must identify whether it changes candidate bytes; any
source or build-input change creates a new baseline and requires affected
verification to be rerun.

ADR-0006 approves the service and logical identity. The remaining external
configuration is the Azure-validated certificate profile and exact subject.
The GitHub API currently reports no protection rules for `release`; first
approve and configure its tag, reviewer, self-review, and admin-bypass policy.
Then set each value only in that environment:

```powershell
gh secret set AZURE_CLIENT_ID --env release
gh secret set AZURE_TENANT_ID --env release
gh secret set AZURE_SUBSCRIPTION_ID --env release
gh secret set ARTIFACT_SIGNING_ENDPOINT --env release
gh secret set ARTIFACT_SIGNING_ACCOUNT_NAME --env release
gh secret set ARTIFACT_SIGNING_CERTIFICATE_PROFILE --env release
gh secret set AUTHENTICODE_EXPECTED_SUBJECT --env release
gh secret set WPM_RELEASE_PRIVATE_KEY --env release
gh secret list --env release
```

`WPM_RELEASE_PRIVATE_KEY` already exists at repository scope and remains
WPM-only. Re-enter it at `release` environment scope, verify that use, then
delete the repository-scoped copy with
`gh secret delete WPM_RELEASE_PRIVATE_KEY`. The tag workflow performs the
equivalent controlled order:

```powershell
./tools/test-authenticode-signatures.ps1 -Path <three-signed-dlls> -ExpectedSubject <exact-validated-subject> -SourceRevision <full-sha>
./tools/build-wpm-package.ps1 -Version 1.0.0-rc.1 -Configuration Release -SigningKey <protected-temporary-key-path>
wpm trust add release_keys/wpm-release.public
wpm verify <exact-signed-package.zip>
./tools/test-wpm-package-authenticode.ps1 -Package <exact-signed-package.zip> -ExpectedSubject <exact-validated-subject> -SourceRevision <full-sha>
```

Retain full SignTool output, signer subject/thumbprint, timestamp and trust
chain, before/after SHA-256 values, protected-key audit reference, and WPM
verification output. Then use the separately reviewed Defender release design
against the exact signed DLLs and final signed package. Finally, perform the
documented clean-system install, consumer, remove, and rollback cases.

No signing command or private-key operation was executed as part of this exact
candidate. Repository-side automation and fail-closed verification are
preparation evidence only.

## Baseline Record

- **Release tag:** None; no tag was created.
- **Full source revision:**
  `aea690d2caf108054d1644eefb176efde779f524`.
- **Exact CI:** run `32027269426`, conclusion success.
- **WSP revision:** `2198ccab08f969a789448767fe7017b774369adc`.
- **Dependency baseline:**
  `docs/evidence/c99-t6/dependency-provenance.json`.
- **Build/test record:** `docs/evidence/c99-t6/local-verification.md`.
- **Artifact record:**
  `docs/evidence/c99-t6/release-candidate-manifest.json`.
- **Published artifact set:** None.
- **Release-document PDF:** None.
