# WCRT Windows Signing Plan

**Content type:** Controlled release signing plan

**Status:** Approved design; external Azure identity and exercised evidence
pending

**Decision:** ADR-0006

## Scope and Identity

Every Release `wcrt.dll` distributed in the WPM package is in scope:

| Architecture | Build input | Package entry |
| --- | --- | --- |
| x86 | `output/build/x86/Release/wcrt.dll` | `x86/bin/wcrt.dll` |
| x64 | `output/build/x64/Release/wcrt.dll` | `x64/bin/wcrt.dll` |
| ARM64 | `output/build/arm64/Release/wcrt.dll` | `arm64/bin/wcrt.dll` |

The approved logical publisher and Windows version-resource `CompanyName` are
`Jordan Waughtal`. The exact Azure-validated certificate subject must be
copied without normalization into `AUTHENTICODE_EXPECTED_SUBJECT`. It remains
Unknown until Azure completes identity validation and issues the first
qualifying certificate. A changed subject requires an ADR and readiness
impact review; a normal short-lived certificate/thumbprint rotation does not.

Static libraries, import definitions, startup objects, headers, JSON profile
records, and Markdown files are not PE files and do not receive Authenticode
signatures. The final WPM ZIP receives the separate WPM signature.

## Services and Access

- **Signing service and trust model:** Microsoft Azure Artifact Signing Public
  Trust, Basic tier.
- **Timestamp service:** `http://timestamp.acs.microsoft.com`, RFC 3161,
  SHA-256 timestamp digest.
- **File digest:** SHA-256.
- **Responsible role:** WCRT maintainer owns Azure identity validation,
  certificate-profile configuration, GitHub environment protection, release
  authorization, incident response, and release evidence. GitHub Actions may
  request signatures only for an authorized tag through the `release`
  environment after its protection rules are approved and configured.
- **Azure access:** a GitHub OIDC federated principal with the Artifact Signing
  certificate-profile signer role scoped to the approved profile. No Azure
  client secret or exportable Authenticode key is selected.
- **Package access:** the separate `WPM_RELEASE_PRIVATE_KEY` GitHub environment
  secret is materialized only as a temporary file for `wpm build --sign`, then
  deleted. It cannot be reused for Authenticode.

Required GitHub `release` environment secrets are:

| Secret | Purpose |
| --- | --- |
| `AZURE_CLIENT_ID` | OIDC federated application or managed-identity client ID |
| `AZURE_TENANT_ID` | Azure tenant containing the signing account |
| `AZURE_SUBSCRIPTION_ID` | Azure subscription containing the signing account |
| `ARTIFACT_SIGNING_ENDPOINT` | Region-specific Artifact Signing account endpoint |
| `ARTIFACT_SIGNING_ACCOUNT_NAME` | Approved Artifact Signing account |
| `ARTIFACT_SIGNING_CERTIFICATE_PROFILE` | Approved Public Trust certificate profile |
| `AUTHENTICODE_EXPECTED_SUBJECT` | Exact validated X.509 signer subject |
| `WPM_RELEASE_PRIVATE_KEY` | Existing, separate WPM package-signing key; migrate from repository scope |

Identifiers are stored as environment secrets at the maintainer's request;
they are not Authenticode private-key material. GitHub OIDC requires
`id-token: write` only in the signing job. The federated subject must be
restricted to this repository and the `release` environment.
Its expected GitHub subject is
`repo:Thewafflication/wcrt:environment:release`.

The repository API inspection on 2026-08-20 reports that `release` currently
has no protection rules or deployment branch/tag policy and permits admin
bypass. Do not provision the Azure or WPM values into that environment until a
tag policy and required-review/bypass policy are explicitly approved. The
workflow deliberately places the first `environment: release` boundary on the
`sign` job, after all architecture-specific Release tests have succeeded.
The same inspection found `WPM_RELEASE_PRIVATE_KEY` only at repository-secret
scope. Re-enter it as a `release` environment secret, verify the environment
copy, and remove the repository-scoped copy before an authorized tag.

## Required Order and Verification

1. The complete Debug matrix passes at the candidate revision.
2. Tagged optimized x86, x64, and ARM64 Release jobs build final PE content and
   pass native consumers/startup; x86 also passes the legacy-import gate.
3. The signing job records unsigned SHA-256 identities and requests one
   SHA-256 Authenticode signature and RFC 3161 SHA-256 timestamp for each DLL.
4. `tools/test-authenticode-signatures.ps1` checks every DLL with
   `Get-AuthenticodeSignature` and Windows SDK SignTool `/pa /all /v`. It
   requires valid Windows policy, the exact subject, Code Signing EKU, a
   timestamp certificate, SHA-256 confirmation, and matching `CompanyName`.
5. The signed x86 DLL repeats the Windows 2000 import inspection. Signing must
   not add an imported DLL or function.
6. Only verified signed build artifacts enter packaging. WPM signs the package
   with its separate key; `wpm verify` output and exact exit code are recorded,
   and any result other than zero fails the job.
7. `tools/test-wpm-package-authenticode.ps1` extracts all three DLLs, compares
   their SHA-256 values with the signed inputs, and repeats complete
   Authenticode verification.
8. Defender scanning of the exact signed bytes and package remains Unknown
   under the explicitly deferred design. Publication is not approved until
   the release-readiness record dispositions that separate gate.

The workflow pins Azure Login and Artifact Signing actions by full commit. A
dependency update requires source review, a workflow-gate test update, and an
impact note. A warning, missing file, missing tool, missing value, unavailable
service, invalid or untrusted signature, absent timestamp, subject mismatch,
hash mismatch, or missing evidence is not Pass.

## Retained Evidence

The tag run must retain:

- source revision, tag, workflow/run/job identities, runner image, action
  commits, Azure signing account/profile identifiers, and timestamp URL;
- unsigned and signed DLL names, architecture, size, product version,
  `CompanyName`, SHA-256, signer subject/issuer/thumbprint/validity, Code
  Signing EKU, timestamp subject/issuer/thumbprint/validity, Authenticode
  status, and full SignTool output/exit status;
- signed x86 Windows 2000 import JSON;
- packaged-versus-signed DLL hash comparisons and repeated signature output;
- final package name, size, SHA-256, WPM public-key SHA-256, full `wpm verify`
  output, and exit code zero; and
- Azure/GitHub audit references required by the release-readiness decision.

GitHub logs and named evidence artifacts provide run evidence. Exact final
identities are copied into the project release-readiness record before
approval. The project-wide evidence retention duration remains a separately
deferred WSP control.

## Certificate Lifecycle and Incident Response

Azure manages short-lived certificate issuance and normal renewal. The
workflow pins identity by exact subject and records each issued thumbprint.
Identity validation or certificate-profile expiration, suspension, or failure
blocks signing and remains Unknown until restored and reverified.

On suspected compromise or unauthorized use:

1. Stop tag and release activity and disable the GitHub federated credential
   or its Artifact Signing role.
2. Disable the certificate profile/account as appropriate and preserve Azure
   and GitHub audit evidence.
3. Enumerate every signature request, certificate thumbprint, artifact digest,
   tag, and publication during the affected interval.
4. Request revocation and notify affected users when the investigation finds
   a certificate or identity compromise.
5. Replace the federated principal/profile, update the expected subject only
   through an approved identity-change ADR, and rerun the entire signing,
   packaging, verification, and readiness sequence.

No previously signed artifact is relabeled or silently replaced. Withdrawal
and rollback follow `docs/release-process.md`.
