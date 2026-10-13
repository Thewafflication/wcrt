# Microsoft drive directory tranche

## Plan and design

REQ-0085 adds selected _chdrive/_getdcwd adapters within the roadmap drive
family. Estimate: 35-50 implementation lines, 100-160 test lines. Reuse
current-directory, drive mask, full-path and current-directory-change APIs.
Keep current/zero drive queries on _getcwd and resolve another drive's dot
path only after checking its logical-drive bit. Validate size and drive before
allocation or mutation. Windows owns drive-relative state; CRT environment
bookkeeping remains the explicitly excluded REQ-0080 gap.

## Verification plan

Use one owned directory for current-drive selection and verify restoration.
Use deterministic OS substitutes for available/unavailable alternate drives,
remembered paths, query errors and all invalid input partitions. Require
native/package x86/x64, ARM64 cross-links, TC-0064/0080/0084 regressions,
quality, traceability and Windows 2000 imports. No actual alternate-drive
mutation or persistent environment changes occur in the tests. Genuine VC6
qualification and full remembered-drive behavior remain Unknown.

## Closeout

On 2026-10-01 TC-0085 passed native x86/x64 real and deterministic OS tests,
plus static/DLL consumers without host CRT imports. ARM64 source/error and
consumer cross-links passed; native execution remains Unknown. The
supplementary Microsoft compiler major 16 x86 DLL consumer also passed with
current-drive selection/query calls. TC-0064/0080/0084 regressions passed on
x86/x64; source quality passed for 310 files, traceability for 84 requirements,
and the Windows 2000 import audit passed with no new imports.

Review removed an unsafe continuation if the owned fixture's cwd query fails
before its output buffer is initialized. The deterministic fixture injects
allocation failure and confirms ENOMEM, queries an inactive drive without
actual drive access, and covers validation before mutation. No production
failure was found. Size is within estimate; effort was not reliably measured.
CRT per-drive bookkeeping and genuine VC6 qualification remain explicit gaps.
