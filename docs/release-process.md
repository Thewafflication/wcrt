# WCRT Release, Rollback, and Support Process

**Content type:** Project release-control baseline

**Status:** Implemented candidate process; signing infrastructure and external
approval pending

## Release Identity and Scope

A WCRT release is identified by a semantic version, a full Git source
revision, the pinned WSP gitlink, exact build dependencies, and the SHA-256 of
every distributed artifact. A release-candidate version uses
`1.0.0-rc.<number>`. Candidate preparation does not create a tag or external
release.

The 1.0.0 artifact set is one `wcrt-any-<package-version>.zip` WPM package
containing x86, x64, and ARM64 headers, DLLs, static libraries, import
definitions, optional console/GUI startup objects, license, README, and C99
capability records; `SHA256SUMS`; and the release-readiness record. Files not
listed with an exact digest are not approved artifacts.

## Build and Trust Order

1. Freeze a clean source revision and pinned dependency baseline.
2. Build Debug verification outputs and Release x86/x64/ARM64 outputs.
3. Run all controlled tests, consumers, startup, ABI, source-quality,
   traceability, import, and evidence checks on the exact baseline.
4. Finalize PE version resources and manifests.
5. Authenticode-sign every distributed `wcrt.dll` with the approved publisher
   identity and an RFC 3161 SHA-256 timestamp. Verify the signature and trust
   chain independently.
6. Record SHA-256 values, then scan those exact signed PE files with supported
   Microsoft Defender security intelligence. Retain scan time, result, engine
   version, intelligence version, and the matching digests.
7. Assemble the WPM package without changing signed PEs. Sign the package with
   the protected WPM release key and verify it using
   `release_keys/wpm-release.public`.
8. Hash the final package and release documents, validate their identities,
   and approve the exact set in the release-readiness record.
9. Only after separate explicit authorization, create the tag, push, create the
   release, and upload the approved bytes.

The repository workflow currently builds and WPM-signs tagged artifacts but
does not provide an approved Authenticode identity/timestamp stage or retained
Defender gate. Until those controls are implemented and exercised, the 1.0.0
release decision is Reject. An unsigned local candidate is preparation
evidence only.

## Verification Commands

The exact readiness record supplies resolved paths. Typical local checks are:

```powershell
Get-AuthenticodeSignature <wcrt.dll>
Get-FileHash -Algorithm SHA256 <artifact>
wpm trust add release_keys/wpm-release.public
wpm verify <package.zip>
Get-MpComputerStatus
Start-MpScan -ScanType CustomScan -ScanPath <exact-final-artifact>
```

The last scan is a release Pass only for final signed bytes with retained
Defender versions and a matching digest. `NotSigned`, unavailable Defender,
or a scan of earlier bytes is Unknown or Fail as specified by the readiness
record, never Pass.

## Installation and Rollback

Before approval, install the exact package into a clean supported test system,
compile and run a static and DLL consumer, verify `WCRT_HOME`, and remove the
package. Confirm removal of the versioned installation directory and removal
or restoration of `WCRT_HOME`.

Rollback means installing the last approved WCRT version after uninstalling
the affected version. Because the package installs into a versioned directory,
the previous bytes remain independently identifiable. If no earlier approved
version exists, uninstall WCRT and rebuild consumers with their prior runtime.
Do not overwrite or relabel a published artifact. A withdrawn release retains
its incident record and digests; external package/release removal or marking
requires explicit maintainer authorization.

Minimum rollback evidence is a clean-system install/remove log for the exact
package, the previous approved version or no-previous-version statement, the
environment-variable result, and a consumer result before and after rollback.

## Support and Communication

GitHub issues are the public support and defect channel. Security-sensitive
reports should use a private repository security advisory when available.
The maintainer owns triage, affected-version analysis, release/rollback
decisions, and user communication. WCRT makes no service-level or response-time
promise.

The current major release and its documented compatibility commitments are
supported until an end-of-support notice is published. Notices for a withdrawn
or superseded release identify affected versions and artifact digests,
recommended action, replacement version when available, and any compatibility
impact.
