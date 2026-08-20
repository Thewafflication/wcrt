# ADR-0006 — Managed Authenticode identity and timestamp service

**Status:** Accepted for release automation; Azure identity provisioning and
first exact-subject evidence pending

**Date:** 2026-08-20

## Context

WCRT distributes one `wcrt.dll` for each of x86, x64, and ARM64. The WPM
package signature authenticates the package envelope but is not an
Authenticode signature and cannot supply a Windows publisher identity. The
existing `WPM_RELEASE_PRIVATE_KEY` is a WPM-format private key, not an X.509
code-signing identity, and shall not be repurposed.

Publicly trusted code-signing keys require protected use. Storing an
exportable PFX and its password as GitHub secrets would expose a copyable
long-lived private key and would not meet the selected WSP protected-identity
control. The signing design also has to cover ARM64 output even though the
signing action executes on a hosted x64 Windows runner.

## Decision

- The approved logical Authenticode publisher is **Jordan Waughtal**, the
  WCRT maintainer named by the package metadata. Windows `VERSIONINFO`
  `CompanyName` uses the same value.
- Microsoft Azure Artifact Signing **Public Trust**, Basic tier, is the
  approved managed signing service. Azure performs identity validation,
  protects the short-lived code-signing private key in managed HSM-backed
  infrastructure, and supplies the public-trust certificate profile. Creating
  the paid Azure resource and completing identity validation require a
  separate account-side action.
- The exact X.509 subject returned by the validated certificate profile is not
  guessed. It is frozen in the GitHub `release` environment secret
  `AUTHENTICODE_EXPECTED_SUBJECT`; every signature must match it exactly. The
  first qualifying signing run records the subject and rotating certificate
  thumbprint. Certificate rotation does not authorize subject drift.
- GitHub authenticates to Azure through OpenID Connect. No Azure client secret,
  PFX, private certificate, or Authenticode private key is stored in GitHub.
  `AZURE_CLIENT_ID`, `AZURE_TENANT_ID`, and `AZURE_SUBSCRIPTION_ID` identify the
  federated principal; the principal receives only the Artifact Signing
  certificate-profile signer role needed for the selected profile.
- The GitHub `release` environment must enforce an approved tag and reviewer/
  bypass policy before signing values are installed. Its 2026-08-20 state has
  no protection rules and is not approved for credential provisioning. The
  workflow reaches that environment only after the native Release jobs pass.
- The approved RFC 3161 service is
  `http://timestamp.acs.microsoft.com`, with SHA-256 for both file and
  timestamp digests. SHA-1 and a Windows 2000 dual-signing exception are not
  selected.
- The workflow signs the final x86, x64, and ARM64 DLLs after optimized build
  and native smoke verification. It uses the immutable action revisions
  `Azure/login@d54469830ea7d513b7371e02a077c3ee5cb7b112` and
  `Azure/artifact-signing-action@c0ae2c1d0c1847ab81ac0ab8521bee597cfedd30`.
  An x64 Windows signing job may sign all three PE machine types because it
  does not execute their target code.
- PowerShell Authenticode inspection and Windows SDK SignTool independently
  verify every signed DLL. The x86 legacy-import gate is repeated after
  signing. Packaging consumes only the verified signed artifacts, WPM-signs
  the package with the separate existing WPM key, requires `wpm verify` exit
  zero, then extracts and re-verifies all three unchanged DLLs.
- Defender remains a distinct deferred control. Neither a successful
  signature nor a zero-exit WPM verification is Defender evidence or release
  approval.

## Consequences

The repository now contains a fail-closed `build -> release -> sign -> package
-> publish` dependency chain and retained JSON trust evidence. A missing Azure
value, unprovisioned profile, unexpected subject, absent timestamp, invalid
trust chain, changed packaged DLL, or nonzero `wpm verify` result blocks the
downstream job.

The source design can be reviewed and committed before the external identity
exists. Until Azure identity validation, certificate-profile creation, RBAC,
GitHub OIDC federation, environment values, and an exact tagged run are
complete, every actual Authenticode and signed-package result remains
`Unknown`; this ADR is not signature evidence.

Azure owns certificate issuance and renewal. On suspected principal or signing
service compromise, disable the federated credential and certificate profile,
stop releases, preserve Azure and GitHub audit records, identify every signed
digest in the affected interval, request revocation when applicable, rotate
the principal/profile, and require a new release-readiness decision.

## References

- [Microsoft Windows code-signing options](https://learn.microsoft.com/en-us/windows/apps/package-and-deploy/code-signing-options)
- [Azure Artifact Signing overview](https://learn.microsoft.com/en-us/azure/artifact-signing/overview)
- [Azure Artifact Signing integrations](https://learn.microsoft.com/en-us/azure/artifact-signing/how-to-signing-integrations)
- [CA/Browser Forum Code Signing Baseline Requirements](https://cabforum.org/working-groups/code-signing/requirements/)
