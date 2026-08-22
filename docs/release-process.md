# WCRT Release, Rollback, and Support Process

**Content type:** Project release-control baseline

**Status:** Implemented candidate and WPM-signing workflow; tagged evidence,
deferred Authenticode/Defender work, and external approval pending

## Release Identity and Scope

A WCRT release is identified by a semantic version, a full Git source
revision, the pinned WSP gitlink, exact build dependencies, and the SHA-256 of
every distributed artifact. A release-candidate version uses
`1.0.0-rc.<number>`. Candidate preparation may commit the exact source revision
before tagged Release evidence exists; it does not create a tag or external
release.

The 1.0.0 artifact set is one `wcrt-any-<package-version>.zip` WPM package
containing x86, x64, and ARM64 headers, DLLs, static libraries, import
definitions, optional console/GUI startup objects, license, README, and C99
capability records;
`SHA256SUMS`; and the release-readiness record. Files not listed with an exact
digest are not approved artifacts.

## Build and Trust Order

1. Freeze and commit a clean source revision. Let WPM resolve the latest
   eligible TinyCC package at workflow execution, retain the exact resolved
   version/source/hashes, and require one identity across every Debug and
   Release target; rerun if the latest entry changes during the workflow.
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
5. Assemble the WPM package without changing the verified Release artifacts.
   Sign the package with the protected WPM release key and verify it using
   `release_keys/wpm-release.public`.
6. Compare every packaged DLL with its Release input, hash the final package
   and release documents, validate their identities,
   and approve the exact set in the release-readiness record.
7. Permit the downstream publication job to create the GitHub Release and
   upload only after every Release architecture and WPM-package verification
   succeeds and the exact set is approved.

The repository workflow orders `build` -> `release` -> `package` -> `publish`,
so a failed or cancelled architecture or WPM package job prevents publication.
Authenticode identity/timestamping and Defender scanning were explicitly
deferred from WCRT 1.0.0 on 2026-08-21. ADR-0006 and
`docs/windows-signing-plan.md` retain the dormant Azure design for future
reassessment; they are not 1.0 gates and provide no signature evidence. The
workflow is fail-closed for the remaining 1.0 controls, but it does not provide
a post-artifact approval checkpoint. Until all active controls are exercised
and approved in the release-readiness record, the 1.0.0 release decision is
Reject and no tag may be pushed. An unsigned-DLL local candidate is preparation
evidence only.

## Verification Commands

The exact readiness record supplies resolved paths. Typical local checks are:

```powershell
Get-FileHash -Algorithm SHA256 <artifact>
wpm trust add release_keys/wpm-release.public
wpm verify <package.zip>
./tools/test-wpm-package-authenticode.ps1 -Package <package.zip> -SkipAuthenticode -BuildRoot <release-build-root>
```

The package identity check proves only that WPM assembly retained the Release
DLL bytes. It does not prove Authenticode trust or malware-scan status; both
controls are Deferred for 1.0, never Pass.

The distinct `WPM_RELEASE_PRIVATE_KEY` secret remains only for WPM package
signing. The currently repository-scoped WPM secret must be migrated into the
protected `release` environment before a tag. No Authenticode key or Azure
identity is required for 1.0. See the deferred signing plan before any future
reactivation.

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
