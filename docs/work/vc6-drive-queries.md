# Microsoft drive query tranche

## Plan, design and verification

REQ-0084 begins the missing drive family with read-only queries. Estimate:
25-35 implementation lines, 80-110 test lines. Dependencies: existing current
directory provider, legacy MAX_PATH model and GetLogicalDrives. Return drive
letters independently of case and preserve errno on non-letter prefixes;
keep OS buffer/failure partitions explicit. _getdrives forwards the logical
drive snapshot without opening or enumerating any drive contents.

Verify native query agreement and unchanged cwd, all letter prefixes,
UNC/non-letter prefixes, OS failure and exact buffer boundaries using
substitutes. Require native/package x86/x64, ARM64 cross-links, directory
regressions, quality/traceability and Windows 2000 imports. The roadmap drive
family is narrowed to these queries for this tranche; switching, per-drive
state and long paths remain gaps. Genuine VC6 qualification remains Unknown.

## Closeout

On 2026-10-01 TC-0084 passed native x86/x64 real and deterministic tests,
plus static/DLL consumers without host CRT imports. ARM64 source/error and
consumer cross-links passed; native execution remains Unknown. TC-0064/0080
directory regressions passed on x86/x64. Source quality passed for 307 files,
traceability passed for 83 requirements and Windows 2000 imports passed with
the added GetLogicalDrives entry. No production defect was found; review
removed an unnecessary file fixture from these read-only tests. Size remained
within estimate; elapsed effort was not reliably measured. Genuine VC6 and
drive switching/per-drive state remain open.
