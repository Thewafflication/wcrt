# WCRT Release, Rollback, and Support Process

**Content type:** Project release-control baseline

**Status:** Implemented candidate process; signing infrastructure and external
approval pending

## Release Identity and Scope

A WCRT release is identified by a semantic version, a full Git source
revision, the pinned WSP gitlink, exact build dependencies, and the SHA-256 of
every distributed artifact. A release-candidate version uses
`1.0.0-rc.<number>`. Candidate preparation may commit the exact source revision
before tagged Release evidence exists; it does not create a tag or external
release.

The 1.0.0 artifact set is one `wcrt-any-<package-version>.zip` WPM package
containing x86, x64, and ARM64 headers, DLLs, static libraries, import
definitions, the ARM64 TinyCC complex-ABI companion archive, optional
console/GUI startup objects, license, README, and C99 capability records;
`SHA256SUMS`; and the release-readiness record. Files not listed with an exact
digest are not approved artifacts.

## Build and Trust Order

1. Freeze and commit a clean source revision and pinned dependency baseline.
2. Build Debug verification outputs and run the complete controlled C89, C99,
   compatibility, consumer, startup, ABI, source-quality, traceability, import,
   package-assembly, and evidence matrix on x86, x64, and ARM64.
3. Finalize PE version resources, manifests, documentation, and the proposed
   release-readiness record at that committed candidate revision.
4. After all other prepublication blockers are closed and separate tag/
   publication authority is granted, push the semantic-version tag. The tag
   starts optimized Release builds and native library-consumer/startup smoke
   tests on x86, x64, and ARM64 and repeats the x86 legacy-import check. The
   full behavior inventory is not duplicated because each Release job depends
   on the successful exact-source Debug matrix.
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
9. Permit the downstream publication job to create the GitHub Release and
   upload only after every Release architecture and signed-package verification
   succeeds and the exact set is approved.

The repository workflow already orders `build` -> `release` -> `package` ->
`publish`, so a failed or cancelled architecture or package job prevents
publication. It currently builds and WPM-signs tagged artifacts but does not
provide an approved Authenticode identity/timestamp stage, retained Defender
gate, or a post-artifact approval checkpoint. Until those controls are
implemented and exercised, the 1.0.0 release decision is Reject and no tag may
be pushed. An unsigned local candidate is preparation evidence only.

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
