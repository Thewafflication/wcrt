# WCRT 1.0.0-rc.1 Release Readiness Record

**Content type:** Project-owned release readiness record from pinned WSP
template `2198ccab08f969a789448767fe7017b774369adc`

**Project:** Waughtal C Run Time (WCRT)

**Release:** 1.0.0-rc.1 local unsigned candidate

**Source revision:** `3fa0b1a3a22bbd93ff9185dbb24689d83e057e56`

**Status:** Proposed and audited locally; release blocked

**Approval:** None

## Release Scope

This candidate contains the C89 hosted-library baseline, C99 REQ-0019,
REQ-0021--REQ-0033, and REQ-0035--REQ-0042, the existing Microsoft
compatibility requirements, and optional console/GUI startup objects. The C99
profile contains 75 facility rows covering clauses 7.1--7.25 and all 24
standard headers. Decimal correct rounding through `DECIMAL_DIG` is omitted
recommended practice; Annex F, Annex G imaginary types, and the three
compiler-rejected standard pragmas are not represented as implemented Passes.

The intended platform matrix is Windows 2000 x86 for the legacy import tier
and Windows 10/11 x86, x64, and ARM64. Local native x86/x64 evidence and ARM64
compile/link evidence exist at the source revision above. Native ARM64
candidate behavior does not exist and remains Unknown. Windows 2000 support is
not claimed for x64 or ARM64.

REQ-0034 optional `wWinMain`, C11 and later library work, and the remaining
Microsoft CRT roadmap are excluded. No tag, push, signature, publication,
release creation, or upload is part of this candidate.

## Gate Summary

| Gate | Evidence | Status | Exception or notes |
| --- | --- | --- | --- |
| Requirements baseline | `docs/REQUIREMENTS.md`; traceability result in local verification record | Pass | 48/48/48 controlled records; baseline status is separate from target verification |
| C99 clause/facility profile | `docs/c99-conformance-profile.md`; TC-0041 | Pass | 25 clauses, 24 headers, 75 classified rows; ARM64 required behavior cells remain Unknown |
| Architecture and DFS review | `docs/security/design-for-security.md`; ADR-0001--0005 | Unknown | DFS exists but independent approval and native ARM64 review are absent |
| Build and package | local verification and candidate manifest | Fail | Six builds and package structure pass, but WPM verification exits 1 because the package is unsigned |
| Verification and test report | `docs/evidence/c99-t6/local-verification.md` | Unknown | Native x86/x64 pass; native ARM64 required tests, ABI, consumers, and startup are Unknown |
| Header/C89 isolation/ABI | TC-0040/TC-0042 and local verification | Unknown | Header matrix passes all compilers; native ARM64 ABI behavior is not available |
| Windows 2000 x86 imports | Release import JSON summarized in local verification | Pass | Release DLL `f5a6dee9...`, machine `0x014C`, 27 allowlisted `kernel32.dll` imports, zero unexpected |
| Defect and vulnerability review | T6 work log; DFS | Fail | T6-D016 release-trust automation is open; target and signing risks are unapproved |
| Documentation | README, profile, implementation-defined/deviation, ARM64, release-process, DFS | Unknown | Final release notes/document and independent review are absent |
| Provenance and dependency record | `docs/evidence/c99-t6/dependency-provenance.json` | Unknown | Local revisions/executables/actions are pinned; exact candidate CI runner and native ARM64 provenance are unavailable |
| Checksums and exact identities | candidate manifest; local `SHA256SUMS` | Pass | Applies only to the exact unsigned local candidate; it is not a published release set |
| Windows signing and Defender scan | candidate manifest | Fail | DLLs are `NotSigned`; clean Defender results cover unsigned bytes only, not final signed bytes |
| PDF metadata, digest, and provenance | None | Unknown | No project release-document PDF was produced |
| PAdES signature, when selected | Release process | N/A | PAdES is not selected for this Markdown readiness record |
| Installation, rollback, and recovery | `docs/release-process.md` | Unknown | Procedure is documented; clean-system install/remove/rollback was not authorized or executed |
| Support and communication | `docs/release-process.md`; DFS | Pass | GitHub issue/security-advisory intake and 1.x support boundary are documented; no response-time promise |
| Exact-revision CI | No artifact | Unknown | No push or workflow dispatch was authorized |

## Artifacts

| Artifact | Platform or purpose | Integrity value | Publication location |
| --- | --- | --- | --- |
| `wcrt-any-1.0.0-rc.1.17+3fa0b1a3.zip` | Unsigned multi-architecture local candidate | SHA-256 `c1aa305e465d1b93dbb29ba13cfde54aa0472dc356ada450f96e4b225debe812`; 412482 bytes | Local `output/packages/`; not published |
| `SHA256SUMS` | Checksum for the exact local package | SHA-256 `d2d193585588f1aeb50999461b30b60787ca92acec16c63f644ad2bfcee0eaac` | Local `output/release-candidate/1.0.0-rc.1/`; not published |
| x86 Release `wcrt.dll` | Windows 2000 x86 / later x86 | SHA-256 `f5a6dee96bb2c12342bcfe7ed9cd77aca86d88fe0bf328006a470e4c4d365ace`; `NotSigned` | Inside local package only |
| x64 Release `wcrt.dll` | Windows 10/11 x64 | SHA-256 `f9160b9f42531af614867c059e707e713e9d4cdced1f06e0a9f8c94e104fa78d`; `NotSigned` | Inside local package only |
| ARM64 Release `wcrt.dll` | Windows 10/11 ARM64 | SHA-256 `f70c3a65668f5dcb457b56204d8583f158df585844e6c989117582fb16a81dd5`; `NotSigned` | Inside local package only |

The machine-readable candidate manifest also identifies every static library
and startup object. Package metadata reports source version `1.0.0-rc.1`,
package version `1.0.0-rc.1.17+3fa0b1a3`, and short source revision
`3fa0b1a3`; this record binds that short value to the full revision above.

## Open Items and Accepted Risk

No release-blocking item is accepted for publication.

| Item | Impact | Owner | Approval | Completion or review condition |
| --- | --- | --- | --- | --- |
| Native ARM64 candidate verification | Required behavior, ABI, fenv/math/complex, signal/setjmp, consumers, and startup are unproved | Maintainer | None | Native `windows-11-arm` run at the exact source/dependency baseline with retained detailed results and artifact hashes |
| Exact candidate CI/provenance | Local results do not identify hosted runner images or uploaded artifacts | Maintainer | None | Authorized push/dispatch and retained source-quality, x86, x64, ARM64, package, and dependency-provenance artifacts |
| Authenticode publisher identity | Distributed PE publisher and integrity cannot be verified | Maintainer | None | Approve managed/hardware-backed identity; sign every final DLL with SHA-256 and a trusted RFC 3161 SHA-256 timestamp; retain audit output |
| Final-byte Defender evidence | Unsigned scans cannot establish the signed release result | Maintainer | None | Scan each exact signed DLL and final signed package; retain timestamps, hashes, engine/intelligence versions, and no-detection output |
| WPM package signature | `wpm verify` rejects the candidate | Maintainer | None | Sign with protected WPM key after PE signing/scanning; trust public key and obtain `wpm verify` exit 0 on the unchanged package |
| Installation and rollback | Install scripts, registry state, removal, and recovery are unexercised | Maintainer | None | Clean supported-system install/static+DLL consumer/remove/rollback log for the exact signed package on each applicable architecture |
| Release document/PDF | Selected WSP documentation gate has no final release document or provenance | Maintainer | None | Decide the release-document format; generate and validate metadata, manifest, and digest; apply PAdES only if selected |
| Independent project review | Author evidence has not received independent PR review | Reviewer | None | Challenge all Pass cells and disposition each finding at the immutable candidate baseline |
| Stale dependency helper inventory | Two unused bootstrap scripts mention absent miniz/libsodium directories | Maintainer | None | Remove or replace the helpers before relying on them; current workflow provenance must remain authoritative |

## Approval Decision

- **Decision:** Reject
- **Approver:** WCRT maintainer local readiness audit; independent project
  approval absent
- **Date:** 2026-08-17
- **Rationale:** Native ARM64 required evidence is Unknown. The package and all
  DLLs are unsigned, WPM verification fails, final-signed-byte Defender
  evidence does not exist, install/rollback is Unknown, and exact-revision CI
  and independent project review are absent. No combination of local x86/x64
  success and ARM64 cross-compilation converts those gates to Pass.
- **Support or communication actions:** Do not tag, push, publish, create a
  release, upload, or announce availability. Retain this candidate locally for
  corrective verification only.

## Minimum Corrective Commands or Evidence

After separate authorization for external writes, publish the exact source to
a review branch and run `.github/workflows/build.yml` without changing source
or dependency pins. Retain the source-quality, x86, x64, native ARM64,
multi-architecture package, dependency-provenance, startup, consumer, and x86
import artifacts from one successful run. If source changes, assign a new
candidate revision and repeat all gates.

After an approved signing service and identity exist, the required order is:

```powershell
signtool sign /fd SHA256 /tr <approved-rfc3161-url> /td SHA256 <each-wcrt.dll>
signtool verify /pa /all /v <each-wcrt.dll>
./tools/build-wpm-package.ps1 -Version 1.0.0-rc.1 -Configuration Release -SigningKey <protected-temporary-key-path>
wpm trust add release_keys/wpm-release.public
wpm verify <exact-signed-package.zip>
```

Retain full SignTool output, signer subject/thumbprint, timestamp and trust
chain, before/after SHA-256 values, protected-key audit reference, and WPM
verification output. Then run non-remediating Defender scans against the exact
signed DLLs and final signed package and retain the engine/intelligence
versions, timestamps, hashes, exit status, and output. Finally, perform the
documented clean-system install, consumer, remove, and rollback cases.

No command in this section was executed as part of candidate preparation.

## Baseline Record

- **Release tag:** None; no tag was created.
- **Full source revision:**
  `3fa0b1a3a22bbd93ff9185dbb24689d83e057e56`.
- **WSP revision:** `2198ccab08f969a789448767fe7017b774369adc`.
- **Dependency baseline:**
  `docs/evidence/c99-t6/dependency-provenance.json`.
- **Build/test record:** `docs/evidence/c99-t6/local-verification.md`.
- **Artifact record:**
  `docs/evidence/c99-t6/release-candidate-manifest.json`.
- **Published artifact set:** None.
- **Release-document PDF:** None.
