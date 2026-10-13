# Compatibility build tag metadata correction

## Plan, finding and design

Final VC6 evidence review observed packages alternating between 0.0.0 and
1.3.1 despite identical Git baseline 9a2564c. Direct git describe returns
1.3.1/exit zero; the existing Get-WcrtNearestReleaseTag returns 0.0.0 when
called after a failed native command. Its Select-Object -First 1 pipeline can
finish before Git's exit status is retained, leaving the preceding status.
This is a build metadata defect, not a compatibility-runtime failure.
Estimate: 5-15 tool lines, 15-30 minutes. Capture complete native output and
its exit status before selecting the first line. Preserve version-shaped tag
filtering and untagged-repository fallback. No runtime API/import/ABI changes.

Review successful/failed preceding commands, repeated calls, clean untagged
fixture repositories and pinned explicit source versions. Rebuild all three
packages, verify version labels, rerun selected consumer/package checks and
renew the retained DLL hashes. Current compatibility evidence identifies exact
DLL/input hashes; no release qualification was inferred from a version label.
Effort is not reliably timed; record retained verification below.

## Review, verification and closeout

On 2026-10-01 twenty calls following failed Git commands retained tag 1.3.1;
an owned untagged repository returned 0.0.0, and an explicit 2.3.4 source
version remained authoritative. Review found the same early pipeline pattern
in the exact-tag query, corrected with complete output/status capture.
Owned clean exact-tag, tracked dirty-tag and untagged fixtures passed.
An initial dirty fixture used an untracked file, which Git describe does not
count as dirty; the fixture was corrected to edit a tracked file.

All three Release packages rebuilt as 1.3.1+9a2564c5.dirty. TC-0092 native
and packaged x86/x64 consumers passed; ARM64 compiled/linked only. Microsoft
major-16 /Od and /O2 consumers passed against the rebuilt x86 DLL, and all
81 retained input hashes and both executable hashes independently matched.
Windows 2000 x86 imports and 91 traceability mappings passed. The preceding
full native extension aggregates retained 73 passing cases per target.
Genuine VC6/reference qualification remains Unknown. Actual effort was not
reliably timed; the correction changes only build version metadata.
