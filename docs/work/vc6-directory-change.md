# Microsoft directory change/removal tranche

## Plan and estimate

REQ-0080 adds narrow `_chdir` and `_rmdir` in direct.h. Dependencies: existing
directory backend, Windows 2000 path APIs and errno. Estimate: 80-130 runtime
lines and 100-160 test lines, 45-90 minutes. Additive interface and two baseline
Windows imports; existing _mkdir/_getcwd remain regression obligations.

## Design and verification plan

Call SetCurrentDirectoryA and RemoveDirectoryA. Preclassify current/root paths
in the selected MAX_PATH model so removal reports ENOTEMPTY without attempting
their deletion. Map missing and not-directory errors explicitly; preserve
errno on success. No POSIX aliases, wide forms, drive switching or code-page
state added. Tests own temporary directories, restore the child cwd, and remove
only their files/empty directories. Verify declarations, relative/absolute
paths, null/missing/file inputs, nonempty/current directory, success and errno.
Run x86/x64, ARM64 links, package consumers, import audit, source quality and
traceability. Genuine VC6 reference behavior remains Unknown.

## Closeout

On 2026-10-01 TC-0080 passed native x86/x64 filesystem and deterministic
OS-substitute cases. Packaged static/DLL consumers passed on x86/x64 without
host CRT imports. ARM64 sources/consumers compiled/linked; native execution
remains Unknown. TC-0064 regression passed, the expanded Windows 2000 x86
DLL import allowlist passed, and source quality passed for 298 files after
the sharing tranche was added. Traceability passes for 80 requirements.

Review corrected syntactic UNC root classification to require actual directory
attributes; an absent share must report missing rather than protected-root
status. Empty removal input is handled explicitly. Source-quality review
corrected three overlong test lines. These were removed before closeout.
Runtime size is about 100 lines, within estimate; focused time was not reliably
recorded. Genuine VC6 qualification and native ARM64 remain open evidence.
